#pragma once

namespace Zoner
{
	namespace Collision
	{
		class Point
		{
		public:
			Point();
			Point(glm::vec3 _xyz, bool _hit);
			glm::vec3 xyz;
			bool hit;
		};
	}
}