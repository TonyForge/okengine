#include "SmoothPath.h"

Starpom::SmoothPathWaypoint Starpom::SmoothPath::GetWaypoint(float pick)
{
	if (waypoints.size() == 0) return Starpom::SmoothPathWaypoint();

	float path_position = static_cast<float>(waypoints.size() - 1) * pick;

	size_t waypoint_index = static_cast<size_t>(glm::floor(path_position));
	size_t waypoint_index_pre = glm::clamp(static_cast<int>(waypoint_index - 1), 0, static_cast<int>(waypoints.size() - 1));
	size_t waypoint_index_post = glm::clamp(static_cast<int>(waypoint_index + 1), 0, static_cast<int>(waypoints.size() - 1));

	float path_position_pre = static_cast<float>(waypoint_index_pre);
	float path_position_post = static_cast<float>(waypoint_index_post);

	float path_piece_progress = (path_position - path_position_pre) / (path_position_post - path_position_pre);

	Starpom::SmoothPathWaypoint& waypoint = waypoints[waypoint_index];
	Starpom::SmoothPathWaypoint& waypoint_pre = waypoints[waypoint_index_pre];
	Starpom::SmoothPathWaypoint& waypoint_post = waypoints[waypoint_index_post];

	Starpom::SmoothPathWaypoint result;

	if (path_piece_progress < 0.5f)
	{
		result.position = ok::Spline::HermitPick(waypoint_pre.position, waypoint_pre.tangent, waypoint.position, waypoint.tangent, path_piece_progress * 2.0f);
		result.tangent = ok::Spline::HermitTangentPick(waypoint_pre.position, waypoint_pre.tangent, waypoint.position, waypoint.tangent, path_piece_progress * 2.0f);
	}
	else
	{
		result.position = ok::Spline::HermitPick(waypoint.position, waypoint.tangent, waypoint_post.position, waypoint_post.tangent, (path_piece_progress - 0.5f) * 2.0f);
		result.tangent = ok::Spline::HermitTangentPick(waypoint.position, waypoint.tangent, waypoint_post.position, waypoint_post.tangent, (path_piece_progress - 0.5f) * 2.0f);
	}

	return result;
}

void Starpom::SmoothPath::BeginWaypointsCollection()
{
	waypoints.clear();
}

void Starpom::SmoothPath::CollectWaypoint(glm::vec3 position, glm::vec3 tangent)
{
	waypoints.push_back(Starpom::SmoothPathWaypoint(position, tangent));
}

void Starpom::SmoothPath::CollectWaypoint(glm::vec3 position)
{
	waypoints.push_back(Starpom::SmoothPathWaypoint(position, glm::vec3(0.f,0.f,0.f)));
}

void Starpom::SmoothPath::EndWaypointsCollection(float normalization_step_length)
{
	float total_distance = 0.f;

	int index = 0;
	int index_pre;
	int index_post;

	//Calculate undefined normals
	{
		for (auto& waypoint : waypoints)
		{
			index_pre = glm::clamp(index - 1, 0, static_cast<int>(waypoints.size() - 1));
			index_post = glm::clamp(index + 1, 0, static_cast<int>(waypoints.size() - 1));

			Starpom::SmoothPathWaypoint& waypoint_pre = waypoints[index_pre];
			Starpom::SmoothPathWaypoint& waypoint_post = waypoints[index_post];

			if (waypoint.tangent.x == 0.f && waypoint.tangent.y == 0.f && waypoint.tangent.z == 0.f)
			{
				glm::vec3 tangent_pre = waypoint_pre.tangent;
				glm::vec3 tangent_post = waypoint_post.tangent;

				if (tangent_pre.x == tangent_pre.y == tangent_pre.z == 0.f)
				{
					tangent_pre = waypoint.position - waypoint_pre.position;
				}

				if (tangent_pre.x == tangent_pre.y == tangent_pre.z == 0.f)
				{
					tangent_pre = waypoint_post.position - waypoint_pre.position;
				}

				if (tangent_pre.x == tangent_pre.y == tangent_pre.z == 0.f)
				{
					tangent_pre.y = 1.f;
				}

				tangent_pre = glm::normalize(tangent_pre);

				if (tangent_post.x == tangent_post.y == tangent_post.z == 0.f)
				{
					tangent_post = waypoint_post.position - waypoint.position;
				}

				if (tangent_post.x == tangent_post.y == tangent_post.z == 0.f)
				{
					tangent_post = tangent_pre;
				}

				tangent_post = glm::normalize(tangent_post);

				waypoint.tangent = ok::Spline::HermitTangentPick(waypoint_pre.position, tangent_pre, waypoint_post.position, tangent_post,0.5f);
			}

			if (index < waypoints.size() - 1)
			{
				float distance = glm::length(waypoint.position - waypoint_post.position);
				total_distance += distance;
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

	//Move step by step along Hermit curve based on waypoints and build normalized path
	{
		index = 0;

		int steps = static_cast<int>(glm::ceil(total_distance / normalization_step_length)) + 1;
		float picks_per_step = 1.f / static_cast<float>(steps - 1);

		float pick = 0;

		std::vector<Starpom::SmoothPathWaypoint> normalized_path;
		normalized_path.reserve(steps);

		for (auto& waypoint : waypoints)
		{
			index_post = glm::clamp(index + 1, 0, static_cast<int>(waypoints.size() - 1));
			Starpom::SmoothPathWaypoint& waypoint_post = waypoints[index_post];

			while (pick >= waypoint.pick && pick <= waypoint_post.pick)
			{
				float local_pick = (pick - waypoint.pick) / (waypoint_post.pick - waypoint.pick);

				normalized_path.push_back(Starpom::SmoothPathWaypoint(
					ok::Spline::HermitPick(waypoint.position, waypoint.tangent, waypoint_post.position, waypoint_post.tangent, local_pick),
					ok::Spline::HermitTangentPick(waypoint.position, waypoint.tangent, waypoint_post.position, waypoint_post.tangent, local_pick)));

				normalized_path[normalized_path.size() - 1].pick = pick;

				pick += picks_per_step;
			}

			index++;
		}

		Starpom::SmoothPathWaypoint last_waypoint = normalized_path[normalized_path.size() - 1];

		while (normalized_path.size() != steps)
		{
			last_waypoint.pick = pick;
			normalized_path.push_back(last_waypoint);

			pick += picks_per_step;
		}

		waypoints = normalized_path;
	}
}

Starpom::SmoothPathWaypoint::SmoothPathWaypoint() : position(0.f,0.f,0.f), tangent(0.f,1.f,0.f)
{
}

Starpom::SmoothPathWaypoint::SmoothPathWaypoint(glm::vec3 _position, glm::vec3 _tangent) : position(_position), tangent(_tangent)
{
}

Starpom::SmoothPathWaypoint::SmoothPathWaypoint(glm::vec3 _position) : position(_position), tangent(0.f, 0.f, 0.f)
{
}
