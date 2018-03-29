#include "SmoothPath.h"

std::vector<Zoner::SmoothPathWaypoint> Zoner::SmoothPath::_waypoints_cache;
std::vector<Zoner::SmoothPathObstacle> Zoner::SmoothPath::_obstacles_cache;

Zoner::SmoothPathWaypoint::SmoothPathWaypoint() : position(0.f,0.f,0.f), tangent(1.f,0.f,0.f) {}
Zoner::SmoothPathWaypoint::SmoothPathWaypoint(glm::vec3 _position) : position(_position), tangent(1.f, 0.f, 0.f) {}
Zoner::SmoothPathWaypoint::SmoothPathWaypoint(glm::vec3 _position, glm::vec3 _tangent) : position(_position), tangent(_tangent) {}

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

void Zoner::SmoothPath::BeginWaypointsCollection(float seg_length)
{
	_seg_length = seg_length;
	_total_length = 0;

	_waypoints.clear();
	_waypoints_cache.clear();
}

void Zoner::SmoothPath::EndWaypointsCollection()
{
	_CalculateTangents();
}

void Zoner::SmoothPath::BuildPassage(glm::vec2 from, glm::vec2 from_direction, glm::vec2 to, std::vector<Zoner::SmoothPathObstacle*>& obstacles, ok::graphics::LineBatch& batch)
{
	_waypoints_cache.clear();

	bool mirrored = false;

	glm::vec2 to_memory = to;
	
	glm::vec2 from_to_direction = to - from;
	glm::vec2 from_direction_nrm = glm::vec2(from_direction.y, -from_direction.x);

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

	float rotation_radius = glm::clamp(from_to_distance, 1.f, 50.f);
	float oriented_angle = glm::degrees(glm::orientedAngle(from_direction, from_to_direction));

	glm::vec2 rotation_center = from - from_direction_nrm * rotation_radius;


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

	//*********************************

	//build path with obstacles avoidance
	//all waypoints created here must be created at waypoints cache!
	{
		//Formiruem uporyadochenniy spisok peresekaemih prepyatstviy 
		_obstacles_cache.clear();

		for (auto&& obstacle : obstacles)
		{
			if (true == _CircleToLineIntesectionCheck(obstacle->position, obstacle->radius, arc_end, to))
			{
				_obstacles_cache.push_back(*obstacle);
			}
		}

		auto sort_obstacles_function = [to](const Zoner::SmoothPathObstacle& left, const Zoner::SmoothPathObstacle& right) {
			return glm::length2(left.position - to) < glm::length2(right.position - to);
		};

		std::sort(_obstacles_cache.begin(), _obstacles_cache.end(), sort_obstacles_function);

		glm::vec2 path_point = to;

		bool in_flip = false;
		bool out_flip = true;

		for (auto&& obstacle : _obstacles_cache)
		{
			glm::vec2 in = _CircleToPointTangent(obstacle.position, obstacle.radius, path_point, in_flip);
			glm::vec2 out = _CircleToPointTangent(obstacle.position, obstacle.radius, arc_end, out_flip);

			in_flip = !in_flip;
			out_flip = !out_flip;

			//create waypoints from line between path_point and in (store in waypoints cache)
			_CollectWaypointsFromLineSegment(path_point, in, true);
			//create waypoints from arc between in and out (store in waypoints cache)
			_CollectWaypointsFromArc(obstacle.position, obstacle.radius, in, out, true);

			path_point = out;
		}

		to = path_point;
	}

	//*************************************

	//create waypoints from arc between from and arc_end
	if (mirrored)
	_CollectWaypointsFromArc(rotation_center, rotation_radius, from, arc_end,false, true, false);
	else
	_CollectWaypointsFromArc(rotation_center, rotation_radius, from, arc_end, false, false, true);
	//create waypoints from line between arc_end and to
	_CollectWaypointsFromLineSegment(arc_end, to);
	//append waypoints from waypoints cache (filled from obstacles avoidance stage)
	_waypoints.reserve(_waypoints.size() + _waypoints_cache.size());
	_waypoints.insert(_waypoints.end(), _waypoints_cache.rbegin(), _waypoints_cache.rend());

	//Otrisovka debug
	/*batch.SetBrushThickness(2.f);
	batch.SetBrushSoftness(0.0001f);

	batch.BatchBegin();

	batch.SetBrushColor(ok::Color::Gray);
	batch.Circle(glm::vec3(rotation_center, 0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), rotation_radius, 5.f);

	batch.SetBrushColor(ok::Color::Red);
	batch.MoveTo(glm::vec3(from, 0.f));
	batch.LineTo(glm::vec3(rotation_center, 0.f));

	batch.SetBrushColor(ok::Color::Green);
	batch.LineTo(glm::vec3(arc_end, 0.f));

	batch.SetBrushColor(ok::Color::Blue);
	batch.LineTo(glm::vec3(to, 0.f));

	batch.BatchEnd();*/
}

glm::vec2 Zoner::SmoothPath::_CircleToPointTangent(glm::vec2 circle_center, float circle_radius, glm::vec2 point, bool flip_side)
{
	float c = glm::length(circle_center - point);
	float a = circle_radius;
	float b = glm::sqrt(c*c - a*a);

	if (a > c) b = 1.0f;

	float sina = a / c;
	float ang_a = glm::degrees(glm::asin(glm::clamp(sina, -1.f, 1.f)));

	return point + b*glm::normalize(glm::rotate(circle_center - point, ((flip_side == true) ? -1.f : 1.f) * glm::radians(ang_a)));
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

	collection.push_back(Zoner::SmoothPathWaypoint(glm::vec3(arc_begin, 0.f)));

	arc_begin = arc_begin - circle_center;

	float _step_angle;

	while (arc_length > 0.f)
	{
		_step_angle = glm::min(seg_angle, arc_angle);

		arc_begin = glm::rotate(arc_begin, rotation_sign * glm::radians(_step_angle));
		_total_length += _seg_length;

		collection.push_back(Zoner::SmoothPathWaypoint(glm::vec3(circle_center + arc_begin, 0.f)));
		arc_length -= _seg_length;
		arc_angle -= _step_angle;
	}

	//collection.push_back(Zoner::SmoothPathWaypoint(glm::vec3(arc_end, 0.f)));
	//_total_length += (_seg_length + arc_length);
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

	collection.push_back(Zoner::SmoothPathWaypoint(glm::vec3(line_begin, 0.f)));

	float line_step = 0.f;

	float _step_len;

	while (line_len > 0.f)
	{
		_step_len = glm::min(_seg_length, line_len);
		line_begin += line_dir * _step_len;
		_total_length += _step_len;

		collection.push_back(Zoner::SmoothPathWaypoint(glm::vec3(line_begin, 0.f)));
		line_len -= _step_len;
	}

	//collection.push_back(Zoner::SmoothPathWaypoint(glm::vec3(line_end, 0.f)));
	//_total_length += (_seg_length + line_len);
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

/*void Zoner::SmoothPath::Build(glm::vec3 from, glm::vec3 from_direction, glm::vec3 to, std::vector<Zoner::SmoothPathObstacle*>& obstacles, ok::graphics::LineBatch& batch)
{
bool mirrored = false;

glm::vec3 to_memory = to;

glm::vec2 from_to_direction = glm::vec2(to - from);
glm::vec2 from_direction_nrm = glm::vec2(from_direction.y, -from_direction.x);

if (glm::dot(from_direction_nrm, from_to_direction) > 0.f)
{
mirrored = true;

float dot_x = glm::dot(glm::vec2(from_direction), from_to_direction);
float dot_y = glm::dot(-from_direction_nrm, from_to_direction);

from_to_direction = glm::vec2(from_direction) * dot_x + from_direction_nrm * dot_y;
to = from + glm::vec3(from_to_direction, 0.f);
}


float from_to_distance = glm::length(from_to_direction);
from_to_direction = glm::normalize(from_to_direction);

float rotation_radius = glm::clamp(from_to_distance, 1.f, 50.f);
float oriented_angle = glm::degrees(glm::orientedAngle(glm::vec2(from_direction), glm::vec2(from_to_direction)));
float oriented_angle_delta = glm::sign(-oriented_angle)*(180.0f - glm::abs(oriented_angle));

glm::vec2 rotation_center = glm::vec2(from) - glm::vec2(from_direction.y, -from_direction.x) * rotation_radius;

float c = glm::length(rotation_center - glm::vec2(to));
float a = rotation_radius;
float b = glm::sqrt(c*c - a*a);

float sina = a / c;
float ang_a = glm::degrees(glm::asin(sina));
float ang_b = 90.0f - ang_a;

glm::vec2 arc_end = (glm::vec2(to) + b*glm::normalize(glm::rotate(rotation_center - glm::vec2(to), glm::radians(ang_a)))) - glm::vec2(from);

if (mirrored)
{
glm::vec2 nrm = glm::vec2(from_direction.y, -from_direction.x);

float dot_x = glm::dot(glm::vec2(from_direction), arc_end);
float dot_y = glm::dot(-nrm, arc_end);

arc_end = glm::vec2(from_direction) * dot_x + nrm * dot_y;

to = to_memory;
rotation_center = glm::vec2(from) - (rotation_center - glm::vec2(from));
}

arc_end += glm::vec2(from);


batch.SetBrushThickness(2.f);
batch.SetBrushSoftness(0.0001f);

batch.BatchBegin();

batch.SetBrushColor(ok::Color::Gray);
batch.Circle(glm::vec3(rotation_center,0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), rotation_radius, 5.f);

batch.SetBrushColor(ok::Color::Red);
batch.MoveTo(from);
batch.LineTo(glm::vec3(rotation_center,0.f));

batch.SetBrushColor(ok::Color::Green);
batch.LineTo(glm::vec3(arc_end, 0.f));

batch.SetBrushColor(ok::Color::Blue);
batch.LineTo(to);

batch.BatchEnd();
}*/
