#include "Ship.h"


void Zoner::Ship::PassTime(float hours_passed)
{
	if (trajectory.Length() != 0.f)
	{
		trajectory_progress += (trajectory.Length() / engine_speed) * hours_passed;
		if (trajectory_progress > 1.f) trajectory_progress = 1.f;
	}
}

void Zoner::Ship::ApplyPassedTime()
{
	//Move in space
	Zoner::SmoothPathWaypoint trajectory_waypoint = trajectory.GetWaypoint(trajectory_progress);

	BeginTransform();
	SetPosition(trajectory_waypoint.position);
	EndTransform(true);

	SetRight(trajectory_waypoint.tangent_out);
}

void Zoner::Ship::OnNewDay()
{
	if (trajectory_progress < 1.f)
	{
		ClickOnceAt(trajectory.GetWaypoint(trajectory_progress).position);
	}
	else
	{
		trajectory.Clear();
	}

	trajectory_progress = 0.f;
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
		std::vector<Zoner::IShip*>& picked_objects = location->WhoIsThere(space_xy);

		if (picked_objects.size() == 0)
		{
			//click at empty space
			picked_object = nullptr;
			picked_object_picks_counter = 0;

			//calculate path to space_xy
			trajectory.BeginWaypointsCollection();

			BeginTransform(ok::TransformSpace::WorldSpace);
				trajectory.CollectWaypoint(GetPosition());
			EndTransform(false);

			//...

			trajectory.CollectWaypoint(glm::vec3(space_xy, 0.f));

			trajectory.EndWaypointsCollection(10.f);
		}
		else
		{
			//to do: show selector UI with list of picked objects names and pick on selected
			//for now: pick on first picked object

			if (picked_objects[0] != picked_object)
			{
				picked_object = picked_objects[0];
				picked_object_picks_counter = 1;
			}
			else
			{
				picked_object_picks_counter++;
				if (picked_object_picks_counter > 3)
					picked_object_picks_counter = 1;
			}
		}
	}
}

void Zoner::Ship::Player_UpdateDecisions(float dt)
{
	if (ok::Input::o().KeyPressed(ok::MKey::Left))
	{
		glm::vec3 click_world_position = location->camera.ScreenToWorldPosition(glm::vec3(ok::Input::o().MouseX(), ok::Input::o().MouseY(),0.f));
		ClickOnceAt(click_world_position);
	}
}
