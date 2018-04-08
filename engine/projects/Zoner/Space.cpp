#include "Space.h"

void Zoner::Space::PassTime(float hours_passed)
{
	local_hour += hours_passed;

	for (auto& visitor : visitors)
	{
		visitor->PassTime(hours_passed);
	}
}

void Zoner::Space::PassToTime(float daytime_hour)
{
	float hours_passed = daytime_hour - local_hour;
	PassTime(hours_passed);
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
	local_hour = 0;

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

	RelocateVisitors();

	for (auto& jump_hole : jump_holes)
	{
		jump_hole->Update(dt);
	}

	ok::graphics::LayeredRenderer::instance().Flush();

	ok::graphics::Camera::PopCamera();
}

void Zoner::Space::VisitorIn(Zoner::IShip * visitor)
{
	visitors.push_back(visitor);

	//visitor->Location() in this line keeps space ptr of visitors previous location
	//and the it changes to this

	visitor->Location() = this;
	visitor->RelocationComplete();
}

void Zoner::Space::VisitorOut(Zoner::IShip * visitor, Zoner::ISpace* destination)
{
	visitors_out.push_back(std::make_pair(visitor, destination));
}

void Zoner::Space::RelocateVisitors()
{
	for (auto& visitor : visitors_out)
	{
		visitors.erase(std::remove(visitors.begin(), visitors.end(), visitor.first), visitors.end());
		visitor.second->VisitorIn(visitor.first);
	}

	visitors_out.clear();
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

	for (auto&& jump_hole : jump_holes)
	{
		Zoner::Collision::Point pick = jump_hole->Pick(glm::vec3(space_xy, 0.f));

		if (pick.hit)
		{
			_who_is_there_container.push_back(jump_hole);
		}
	}

	return _who_is_there_container;
}

std::vector<Zoner::IShip*>& Zoner::Space::Visitors()
{
	return visitors;
}
