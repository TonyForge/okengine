#include "SmoothPath.h"

Starpom::SmoothPathWaypoint Starpom::SmoothPath::GetWaypoint(float pick)
{
	if (waypoints.size() == 0) return Starpom::SmoothPathWaypoint();

	float path_position = static_cast<float>(waypoints.size()-1) * pick;
	size_t waypoint_index = static_cast<size_t>(glm::floor(path_position));
	size_t waypoint_index_post = glm::clamp(static_cast<int>(waypoint_index + 1), 0, static_cast<int>(waypoints.size() - 1));

	float path_piece_progress = (path_position - waypoint_index);

	Starpom::SmoothPathWaypoint result;
	Starpom::SmoothPathWaypoint& waypoint = waypoints[waypoint_index];
	Starpom::SmoothPathWaypoint& waypoint_post = waypoints[waypoint_index_post];

	result.position = glm::lerp(waypoint.position, waypoint_post.position, path_piece_progress);
	result.tangent_out = glm::lerp(waypoint.tangent_out, waypoint_post.tangent_in, path_piece_progress);
	result.tangent_in = -result.tangent_out;

	return result;
}

void Starpom::SmoothPath::BeginWaypointsCollection()
{
	waypoints.clear();
}

void Starpom::SmoothPath::CollectWaypoint(glm::vec3 position)
{
	waypoints.push_back(Starpom::SmoothPathWaypoint(position));
}

void Starpom::SmoothPath::EndWaypointsCollection(float normalization_step_length)
{
	float total_distance = 0.f;

	int index = 0;
	int index_pre;
	int index_post;

	//Calculate tangents and total distance
	{
		for (auto& waypoint : waypoints)
		{
			index_pre = glm::clamp(index - 1, 0, static_cast<int>(waypoints.size() - 1));
			index_post = glm::clamp(index + 1, 0, static_cast<int>(waypoints.size() - 1));

			Starpom::SmoothPathWaypoint& waypoint_pre = waypoints[index_pre];
			Starpom::SmoothPathWaypoint& waypoint_post = waypoints[index_post];

			glm::vec3 auto_tangent = waypoint_post.position - waypoint_pre.position;
			float in_tangent_length = glm::sqrt(glm::abs(glm::dot(waypoint.position - waypoint_pre.position, auto_tangent)));
			float out_tangent_length = glm::sqrt(glm::abs(glm::dot(waypoint.position - waypoint_post.position, -auto_tangent)));

			auto_tangent = glm::normalize(auto_tangent);

			//in tangent
			waypoint.tangent_in = auto_tangent * in_tangent_length;
			//out tangent
			waypoint.tangent_out = auto_tangent * out_tangent_length;

			if (index == 0)
			{
				waypoint.tangent_in = waypoint.tangent_out;
			}

			if (index == index_post)
			{
				waypoint.tangent_out = waypoint.tangent_in;
			}

			if (index < static_cast<int>(waypoints.size() - 1))
			{
				total_distance += glm::length(waypoint.position - waypoint_post.position);
			}

			index++;
		}
	}

	//Calculate sparse picks
	{
		index = 0;

		float waypoint_pick = 0;

		for (auto& waypoint : waypoints)
		{
			index_post = glm::clamp(index + 1, 0, static_cast<int>(waypoints.size() - 1));
			Starpom::SmoothPathWaypoint& waypoint_post = waypoints[index_post];

			waypoint.pick = waypoint_pick;

			waypoint_pick += glm::length(waypoint.position - waypoint_post.position) / total_distance;

			index++;
		}
	}

	Starpom::SmoothPathWaypoint end_waypoint = waypoints[waypoints.size() - 1];
	end_waypoint.tangent_in = glm::normalize(end_waypoint.tangent_in);
	end_waypoint.tangent_out = glm::normalize(end_waypoint.tangent_out);

	//Hermit curve linearization (STAGE 1)
	{
		index = 0;

		int steps = static_cast<int>(glm::ceil(total_distance / normalization_step_length)) + 1;
		float picks_per_step = 1.f / static_cast<float>(steps - 1);

		float pick = 0;

		std::vector<Starpom::SmoothPathWaypoint>& normalized_path = _waypoints_cache;
		normalized_path.clear();
		normalized_path.reserve(steps);

		for (auto& waypoint : waypoints)
		{
			index_post = glm::clamp(index + 1, 0, static_cast<int>(waypoints.size() - 1));
			Starpom::SmoothPathWaypoint& waypoint_post = waypoints[index_post];

			while (pick >= waypoint.pick && pick <= waypoint_post.pick)
			{
				float local_pick = (pick - waypoint.pick) / (waypoint_post.pick - waypoint.pick);

				normalized_path.push_back(
					Starpom::SmoothPathWaypoint(
						ok::Spline::HermitPick(waypoint.position, waypoint.tangent_out, waypoint_post.position, waypoint_post.tangent_in, local_pick),
						glm::normalize(ok::Spline::HermitTangentPick(waypoint.position, waypoint.tangent_out, waypoint_post.position, waypoint_post.tangent_in, local_pick))
					)
				);
				normalized_path[normalized_path.size() - 1].pick = pick;

				pick += picks_per_step;
			}

			index++;
		}

		normalized_path.push_back(end_waypoint);

		waypoints = normalized_path;
	}

	//Calculate linearized curve total distance
	{
		index = 0;
		total_distance = 0;

		float waypoint_pick = 0;

		for (auto& waypoint : waypoints)
		{
			index_post = glm::clamp(index + 1, 0, static_cast<int>(waypoints.size() - 1));
			Starpom::SmoothPathWaypoint& waypoint_post = waypoints[index_post];

			total_distance += glm::length(waypoint.position - waypoint_post.position);

			index++;
		}
	}

	//Recalculate sparse picks
	{
		index = 0;

		float waypoint_pick = 0;

		for (auto& waypoint : waypoints)
		{
			index_post = glm::clamp(index + 1, 0, static_cast<int>(waypoints.size() - 1));
			Starpom::SmoothPathWaypoint& waypoint_post = waypoints[index_post];

			waypoint.pick = waypoint_pick;

			waypoint_pick += glm::length(waypoint.position - waypoint_post.position) / total_distance;

			index++;
		}
	}

	//Linearized curve resample (STAGE 2)
	{
		index = 0;

		int steps = static_cast<int>(glm::ceil(total_distance / normalization_step_length)) + 1;
		float picks_per_step = 1.f / static_cast<float>(steps - 1);

		float pick = 0;

		std::vector<Starpom::SmoothPathWaypoint>& normalized_path = _waypoints_cache;
		normalized_path.clear();
		normalized_path.reserve(steps);

		for (auto& waypoint : waypoints)
		{
			index_post = glm::clamp(index + 1, 0, static_cast<int>(waypoints.size() - 1));
			Starpom::SmoothPathWaypoint& waypoint_post = waypoints[index_post];

			while (pick >= waypoint.pick && pick <= waypoint_post.pick)
			{
				float local_pick = (pick - waypoint.pick) / (waypoint_post.pick - waypoint.pick);

				normalized_path.push_back(Starpom::SmoothPathWaypoint(
					glm::lerp(waypoint.position, waypoint_post.position, local_pick),
					glm::lerp(waypoint.tangent_out, waypoint_post.tangent_in, local_pick)));

				normalized_path[normalized_path.size() - 1].pick = pick;

				pick += picks_per_step;
			}

			index++;
		}

		waypoints = normalized_path;
	}

	if (glm::length(waypoints[waypoints.size()-1].position - end_waypoint.position) >= normalization_step_length*0.5f)
	waypoints.push_back(end_waypoint);
}

void Starpom::SmoothPath::DrawDebug(ok::graphics::LineBatch & line_batch, Starpom::SmoothPath & path)
{
	line_batch.SetBrushThickness(4.f);
	line_batch.SetBrushSoftness(0.01f);

	line_batch.BatchBegin();

	line_batch.SetBrushColor(ok::Color(0.f, 0.f, 1.f, 1.f));

	line_batch.MoveTo(path.waypoints[0].position);
	for (auto&& waypoint : path.waypoints)
	{
		line_batch.SetBrushColor(ok::Color(glm::fract(waypoint.position.x), glm::fract(waypoint.position.y), glm::fract(waypoint.position.z), 1.f));
		line_batch.LineTo(waypoint.position);
	}
	
	line_batch.SetBrushThickness(1.f);

	for (auto&& waypoint : path.waypoints)
	{
		line_batch.SetBrushColor(ok::Color(1.f, 1.f, 0.f, 1.f));
		line_batch.MoveTo(waypoint.position);
		line_batch.LineTo(waypoint.position - waypoint.tangent_in);

		line_batch.SetBrushColor(ok::Color(0.f, 1.f, 0.f, 1.f));
		line_batch.MoveTo(waypoint.position);
		line_batch.LineTo(waypoint.position + waypoint.tangent_out);
	}
	
	line_batch.BatchEnd();
}

Starpom::SmoothPathWaypoint::SmoothPathWaypoint() : position(0.f,0.f,0.f), tangent_in(0.f,0.f,0.f), tangent_out(0.f, 0.f, 0.f), pick(0.f)
{
}

Starpom::SmoothPathWaypoint::SmoothPathWaypoint(glm::vec3 _position) : position(_position), tangent_in(0.f, 0.f, 0.f), tangent_out(0.f, 0.f, 0.f), pick(0.f)
{
}

Starpom::SmoothPathWaypoint::SmoothPathWaypoint(glm::vec3 _position, glm::vec3 _tangent) : position(_position), tangent_in(_tangent), tangent_out(_tangent), pick(0.f)
{
}
