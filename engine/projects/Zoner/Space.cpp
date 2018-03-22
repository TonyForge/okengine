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
