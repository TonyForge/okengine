#include "SmoothPath.h"

std::vector<Zoner::SmoothPathWaypoint> Zoner::SmoothPath::_waypoints_cache;

bool Zoner::SmoothPath::_pfs_mirrored;
bool Zoner::SmoothPath::_pfs_confluent_arc;
glm::vec2 Zoner::SmoothPath::_pfs_rotation_center;
float Zoner::SmoothPath::_pfs_rotation_radius;

Zoner::SmoothPathWaypoint::SmoothPathWaypoint() : position(0.f,0.f,0.f), tangent(1.f,0.f,0.f) {}
Zoner::SmoothPathWaypoint::SmoothPathWaypoint(glm::vec3 _position) : position(_position), tangent(1.f, 0.f, 0.f) {}
Zoner::SmoothPathWaypoint::SmoothPathWaypoint(glm::vec3 _position, glm::vec3 _tangent) : position(_position), tangent(_tangent) {}

bool Zoner::SmoothPathWaypoint::IsEqualTo(const Zoner::SmoothPathWaypoint & waypoint)
{
	return 
		(glm::abs(position.x - waypoint.position.x) < 1.f) &&
		(glm::abs(position.y - waypoint.position.y) < 1.f) &&
		(glm::abs(position.z - waypoint.position.z) < 1.f);
}

Zoner::SmoothPathWaypoint Zoner::SmoothPath::Pick(float pick)
{
	if (_waypoints.size() == 0) return Zoner::SmoothPathWaypoint();

	float path_position = static_cast<float>(_waypoints.size() - 1) * pick;
	size_t waypoint_index = static_cast<size_t>(glm::floor(path_position));
	size_t waypoint_index_post = glm::clamp(static_cast<int>(waypoint_index + 1), 0, static_cast<int>(_waypoints.size() - 1));

	float path_piece_progress = (path_position - waypoint_index);

	Zoner::SmoothPathWaypoint result;
	Zoner::SmoothPathWaypoint& waypoint = _waypoints[waypoint_index];
	Zoner::SmoothPathWaypoint& waypoint_post = _waypoints[waypoint_index_post];

	result.position = glm::lerp(waypoint.position, waypoint_post.position, path_piece_progress);
	result.tangent = glm::lerp(waypoint.tangent, waypoint_post.tangent, path_piece_progress);

	return result;
}

void Zoner::SmoothPath::BeginWay(glm::vec2 begin_position, glm::vec2 begin_direction, float seg_length)
{
	_seg_length = seg_length;
	_total_length = 0;

	_waypoints.clear();
	_waypoints_cache.clear();

	_waypoints_cache.push_back(Zoner::SmoothPathWaypoint(glm::vec3(begin_position,0.f), glm::vec3(begin_direction, 0.f)));
}

void Zoner::SmoothPath::AdvanceWay(glm::vec2 next_position)
{
	if (glm::length2(next_position - glm::vec2(_waypoints_cache[_waypoints_cache.size() - 1].position)) > 1.f)
	{
		glm::vec2 arc_end = _CalculatePassageFirstSection(_waypoints_cache[_waypoints_cache.size() - 1].position, _waypoints_cache[_waypoints_cache.size() - 1].tangent, next_position);
		glm::vec2 next_direction = glm::normalize(next_position - arc_end);

		_waypoints_cache.push_back(Zoner::SmoothPathWaypoint(glm::vec3(arc_end, _pfs_mirrored == true ? 1.f : 0.f), glm::vec3(next_direction, _pfs_rotation_center.x)));
		_waypoints_cache.push_back(Zoner::SmoothPathWaypoint(glm::vec3(next_position, _pfs_rotation_center.y), glm::vec3(next_direction, (_pfs_confluent_arc == true) ? -1.f : _pfs_rotation_radius)));
	}
}

void Zoner::SmoothPath::EndWay()
{
	for (size_t i = 1; i < _waypoints_cache.size() - 1; i+=2)
	{
		Zoner::SmoothPathWaypoint& wp0 = _waypoints_cache[i - 1];
		Zoner::SmoothPathWaypoint& wp1 = _waypoints_cache[i];
		Zoner::SmoothPathWaypoint& wp2 = _waypoints_cache[i + 1];

		if (wp2.tangent.z < 0.f)
		{
			_CollectWaypointsFromLineSegment(wp0.position, wp1.position, false);
		}
		else
		{
			if (wp1.position.z == 1.f) //if (_pfs_mirrored)
				 //_CollectWaypointsFromArc(_pfs_rotation_center, _pfs_rotation_radius, from, arc_end, false, true, false);
				_CollectWaypointsFromArc(glm::vec2(wp1.tangent.z, wp2.position.z), wp2.tangent.z, wp0.position, wp1.position, false, true, false);
			else
				//_CollectWaypointsFromArc(_pfs_rotation_center, _pfs_rotation_radius, from, arc_end, false, false, true);
				_CollectWaypointsFromArc(glm::vec2(wp1.tangent.z, wp2.position.z), wp2.tangent.z, wp0.position, wp1.position, false, false, true);
		}

			
		_CollectWaypointsFromLineSegment(wp1.position, wp2.position, false);
	}

	_CalculateTangents();
}

float Zoner::SmoothPath::Length()
{
	return _total_length;
}

void Zoner::SmoothPath::Clear()
{
	_waypoints.clear();
	_waypoints_cache.clear();

	_total_length = 0.f;
}

glm::vec2 Zoner::SmoothPath::_LineToLineIntersection(glm::vec2 line_a_start, glm::vec2 line_a_end, glm::vec2 line_b_start, glm::vec2 line_b_end)
{
	float denominator = (line_a_start.x - line_a_end.x) * (line_b_start.y - line_b_end.y) - (line_a_start.y - line_a_end.y) * (line_b_start.x - line_b_end.x);

	if (denominator != 0.f)
	{
		float px =
			(line_a_start.x * line_a_end.y - line_a_start.y * line_a_end.x) * (line_b_start.x - line_b_end.x) -
			(line_a_start.x - line_a_end.x) * (line_b_start.x * line_b_end.y - line_b_start.y * line_b_end.x);

		px /= denominator;

		float py =
			(line_a_start.x * line_a_end.y - line_a_start.y * line_a_end.x) * (line_b_start.y - line_b_end.y) -
			(line_a_start.y - line_a_end.y) * (line_b_start.x * line_b_end.y - line_b_start.y * line_b_end.x);

		py /= denominator;

		return glm::vec2(px, py);
	}

	return glm::vec2();
}

glm::vec2 Zoner::SmoothPath::_CircleToPointTangent(glm::vec2 circle_center, float circle_radius, glm::vec2 point, bool flip_side)
{
	float c = glm::length(circle_center - point);
	float a = circle_radius;
	if (c == a)
	{
		c += 1.0f;
	}
	float b = glm::sqrt(c*c - a*a);

	if (a > c) b = 1.0f;

	float sina = a / c;
	float ang_a = glm::degrees(glm::asin(glm::clamp(sina, -1.f, 1.f)));

	return point + b*glm::normalize(glm::rotate(circle_center - point, ((flip_side == true) ? -1.f : 1.f) * glm::radians(ang_a)));
}

Zoner::SmoothPath::_Tangent2D Zoner::SmoothPath::_CircleToCircleTangent(glm::vec2 from_circle_center, float from_circle_circle_radius, glm::vec2 to_circle_center, float to_circle_circle_radius, bool outside, bool flip_side)
{
	if (from_circle_circle_radius < to_circle_circle_radius)
	{
		if (outside)
		{
			to_circle_circle_radius -= from_circle_circle_radius;
			glm::vec2 tangent = _CircleToPointTangent(to_circle_center, to_circle_circle_radius, from_circle_center, !flip_side);

			glm::vec2 nrm = glm::normalize(tangent - from_circle_center);
			nrm = glm::vec2(nrm.y, -nrm.x) * from_circle_circle_radius;

			if (flip_side == true)
			{
				tangent -= nrm;
				from_circle_center -= nrm;
			}
			else
			{
				tangent += nrm;
				from_circle_center += nrm;
			}

			Zoner::SmoothPath::_Tangent2D result;
			result.begin = from_circle_center;
			result.end = tangent;

			return result;
		}
		else
		{
			to_circle_circle_radius += from_circle_circle_radius;
			glm::vec2 tangent = _CircleToPointTangent(to_circle_center, to_circle_circle_radius, from_circle_center, !flip_side);

			glm::vec2 nrm = glm::normalize(tangent - from_circle_center);
			nrm = glm::vec2(nrm.y, -nrm.x) * from_circle_circle_radius;

			if (flip_side == true)
			{
				tangent += nrm;
				from_circle_center += nrm;
			}
			else
			{
				tangent -= nrm;
				from_circle_center -= nrm;
			}

			Zoner::SmoothPath::_Tangent2D result;
			result.begin = from_circle_center;
			result.end = tangent;

			return result;
		}
	}
	else if (from_circle_circle_radius > to_circle_circle_radius)
	{
		if (outside)
		{
			from_circle_circle_radius -= to_circle_circle_radius;
			glm::vec2 tangent = _CircleToPointTangent(from_circle_center, from_circle_circle_radius, to_circle_center, flip_side);

			glm::vec2 nrm = glm::normalize(tangent - to_circle_center);
			nrm = glm::vec2(nrm.y, -nrm.x) * to_circle_circle_radius;

			if (flip_side == false)
			{
				tangent -= nrm;
				to_circle_center -= nrm;
			}
			else
			{
				tangent += nrm;
				to_circle_center += nrm;
			}

			Zoner::SmoothPath::_Tangent2D result;
			result.begin = tangent;//to_circle_center;
			result.end = to_circle_center;//tangent;

			return result;
		}
		else
		{
			from_circle_circle_radius += to_circle_circle_radius;
			glm::vec2 tangent = _CircleToPointTangent(from_circle_center, from_circle_circle_radius, to_circle_center, !flip_side);

			glm::vec2 nrm = glm::normalize(tangent - to_circle_center);
			nrm = glm::vec2(nrm.y, -nrm.x) * to_circle_circle_radius;

			if (flip_side == true)
			{
				tangent += nrm;
				to_circle_center += nrm;
			}
			else
			{
				tangent -= nrm;
				to_circle_center -= nrm;
			}

			Zoner::SmoothPath::_Tangent2D result;
			result.begin = tangent;//to_circle_center;
			result.end = to_circle_center;//tangent;

			return result;
		}
	}
	else if (from_circle_circle_radius == to_circle_circle_radius)
	{
		if (outside)
		{
			Zoner::SmoothPath::_Tangent2D result;
			result.begin = from_circle_center;
			result.end = to_circle_center;
			glm::vec2 nrm = glm::normalize(result.end - result.begin);
			nrm = glm::vec2(nrm.y, -nrm.x) * from_circle_circle_radius;
			if (flip_side == false)
			{
				result.begin += nrm;
				result.end += nrm;
			}
			else
			{
				result.begin -= nrm;
				result.end -= nrm;
			}

			return result;
		}
		else
		{
			to_circle_circle_radius += from_circle_circle_radius;
			glm::vec2 tangent = _CircleToPointTangent(to_circle_center, to_circle_circle_radius, from_circle_center, !flip_side);

			glm::vec2 nrm = glm::normalize(tangent - from_circle_center);
			nrm = glm::vec2(nrm.y, -nrm.x) * from_circle_circle_radius;

			if (flip_side == false)
			{
				tangent -= nrm;
				from_circle_center -= nrm;
			}
			else
			{
				tangent += nrm;
				from_circle_center += nrm;
			}

			Zoner::SmoothPath::_Tangent2D result;
			result.begin = from_circle_center;
			result.end = tangent;

			return result;
		}
	}

	return Zoner::SmoothPath::_Tangent2D();
}

glm::vec2 Zoner::SmoothPath::_CalculatePassageFirstSection(const glm::vec2 & from, const glm::vec2 & from_direction, glm::vec2 to)
{
	bool& mirrored = _pfs_mirrored;
	mirrored = false;

	glm::vec2 from_to_direction = to - from;
	glm::vec2 to_memory = to;
	glm::vec2 from_direction_nrm = glm::vec2(from_direction.y, -from_direction.x);

	if (glm::dot(from_direction, glm::normalize(from_to_direction)) > 0.95f)
	{
		_pfs_confluent_arc = true;
	}
	else
	{
		_pfs_confluent_arc = false;
	}
	//Esli tochka "to" ne v nignei poluploskosti, to otzerkalivaem ee
	if (glm::dot(from_direction_nrm, from_to_direction) > 0.f)
	{
		mirrored = true;

		float dot_x = glm::dot(from_direction, from_to_direction);
		float dot_y = glm::dot(-from_direction_nrm, from_to_direction);

		from_to_direction = from_direction * dot_x + from_direction_nrm * dot_y;
		to = from + from_to_direction;
	}

	//Nahodim centr okrugnosti vokrug kotorogo proishodit povorot
	float from_to_distance = glm::length(from_to_direction);
	from_to_direction = glm::normalize(from_to_direction);

	float& rotation_radius = _pfs_rotation_radius;
	rotation_radius = glm::clamp(from_to_distance, 1.f, 50.f);
	float oriented_angle = glm::degrees(glm::orientedAngle(from_direction, from_to_direction));

	glm::vec2& rotation_center = _pfs_rotation_center;
	rotation_center = from - from_direction_nrm * rotation_radius;


	//Esli tochka "to" sligkom blizko k centry okrugnosti, vichislyaem noviy radius po formule opisannoi vokrug treugolnika okrugnosti
	if (glm::length(rotation_center - to) < rotation_radius)
	{
		float triangle_a = glm::abs(glm::dot(to - from, from_direction))*2.f;
		float triangle_b = from_to_distance;
		float triangle_p = (triangle_a + triangle_b + triangle_b) * 0.5f;

		rotation_radius = (triangle_a * triangle_b * triangle_b) / (4.f * glm::sqrt(triangle_p * (triangle_p - triangle_a) * (triangle_p - triangle_b) * (triangle_p - triangle_b)));
		rotation_center = from - glm::vec2(from_direction.y, -from_direction.x) * rotation_radius;
	}


	//Nahodim tochku iz kotoroi kasatelnaya uhodit tochno d tochku "to"
	glm::vec2 arc_end = _CircleToPointTangent(rotation_center, rotation_radius, to) - from;

	//Esli "to" otzerkalivali, to otzerkalivaem obratno vse reshenie
	if (mirrored)
	{
		glm::vec2 nrm = glm::vec2(from_direction.y, -from_direction.x);

		float dot_x = glm::dot(from_direction, arc_end);
		float dot_y = glm::dot(-nrm, arc_end);

		arc_end = from_direction * dot_x + nrm * dot_y;

		to = to_memory;
		rotation_center = from - (rotation_center - from);
	}

	arc_end += from;

	return arc_end;
}

bool Zoner::SmoothPath::_CircleToLineIntesectionCheck(glm::vec2 circle_center, float circle_radius, glm::vec2 line_begin, glm::vec2 line_end)
{
	glm::vec2 dir = line_end - line_begin;
	float line_len = glm::length(dir);
	dir /= line_len;

	float projection = glm::dot(circle_center - line_begin, dir);
	glm::vec2 d = line_begin + dir * projection;

	float d_proj = glm::dot(d - line_begin, line_end - line_begin);

	if (d_proj >= 0.f && d_proj <= line_len*line_len)
	if (glm::length2(circle_center - d) <= circle_radius*circle_radius)
		return true;

	return false;
}

void Zoner::SmoothPath::_CollectWaypointsFromArc(glm::vec2 circle_center, float circle_radius, glm::vec2 arc_begin, glm::vec2 arc_end, bool use_cache, bool force_ccw, bool force_cw)
{
	std::vector<Zoner::SmoothPathWaypoint>* _waypoints_ptr = nullptr;

	if (use_cache)
	{
		_waypoints_ptr = &_waypoints_cache;
	}
	else
	{
		_waypoints_ptr = &_waypoints;
	}

	std::vector<Zoner::SmoothPathWaypoint>& collection = *_waypoints_ptr;

	float arc_angle = glm::degrees(glm::orientedAngle(glm::normalize(arc_begin - circle_center), glm::normalize(arc_end - circle_center)));

	float rotation_sign = glm::sign(arc_angle);
	if (force_cw)
	{
		if (rotation_sign < 0.f)
		{
			arc_angle = glm::abs(360.0f + arc_angle);
		}

		rotation_sign = 1.f;
	}
		
	if (force_ccw)
	{
		if (rotation_sign > 0.f)
		{
			arc_angle = -glm::abs(360.0f - arc_angle);
		}

		rotation_sign = -1.f;
	}
		
	arc_angle = glm::abs(arc_angle);
	float arc_length = (glm::pi<float>() * circle_radius * arc_angle) / 180.f;
	float seg_angle = (_seg_length * 180.f) / (glm::pi<float>() * circle_radius);

	Zoner::SmoothPathWaypoint waypoint = Zoner::SmoothPathWaypoint(glm::vec3(arc_begin, 0.f));

	if (collection.size() == 0 || collection.size() > 0 && false == collection[collection.size() - 1].IsEqualTo(waypoint))
	collection.push_back(waypoint);

	arc_begin = arc_begin - circle_center;

	float _step_angle;

	while (arc_length > 0.f)
	{
		_step_angle = glm::min(seg_angle, arc_angle);

		arc_begin = glm::rotate(arc_begin, rotation_sign * glm::radians(_step_angle));
		_total_length += _seg_length;

		waypoint = Zoner::SmoothPathWaypoint(glm::vec3(circle_center + arc_begin, 0.f));

		if (collection.size() == 0 || collection.size() > 0 && false == collection[collection.size() - 1].IsEqualTo(waypoint))
		collection.push_back(waypoint);

		arc_length -= _seg_length;
		arc_angle -= _step_angle;
	}
}

void Zoner::SmoothPath::_CollectWaypointsFromLineSegment(glm::vec2 line_begin, glm::vec2 line_end, bool use_cache)
{
	std::vector<Zoner::SmoothPathWaypoint>* _waypoints_ptr = nullptr;

	if (use_cache)
	{
		_waypoints_ptr = &_waypoints_cache;
	}
	else
	{
		_waypoints_ptr = &_waypoints;
	}

	std::vector<Zoner::SmoothPathWaypoint>& collection = *_waypoints_ptr;

	glm::vec2 line_dir = line_end - line_begin;
	float line_len = glm::length(line_dir);
	line_dir = glm::normalize(line_dir);

	Zoner::SmoothPathWaypoint waypoint = Zoner::SmoothPathWaypoint(glm::vec3(line_begin, 0.f));

	if (collection.size() == 0 || collection.size() > 0 && false == collection[collection.size()-1].IsEqualTo(waypoint))
	collection.push_back(waypoint);

	float line_step = 0.f;

	float _step_len;

	while (line_len > 0.f)
	{
		_step_len = glm::min(_seg_length, line_len);
		line_begin += line_dir * _step_len;
		_total_length += _step_len;

		waypoint = Zoner::SmoothPathWaypoint(glm::vec3(line_begin, 0.f));

		if (collection.size() == 0 || collection.size() > 0 && false == collection[collection.size() - 1].IsEqualTo(waypoint))
		collection.push_back(waypoint);
		line_len -= _step_len;
	}
}

void Zoner::SmoothPath::_CalculateTangents()
{
	int index_pre;
	int index_post;

	int index = 0;

	for (auto& waypoint : _waypoints)
	{
		index_pre = glm::clamp(index - 1, 0, static_cast<int>(_waypoints.size() - 1));
		index_post = glm::clamp(index + 1, 0, static_cast<int>(_waypoints.size() - 1));

		Zoner::SmoothPathWaypoint& waypoint_pre = _waypoints[index_pre];
		Zoner::SmoothPathWaypoint& waypoint_post = _waypoints[index_post];

		if (index == 0)
		{
			waypoint.tangent = glm::normalize(waypoint_post.position - waypoint.position);
		}
		else
			if (index == index_post)
			{
				waypoint.tangent = waypoint_pre.tangent;
			}
			else
			{
				waypoint.tangent = glm::normalize(waypoint_post.position - waypoint_pre.position);
			}

		index++;
	}
}

void Zoner::SmoothPath::_RescaleCircle(glm::vec2 & circle_center, float & circle_radius, glm::vec2 scale_hotspot, float scale)
{
	circle_center = scale_hotspot + (circle_center - scale_hotspot) * scale;
	circle_radius *= scale;
}
