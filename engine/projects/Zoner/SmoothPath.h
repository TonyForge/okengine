#pragma once

#include "..\..\Spline.h"
#include "..\..\LineBatch.h"

namespace Zoner
{

	class SmoothPathObstacle //obsolete
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

		void BeginWay(glm::vec2 begin_position, glm::vec2 begin_direction, float seg_length, float section_length = 0.f, float section_discard_limit_length = 0.f);
		void AdvanceWay(glm::vec2 next_position);
		void EndWay();
		

		float Length();
		void Clear();

		//void BuildFromEllipse(glm::vec2 center, glm::vec2 axis_x, glm::vec2 axis_y);
		
	private:
		struct _Tangent2D
		{
			glm::vec2 begin;
			glm::vec2 end;
		};

		std::vector<Zoner::SmoothPathWaypoint> _waypoints;
		static std::vector<Zoner::SmoothPathWaypoint> _waypoints_cache;

		//circle_center is in world space
		bool _CircleToLineIntesectionCheck(glm::vec2 circle_center, float circle_radius, glm::vec2 line_begin, glm::vec2 line_end);
		void _CollectWaypointsFromArc(glm::vec2 circle_center, float circle_radius, glm::vec2 arc_begin, glm::vec2 arc_end, bool use_cache = false, bool force_ccw = false, bool force_cw = false);
		void _CollectWaypointsFromLineSegment(glm::vec2 line_begin, glm::vec2 line_end, bool use_cache = false);
		void _CalculateTangents();
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
		float _section_length;
		float _section_discard_limit_length;
		float _total_length;

		static bool _pfs_confluent_arc;
		static bool _pfs_mirrored;
		static glm::vec2 _pfs_rotation_center;
		static float _pfs_rotation_radius;
	};
}