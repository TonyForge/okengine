#pragma once

#include "..\..\Spline.h"
#include "..\..\LineBatch.h"

namespace Zoner
{
	class SmoothPathObstacle
	{
	public:
		glm::vec2 position;
		float radius;
	};

	class SmoothPathWaypoint
	{
	public:
		SmoothPathWaypoint();
		SmoothPathWaypoint(glm::vec3 _position);
		SmoothPathWaypoint(glm::vec3 _position, glm::vec3 _tangent);

		glm::vec3 position;
		glm::vec3 tangent;
	};

	class SmoothPath
	{
	public:
		Zoner::SmoothPathWaypoint Pick(float pick);

		void BeginWaypointsCollection(float seg_length);
		void EndWaypointsCollection();

		/*void BeginWaypointsCollection();
		void CollectWaypoint(glm::vec2 position, glm::vec2 direction);
		void EndWaypointsCollection();

		void BuildFromWaypoints(std::vector<Zoner::SmoothPathObstacle*>& obstacles);
		void BuildFromEllipse(glm::vec2 center, glm::vec2 axis_x, glm::vec2 axis_y);*/

		void BuildPassage(glm::vec2 from, glm::vec2 from_direction, glm::vec2 to, std::vector<Zoner::SmoothPathObstacle*>& obstacles, ok::graphics::LineBatch& batch);

		//void Build(glm::vec3 from, glm::vec3 from_direction, glm::vec3 to, std::vector<Zoner::SmoothPathObstacle*>& obstacles, ok::graphics::LineBatch& batch);

		
	private:
		std::vector<Zoner::SmoothPathWaypoint> _waypoints;
		static std::vector<Zoner::SmoothPathWaypoint> _waypoints_cache;
		static std::vector<Zoner::SmoothPathObstacle> _obstacles_cache;

		//circle_center, arc_begin, arc_end is in world space
		bool _CircleToLineIntesectionCheck(glm::vec2 circle_center, float circle_radius, glm::vec2 line_begin, glm::vec2 line_end);
		void _CollectWaypointsFromArc(glm::vec2 circle_center, float circle_radius, glm::vec2 arc_begin, glm::vec2 arc_end, bool use_cache = false);
		void _CollectWaypointsFromLineSegment(glm::vec2 line_begin, glm::vec2 line_end, bool use_cache = false);
		void _CalculateTangents();

		glm::vec2 _CircleToPointTangent(glm::vec2 circle_center, float circle_radius, glm::vec2 point, bool flip_side = false);

		float _seg_length;
		float _total_length;
	};
}