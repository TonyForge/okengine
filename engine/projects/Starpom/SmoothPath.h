#pragma once

#include "..\..\Spline.h"

namespace Starpom
{
	class SmoothPathWaypoint
	{
	public:
		glm::vec3 position;
		glm::vec3 tangent;
		float pick;

		SmoothPathWaypoint();
		SmoothPathWaypoint(glm::vec3 _position, glm::vec3 _tangent);
		SmoothPathWaypoint(glm::vec3 _position);
	};

	class SmoothPath
	{
	public:
		Starpom::SmoothPathWaypoint GetWaypoint(float path_normalized_position);
		void BeginWaypointsCollection();
		void CollectWaypoint(glm::vec3 position, glm::vec3 tangent);
		void CollectWaypoint(glm::vec3 position);
		void EndWaypointsCollection(float normalization_step_length);
	protected:
	private:
		std::vector<Starpom::SmoothPathWaypoint> waypoints;
	};
}