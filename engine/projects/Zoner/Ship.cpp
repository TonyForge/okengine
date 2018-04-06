#include "Ship.h"


void Zoner::Ship::PassTime(float hours_passed)
{
	if (trajectory.Length() > 0.f)
	{
		trajectory_progress += (engine_speed * hours_passed) / trajectory.Length();
		if (trajectory_progress > 1.f) trajectory_progress = 1.f;
	}
}

void Zoner::Ship::ApplyPassedTime()
{
	if (trajectory.Length() > 0.f)
	{
		//Move in space
		Zoner::SmoothPathWaypoint trajectory_waypoint = trajectory.Pick(trajectory_progress);

		BeginTransform();
		SetPosition(trajectory_waypoint.position);
		EndTransform(true);

		LookAt(trajectory_waypoint.tangent, glm::vec3(0.f, 0.f, 1.f), ok::LookAtAxis::Right, ok::LookAtAxis::Forward);
	}
}

void Zoner::Ship::OnNewDay()
{
	if (trajectory.Length() > 0.f)
	{
		if (trajectory_progress < 1.f)
		{
			ClickOnceAt(trajectory.Pick(1.f).position, true);
		}
		else
		{
			trajectory.Clear();
		}

		trajectory_progress = 0.f;
	}
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
			
			this_blueprint->BeginTransform();
			this_blueprint->SetRotation(this_blueprint->GetRotation() + glm::vec3(15.f * dt, 0.f, 0.f));
			this_blueprint->EndTransform(true);
		}
		break;
	}

	ok::GameObject::Update(dt);
}

void Zoner::Ship::ClickOnceAt(glm::vec2 space_xy, bool ignore_objects)
{
	if (location != nullptr)
	{
		//todo: avoid WhoIsThere call if ignore_objects == true
		std::vector<Zoner::IShip*>& picked_objects = location->WhoIsThere(space_xy);

		if (picked_objects.size() == 0 || ignore_objects == true)
		{
			//click at empty space
			picked_object = nullptr;
			picked_object_picks_counter = 0;

			BeginTransform(ok::TransformSpace::WorldSpace);
				glm::vec3 this_position = GetPosition();
			EndTransform(false);

			//calculate path to space_xy
			trajectory.BeginWay(this_position, GetRight(), 10.0f);
			trajectory.AdvanceWay(space_xy);
			trajectory.EndWay();
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
