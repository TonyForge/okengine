#include "ShipCommands.h"

ok::Pool<Zoner::Cmd_Ship_WaitArrival> Zoner::Cmd_Ship_WaitArrival::pool;
ok::Pool<Zoner::Cmd_Ship_Relocate> Zoner::Cmd_Ship_Relocate::pool;

void Zoner::Cmd_Ship_MoveTo::OnEnterList()
{
	owner->BeginTransform(ok::TransformSpace::WorldSpace);
	glm::vec3 this_position = owner->GetPosition();
	owner->EndTransform(false);

	//calculate path to space_xy
	if (exact_arrival)
		owner->trajectory.BeginWay(this_position, owner->GetRight(), 10.0f);
	else
		owner->trajectory.BeginWay(this_position, owner->GetRight(), 10.0f, owner->engine_speed*24.f, 70.f);

	//avoid negative objects here...

	owner->trajectory.AdvanceWay(destination);
	owner->trajectory.EndWay();

	destination = owner->trajectory.Pick(1.f).position;
	owner->trajectory_progress = 0.f;

	if (owner->trajectory.Length() == 0.f)
	{
		destination_reached = true;
	}
	else
	{
		destination_reached = false;
	}

	travel_time_passed = 0.f;
	travel_distance_left = owner->trajectory.Length();

	travel_speed = owner->engine_speed;
	if (owner->afterburner_enabled) travel_speed *= 2.f;

	float daily_travel_distance = travel_speed * 24.f;

	if (travel_distance_left < daily_travel_distance)
	{
		travel_speed *= travel_distance_left / daily_travel_distance;
	}
}

bool Zoner::Cmd_Ship_MoveTo::Execute()
{
	return destination_reached;
}

void Zoner::Cmd_Ship_MoveTo::OnExitList()
{
	owner->trajectory.Clear();
	owner->trajectory_progress = 0.f;
}

void Zoner::Cmd_Ship_MoveTo::PassTime(float hours_passed)
{
	travel_time_passed += hours_passed;

	if (destination_reached == false)
	{
		owner->trajectory_progress += (travel_speed * hours_passed) / owner->trajectory.Length();

		if (owner->trajectory_progress > 1.f) owner->trajectory_progress = 1.f;
	}
}

void Zoner::Cmd_Ship_MoveTo::ApplyPassedTime()
{
	if (destination_reached == false)
	{
		//Move in space
		Zoner::SmoothPathWaypoint trajectory_waypoint = owner->trajectory.Pick(owner->trajectory_progress);

		owner->BeginTransform();
		owner->SetPosition(trajectory_waypoint.position);
		owner->EndTransform(false); //childrens will be updated later when necessary

		owner->LookAt(trajectory_waypoint.tangent, glm::vec3(0.f, 0.f, 1.f), ok::LookAtAxis::Right, ok::LookAtAxis::Forward);

		if (owner->trajectory_progress >= 1.f)
		{
			destination_reached = true;
		}
	}
}

void Zoner::Cmd_Ship_MoveTo::OnNewDay()
{
	if (destination_reached)
	{
		//do nothing
	}
	else
	{
		travel_distance_left -= travel_speed * 24.f;

		travel_speed = owner->engine_speed;
		if (owner->afterburner_enabled) travel_speed *= 2.f;

		float daily_travel_distance = travel_speed * 24.f;

		if (travel_distance_left < daily_travel_distance)
		{
			travel_speed *= glm::abs(travel_distance_left) / daily_travel_distance;
		}

		if (owner->isNPC)
		{
			//do nothing
		}
		else
		{
			if (travel_distance_left > 70.f*2.f)
			{
				Zoner::IGame::o().State(Zoner::GameStates::CancelAutoPause, true);
			}
		}
		
	}
}

void Zoner::Cmd_Ship_WaitArrival::ReturnToPool()
{
	pool.Inject(container);
}

bool Zoner::Cmd_Ship_WaitArrival::Execute()
{
	owner->BeginTransform(ok::TransformSpace::WorldSpace);

	if (glm::length2(glm::vec2(owner->GetPosition()) - destination) < 2.f)
	{
		owner->EndTransform(false);
		owner->cmd_parallel.Remove(Zoner::Cmd_Groups::Movement, Zoner::CommandExecutionStrategy::daily_240);
		return true;
	}

	owner->EndTransform(false);

	return false;
}

Zoner::Cmd_Ship_WaitArrival * Zoner::Cmd_Ship_WaitArrival::New()
{
	std::shared_ptr<ok::PoolContainer<Zoner::Cmd_Ship_WaitArrival>> ptr = pool.Eject();
	Zoner::Cmd_Ship_WaitArrival& result = (**ptr);
	result.poolable = true;
	result.deletable = true;
	result.container = ptr;

	return &result;
}

void Zoner::Cmd_Ship_Relocate::ReturnToPool()
{
	pool.Inject(container);
}

bool Zoner::Cmd_Ship_Relocate::Execute()
{
	owner->relocationDestinationPosition = destination_position;
	owner->Relocate(destination);

	return true;
}

Zoner::Cmd_Ship_Relocate * Zoner::Cmd_Ship_Relocate::New()
{
	std::shared_ptr<ok::PoolContainer<Zoner::Cmd_Ship_Relocate>> ptr = pool.Eject();
	Zoner::Cmd_Ship_Relocate& result = (**ptr);
	result.poolable = true;
	result.deletable = true;
	result.container = ptr;

	return &result;
}