#include "Collision.h"

Zoner::Collision::Point::Point() : xyz(0.f,0.f,0.f), hit(false)
{
}

Zoner::Collision::Point::Point(glm::vec3 _xyz, bool _hit) : xyz(_xyz), hit(_hit)
{
}
