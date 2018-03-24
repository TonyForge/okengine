#include "Space.h"

void Zoner::Space::PassTime(float hours_passed)
{
	for (auto& visitor : visitors)
	{
		visitor->PassTime(hours_passed);
	}
}

void Zoner::Space::ApplyPassedTime()
{
	for (auto& visitor : visitors)
	{
		visitor->ApplyPassedTime();
	}
}

void Zoner::Space::OnNewDay()
{
	for (auto& visitor : visitors)
	{
		visitor->OnNewDay();
	}
}

void Zoner::Space::Update(float dt)
{
	ok::GameObject::Update(dt);

	ok::graphics::Camera::PushCamera(&camera);

	for (auto& visitor : visitors)
	{
		visitor->Update(dt);
	}

	ok::graphics::LayeredRenderer::instance().Flush();

	ok::graphics::Camera::PopCamera();
}

void Zoner::Space::VisitorIn(Zoner::IShip * visitor)
{
	visitors.push_back(visitor);
	visitor->Location() = this;
}

void Zoner::Space::VisitorOut(Zoner::IShip * visitor)
{
	visitors.erase(std::remove(visitors.begin(), visitors.end(), visitor), visitors.end());
	visitor->Location() = nullptr;
}

std::vector<Zoner::IShip*>& Zoner::Space::WhoIsThere(glm::vec2 space_xy)
{
	_who_is_there_container.clear();

	for (auto&& visitor : visitors)
	{
		Zoner::Collision::Point pick = visitor->Pick(glm::vec3(space_xy, 0.f));

		if (pick.hit)
		{
			_who_is_there_container.push_back(visitor);
		}
	}

	return _who_is_there_container;
}
