#pragma once

#include "..\..\Spline.h"
#include "..\..\LineBatch.h"

namespace Zoner
{
	class SmoothPathWaypoint
	{
	public:
		glm::vec3 position;
		glm::vec3 tangent_in;
		glm::vec3 tangent_out;
		float pick;

		SmoothPathWaypoint();
		SmoothPathWaypoint(glm::vec3 _position);
		SmoothPathWaypoint(glm::vec3 _position, glm::vec3 _tangent);
	};

	class SmoothPath
	{
	public:
		Zoner::SmoothPathWaypoint GetWaypoint(float path_normalized_position);
		void BeginWaypointsCollection();
		void CollectWaypoint(glm::vec3 position);
		void EndWaypointsCollection(float normalization_step_length);
		float Length();
		void Clear();

		static void DrawDebug(ok::graphics::LineBatch& line_batch, Zoner::SmoothPath& path);
	protected:
	private:
		std::vector<Zoner::SmoothPathWaypoint> waypoints;
		static std::vector<Zoner::SmoothPathWaypoint> _waypoints_cache;
		float length = 0;
	};
}