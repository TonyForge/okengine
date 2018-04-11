#include "ShipCommands.h"

ok::Pool<Zoner::Cmd_Ship_WaitArrival> Zoner::Cmd_Ship_WaitArrival::pool;
ok::Pool<Zoner::Cmd_Ship_Relocate> Zoner::Cmd_Ship_Relocate::pool;

void Zoner::Cmd_Ship_MoveTo::OnEnterList()
{
	owner->BeginTransform(ok::TransformSpace::WorldSpace);
	glm::vec3 this_position = owner->GetPosition();
	owner->EndTransform(false);

	//calculate path to space_xy
	owner->trajectory.BeginWay(this_position, owner->GetRight(), 10.0f);

	//avoid negative objects here...

	owner->trajectory.AdvanceWay(destination);
	owner->trajectory.EndWay();

	owner->trajectory_progress = 0.f;

	if (owner->trajectory.Length() == 0.f)
	{
		destination_reached = true;
	}
	else
	{
		destination_reached = false;
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
	if (destination_reached == false)
	{
		owner->trajectory_progress += (owner->engine_speed * hours_passed) / owner->trajectory.Length();
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
		owner->EndTransform(true);

		owner->LookAt(trajectory_waypoint.tangent, glm::vec3(0.f, 0.f, 1.f), ok::LookAtAxis::Right, ok::LookAtAxis::Forward);

		if (owner->trajectory_progress >= 1.f)
		{
			destination_reached = true;
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
