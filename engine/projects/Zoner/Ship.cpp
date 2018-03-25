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
		Zoner::SmoothPathWaypoint trajectory_waypoint = trajectory.GetWaypoint(trajectory_progress);

		BeginTransform();
		SetPosition(trajectory_waypoint.position);
		EndTransform(true);

		LookAt(trajectory_waypoint.tangent_out, glm::vec3(0.f, 0.f, 1.f), ok::LookAtAxis::Right, ok::LookAtAxis::Forward);
	}
}

void Zoner::Ship::OnNewDay()
{
	if (trajectory.Length() > 0.f)
	{
		if (trajectory_progress < 1.f)
		{
			ClickOnceAt(trajectory.GetWaypoint(1.f).position, true);
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

#include "..\..\LineBatch.h"
ok::graphics::LineBatch* line_batch_debug = nullptr;

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

	if (line_batch_debug == nullptr)
	{
		line_batch_debug = new ok::graphics::LineBatch();
	}

	if (trajectory.Length() > 0.f)
	Zoner::SmoothPath::DrawDebug(*line_batch_debug, trajectory);
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
			trajectory.BeginWaypointsCollection();

			trajectory.CollectWaypoint(this_position); //1
			
			//...
				glm::vec3 to_target_distance = glm::vec3(space_xy, this_position.z) - this_position;
				float to_target_distance_length = glm::length(to_target_distance);
				glm::vec3 to_target_vector = glm::normalize(to_target_distance);
				glm::vec3 direction_vector = GetRight();

				float oriented_angle = glm::degrees(glm::orientedAngle(glm::vec2(direction_vector), glm::vec2(to_target_vector)));

				glm::vec3 shift1 = glm::rotate(direction_vector, glm::radians(glm::sign(oriented_angle) * 45.f), glm::vec3(0.f, 0.f, 1.f));
				glm::vec3 shift2 = -glm::rotate(direction_vector, glm::radians(glm::sign(-oriented_angle) * 45.f), glm::vec3(0.f, 0.f, 1.f));
				glm::vec3 shift3 = glm::rotate(direction_vector, glm::radians(glm::sign(oriented_angle) * 90.f), glm::vec3(0.f, 0.f, 1.f));
				
				//glm::vec3 shift2 = glm::rotate(direction_vector, glm::radians(oriented_angle * 0.3f), glm::vec3(0.f, 0.f, 1.f));
				//glm::vec3 shift3 = glm::rotate(direction_vector, glm::radians(oriented_angle * 0.6f), glm::vec3(0.f, 0.f, 1.f));
				//glm::vec3 shift4 = glm::rotate(direction_vector, glm::radians(oriented_angle * 0.8f), glm::vec3(0.f, 0.f, 1.f));

			float angular_push = 50.f;

			if (glm::abs(oriented_angle) >= 90.f)
			{
				if (to_target_distance_length < angular_push*2.f)
				{
					trajectory.CollectWaypoint(this_position + direction_vector*angular_push);
					trajectory.CollectWaypoint(this_position + shift1*angular_push*1.5f);

					if (glm::abs(oriented_angle) >= 135.f || to_target_distance_length < angular_push)
						trajectory.CollectWaypoint(this_position + shift3*angular_push);
				}
				else
				{
					if (glm::abs(oriented_angle) >= 135.f)
					{
						trajectory.CollectWaypoint(this_position + shift1*angular_push);
						trajectory.CollectWaypoint(this_position + (shift1 + shift2)*angular_push);
					}
					else
					{
						trajectory.CollectWaypoint(this_position + shift1*angular_push);
					}

					trajectory.CollectWaypoint(this_position + (to_target_distance + (shift1 + shift2)*angular_push)*0.5f + shift3*angular_push*0.2f);
				}
			}
			else
			{

			}
			

			/*if (to_target_distance_length > 100.f)
			trajectory.CollectWaypoint(this_position + to_target_distance*0.5f + shift3*50.f);
			else
			trajectory.CollectWaypoint(this_position + shift2 * 75.f);*/

			/*if (glm::abs(oriented_angle) > 80.f && glm::abs(oriented_angle) < 100.f)
			{
				trajectory.CollectWaypoint(this_position + to_target_distance*0.5f + direction_vector*glm::clamp(to_target_distance_length*0.5f, 0.f, 50.f));
			}
			else
			{
				if (glm::abs(oriented_angle) > 90.f)
					trajectory.CollectWaypoint(this_position + shift1*50.f);

				if (glm::abs(oriented_angle) > 120.f)
					trajectory.CollectWaypoint(this_position + shift1*50.f + shift2 * 50.f);

				if (glm::abs(oriented_angle) > 90.f)
					trajectory.CollectWaypoint(this_position + to_target_distance*0.5f + shift3*50.f);
				else
					trajectory.CollectWaypoint(this_position + direction_vector*glm::clamp(to_target_distance_length*0.5f, 0.f, 100.f));
			}*/

			//if (to_target_distance_length > 100.0f)
		//	{
				
			//}
			//trajectory.CollectWaypoint(this_position + direction_vector);
			//trajectory.CollectWaypoint(this_position + direction_vector*50.0f);
			//trajectory.CollectWaypoint(this_position + shift1*50.f);  //2
			//trajectory.CollectWaypoint(this_position + shift2*50.f);  //3
		//	trajectory.CollectWaypoint(this_position + shift3*50.f);  //4
			//trajectory.CollectWaypoint(this_position + shift4*50.f);  //4
			//trajectory.CollectWaypoint(this_position + (glm::vec3(space_xy, this_position.z) - this_position)*0.5f);
			//...
			
			trajectory.CollectWaypoint(glm::vec3(space_xy, 0.f));  //5

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
