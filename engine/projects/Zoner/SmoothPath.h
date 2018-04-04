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

		bool IsEqualTo(const Zoner::SmoothPathWaypoint& waypoint);

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

		
	//private:
		struct _Tangent2D
		{
			glm::vec2 begin;
			glm::vec2 end;
		};

		std::vector<Zoner::SmoothPathWaypoint> _waypoints;
		static std::vector<Zoner::SmoothPathWaypoint> _waypoints_cache;
		static std::vector<Zoner::SmoothPathObstacle> _obstacles_cache;
		static std::vector<Zoner::SmoothPathObstacle> _obstacles_cache_post;
		static std::vector<Zoner::SmoothPathObstacle> _obstacles_cache_pre;
		static std::vector<bool> _obstacles_moved_to_cache;
		static std::vector<bool> _obstacles_forbidden_cache;
		static std::vector<glm::vec2> _pathfind_pivots_cache;
		static std::vector<glm::vec3> _obstacles_hotspot_cache;

		//circle_center is in world space
		bool _CircleToLineIntesectionCheck(glm::vec2 circle_center, float circle_radius, glm::vec2 line_begin, glm::vec2 line_end);
		void _CollectWaypointsFromArc(glm::vec2 circle_center, float circle_radius, glm::vec2 arc_begin, glm::vec2 arc_end, bool use_cache = false, bool force_ccw = false, bool force_cw = false);
		void _CollectWaypointsFromLineSegment(glm::vec2 line_begin, glm::vec2 line_end, bool use_cache = false);
		void _CalculateTangents();
		bool _RebuildPathfindPivotsCache(glm::vec2 from, glm::vec2 to, std::vector<Zoner::SmoothPathObstacle*>& obstacles);
		void _RescaleCircle(glm::vec2& circle_center, float& circle_radius, glm::vec2 scale_hotspot, float scale);

		//must be non parallel
		glm::vec2 _LineToLineIntersection(glm::vec2 line_a_start, glm::vec2 line_a_end, glm::vec2 line_b_start, glm::vec2 line_b_end);

		glm::vec2 _CircleToPointTangent(glm::vec2 circle_center, float circle_radius, glm::vec2 point, bool flip_side = false);
		Zoner::SmoothPath::_Tangent2D _CircleToCircleTangent(
			glm::vec2 from_circle_center, float from_circle_circle_radius,
			glm::vec2 to_circle_center, float to_circle_circle_radius,
			bool outside = false,
			bool flip_side = false);

		glm::vec2 _CalculatePassageFirstSection(const glm::vec2& from, const glm::vec2& from_direction, glm::vec2 to);

		float _seg_length;
		float _total_length;

		static bool _pfs_mirrored;
		static glm::vec2 _pfs_rotation_center;
		static float _pfs_rotation_radius;
	};
}