#pragma once

#include "..\..\Spline.h"
#include "..\..\LineBatch.h"

namespace Zoner
{
	class SmoothPathObstacle
	{
	public:
		glm::vec3 position;
		float radius;
	};

	class SmoothPathWaypoint
	{
	};

	class SmoothPath
	{
	public:
		void Build(glm::vec3 from, glm::vec3 from_direction, glm::vec3 to, std::vector<Zoner::SmoothPathObstacle*>& obstacles, ok::graphics::LineBatch& batch);
	};
}