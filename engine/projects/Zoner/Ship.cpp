#include "Ship.h"


void Zoner::Ship::PassTime(float hours_passed)
{
	cmd_parallel.PassTime(hours_passed);
}

void Zoner::Ship::ApplyPassedTime()
{
	cmd_parallel.ApplyPassedTime();
	
	/*if (isNPC)
	if (location != nullptr)
	{
		auto local_visitors = location->Visitors();

		for (auto&& visitor : local_visitors)
		{
			if (visitor->cmp_uid(*this))
			{
				//do nothing
			}
			else
			{
				if (visitor->trajectory.Length() > 0.f)
				{
					glm::vec3 pos = visitor->GetPosition() + visitor->GetRight() * visitor->engine_speed * 8.f;
					if (glm::length(pos - GetPosition()) > 10.f)
					{
						ClickOnceAt(visitor->GetPosition() + visitor->GetRight() * visitor->engine_speed * 8.f, true);
						trajectory_progress = 0.f;
					}
					
				}
				
			}
		}
	}*/
}

void Zoner::Ship::OnNewDay()
{
	cmd_parallel.OnNewDay();

	if (isNPC)
	{
		NPC_MakeDecisions();
	}
}

void Zoner::Ship::Relocate(Zoner::ISpace * to)
{
	if (relocationInProgress) return;

	if (location != nullptr)
	{
		location->VisitorOut(this, to);
		relocationInProgress = true;
	}
	else
		to->VisitorIn(this);
}

void Zoner::Ship::RelocationComplete()
{
	BeginTransform(ok::TransformSpace::WorldSpace);
	SetPosition(glm::vec3(relocationDestinationPosition, 0.f));
	EndTransform(true);

	relocationInProgress = false;
}

Zoner::ISpace *& Zoner::Ship::Location()
{
	return location;
}

void Zoner::Ship::Update(float dt)
{
	cmd_sequence.PopExecute(dt);

	switch (this_type)
	{
		case Zoner::ShipType::ST_Spacecraft :
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

			cmd_sequence.Clear();

			_cmd_ship_moveto.owner = this;
			_cmd_ship_moveto.destination = space_xy;
			cmd_parallel.Replace(&_cmd_ship_moveto, Zoner::Cmd_Groups::Movement);
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

			//do something with picked_object
			if (picked_object != nullptr)
			{
				if (picked_object->this_type == Zoner::ShipType::ST_Jumphole)
				{
					_cmd_ship_moveto.owner = this;
					_cmd_ship_moveto.destination = space_xy;
					cmd_parallel.Replace(&_cmd_ship_moveto, Zoner::Cmd_Groups::Movement);

					Zoner::Cmd_Ship_WaitArrival* cmd_arrival = Zoner::Cmd_Ship_WaitArrival::New();
					cmd_arrival->owner = this;
					cmd_arrival->destination = space_xy;

					Zoner::Cmd_Ship_Relocate* cmd_relocate = Zoner::Cmd_Ship_Relocate::New();
					cmd_relocate->owner = this;
					cmd_relocate->destination = static_cast<Zoner::ISpace*>(picked_object->GetPtr(Zoner::Requests::JumpHole_Destination));

					picked_object->BeginTransform(ok::TransformSpace::WorldSpace);
					cmd_relocate->destination_position = *static_cast<glm::vec2*>(picked_object->GetPtr(Zoner::Requests::JumpHole_DestinationPosition)) + (space_xy - glm::vec2(picked_object->GetPosition()));
					picked_object->EndTransform(false);

					cmd_sequence.Clear();
					cmd_sequence.Push(cmd_arrival, -1);
					cmd_sequence.Push(cmd_relocate, -1);
				}
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
		//ClickOnceAt(click_world_position);
	}

	/*if (ok::Input::o().KeyPressed(ok::KKey::J))
	{
		Relocate(_spaces[_game_file_element_iterator->FirstChildElement("location")->Attribute("space_id")]);
	}*/
}

void Zoner::Ship::NPC_MakeDecisions()
{
	/*if (location != nullptr)
	{
		auto local_visitors = location->Visitors();

		for (auto&& visitor : local_visitors)
		{
			if (visitor->cmp_uid(*this))
			{
				//do nothing
			}
			else
			{
				ClickOnceAt(visitor->GetPosition(), true);
			}
		}
	}*/
}

void * Zoner::Ship::GetPtr(int id)
{
	return nullptr;
}
