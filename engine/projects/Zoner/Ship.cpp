#include "Ship.h"


void Zoner::Ship::PassTime(float hours_passed)
{
}

void Zoner::Ship::ApplyPassedTime()
{
}

void Zoner::Ship::Relocate(Zoner::ISpace * to)
{
	if (location != nullptr)
	{
		location->VisitorOut(this);
	}

	to->VisitorIn(this);
}

Zoner::ISpace *& Zoner::Ship::Location()
{
	return location;
}

void Zoner::Ship::Update(float dt)
{
	switch (this_type)
	{
		case Zoner::ShipType::Spacecraft :
		{
			BeginTransform();
			SetRotation(GetRotation() + glm::vec3(15.f * dt, 0.f, 0.f));
			EndTransform(true);
		}
		break;
	}

	ok::GameObject::Update(dt);
}

void Zoner::Ship::ClickOnceAt(glm::vec2 space_xy)
{
	if (location != nullptr)
	{

	}
}
