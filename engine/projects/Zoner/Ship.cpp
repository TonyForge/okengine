#include "Ship.h"


/*void Zoner::Ship::PassTime(float hours_passed)
{
	cmd_parallel.PassTime(hours_passed);
}*/

/*void Zoner::Ship::ApplyPassedTime()
{
	cmd_parallel.ApplyPassedTime();
	*/
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
//}

/*void Zoner::Ship::OnNewDay()
{
	cmd_parallel.OnNewDay();

	if (isNPC)
	{
		NPC_MakeDecisions();
	}
}*/

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

	if (isNPC)
	{
		//do nothing
	}
	else
	{
		Zoner::IGame::o().ChangeCurrentLocation(location);
		location->camera.SetPosition(glm::vec3(relocationDestinationPosition, location->camera.GetPosition().z));
	}

	if (location != nullptr && location->isCurrent)
	{
		cmd_sequence.restriction_a = true;
		cmd_parallel.restriction_a = true;

		cmd_sequence.restriction_b = true;
		cmd_parallel.restriction_b = true;
	}
	else
	{
		cmd_sequence.restriction_a = false;
		cmd_parallel.restriction_a = false;

		cmd_sequence.restriction_b = false;
		cmd_parallel.restriction_b = false;
	}

	if (isNPC)
	{
		//do nothing
	}
	else
	{
		Zoner::IGame::o().State(Zoner::GameStates::PauseRequest, true);
	}
}

Zoner::ISpace *& Zoner::Ship::Location()
{
	return location;
}

ok::graphics::LineBatch* lb = nullptr;

void Zoner::Ship::Update(float dt)
{
	if (this_blueprint) this_blueprint->enabled = false;

	switch (this_type)
	{
		case Zoner::ShipType::ST_Spacecraft :
		{
			BeginTransform(ok::TransformSpace::WorldSpace);
			isInViewport = ok::graphics::Camera::GetCurrent()->IsInViewport(GetPosition(), this_blueprint->GetBounderMaxRadiusInWorldSpace());
			EndTransform(false);

			this_blueprint->BeginTransform();
			this_blueprint->SetRotation(this_blueprint->GetRotation() + glm::vec3(15.f * dt, 0.f, 0.f));
			this_blueprint->EndTransform(false);

			if (isInViewport == true)
			{
				this_blueprint->enabled = true;

				if (IsUpdateChildrensTransformNeeded())
					UpdateChildrensTransform();
				else
					this_blueprint->UpdateChildrensTransform();
			}
		}
		break;
	}

	if (isInViewport)
	{
		cmd_sequence.restriction_b = true;
		cmd_parallel.restriction_b = true;
	}
	else
	{
		cmd_sequence.restriction_b = false;
		cmd_parallel.restriction_b = false;
	}

	ok::GameObject::Update(dt);

	if (isNPC == false)
	{
		if (lb == nullptr)
		{
			lb = new ok::graphics::LineBatch();
			lb->SetBrushColor(ok::Color::Green);
			lb->SetBrushThickness(2.f);
			lb->SetBrushSoftness(0.0001f);
		}

		if (trajectory.Length() > 0)
		{
			lb->BatchBegin();
			int steps = 100;
			lb->MoveTo(trajectory.Pick(0.f).position);
			for (int i = 0; i <= steps; i++)
			{
				lb->LineTo(trajectory.Pick(static_cast<float>(i) / static_cast<float>(steps)).position);
			}
			lb->BatchEnd();
		}
	}
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

			cmd_sequence.Clear(Zoner::CommandExecutionStrategy::daily_240);

			_cmd_ship_moveto.owner = this;
			_cmd_ship_moveto.destination = space_xy;
			_cmd_ship_moveto.exact_arrival = false;
			cmd_parallel.Replace(&_cmd_ship_moveto, Zoner::Cmd_Groups::Movement, Zoner::CommandExecutionStrategy::daily_240);
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
					_cmd_ship_moveto.exact_arrival = true;
					cmd_parallel.Replace(&_cmd_ship_moveto, Zoner::Cmd_Groups::Movement, Zoner::CommandExecutionStrategy::daily_240);

					Zoner::Cmd_Ship_WaitArrival* cmd_arrival = Zoner::Cmd_Ship_WaitArrival::New();
					cmd_arrival->owner = this;
					cmd_arrival->destination = space_xy;

					Zoner::Cmd_Ship_Relocate* cmd_relocate = Zoner::Cmd_Ship_Relocate::New();
					cmd_relocate->owner = this;
					cmd_relocate->destination = static_cast<Zoner::ISpace*>(picked_object->GetPtr(Zoner::Requests::JumpHole_Destination));

					picked_object->BeginTransform(ok::TransformSpace::WorldSpace);
					cmd_relocate->destination_position = *static_cast<glm::vec2*>(picked_object->GetPtr(Zoner::Requests::JumpHole_DestinationPosition)) + (space_xy - glm::vec2(picked_object->GetPosition()));
					picked_object->EndTransform(false);

					cmd_sequence.Clear(Zoner::CommandExecutionStrategy::daily_4);
					cmd_sequence.Push(cmd_arrival, -1, Zoner::CommandExecutionStrategy::daily_4);
					cmd_sequence.Push(cmd_relocate, -1, Zoner::CommandExecutionStrategy::daily_4);
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
	}
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

void Zoner::Ship::SaveTo(tinyxml2::XMLDocument & doc, tinyxml2::XMLElement & element)
{
	element.SetAttribute("type", "spacecraft");
	element.SetAttribute("id", _gameengine_id.toAnsiString().c_str());
	element.SetAttribute("name", name.toAnsiString().c_str());
	element.SetAttribute("blueprint", this_blueprint->name.toAnsiString().c_str());
	element.SetAttribute("isNPC", isNPC);

	auto pos = doc.NewElement("position");
	auto rot = doc.NewElement("rotation");
	auto loc = doc.NewElement("location");

	BeginTransform();
	pos->SetAttribute("x", GetPosition().x);
	pos->SetAttribute("y", GetPosition().y);
	pos->SetAttribute("z", GetPosition().z);

	rot->SetAttribute("x", GetRotation().x);
	rot->SetAttribute("y", GetRotation().y);
	rot->SetAttribute("z", GetRotation().z);
	EndTransform(false);

	loc->SetAttribute("space_id", location->_gameengine_id.toAnsiString().c_str());

	element.InsertEndChild(pos);
	element.InsertEndChild(rot);
	element.InsertEndChild(loc);
	
	auto _uid = doc.NewElement("uid");
	element.InsertEndChild(_uid);
	save_uid_to_xml(*_uid);
}

void Zoner::Ship::LoadFrom(tinyxml2::XMLDocument & doc, tinyxml2::XMLElement & element)
{
	this_type = Zoner::ShipType::ST_Spacecraft;
	Rename(element.Attribute("name"));
	isNPC = element.BoolAttribute("isNPC");

	_gameengine_id = element.Attribute("id");

	this_blueprint = static_cast<Zoner::ShipBlueprint*>(Zoner::IGame::o().GetShipBlueprints()[element.Attribute("blueprint")]->Duplicate());
	AddChild(this_blueprint);

	load_uid_from_xml(*element.FirstChildElement("uid"));


	//------------------------------------------

	if (_gameengine_id == "Sozdatel_Mirov")
	{
		//item injection here
		//this_item = 
	}

	//------------------------------------------
}
