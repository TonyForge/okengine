#include "Object.h"

bool Kripta::TurnController::turn_in_progress = false;
int Kripta::TurnController::turn_members_ready = 0;
int Kripta::TurnController::turn_members_total = 0;

void Kripta::TurnController::Update(float dt)
{
	Kripta::Object& obj = *gameObject().GetComponent<Kripta::Object>();
	
	if (turn_in_progress)
	{
		if (obj.action_id == Kripta::ObjActionID::Wait)
		{
			//do nothing
		}
		else
		{
			if (obj.action_id == Kripta::ObjActionID::Idle)
			{
				turn_members_ready++;
				obj.action_id = Kripta::ObjActionID::Wait;
			}

			if (obj.action_id == Kripta::ObjActionID::Act)
			{
				if (obj.action_grid_x)
			}
		}

		if (turn_members_ready == turn_members_total) turn_in_progress = false;
	}
}

void Kripta::TurnController::Turn()
{
	turn_members_ready = 0;
	turn_in_progress = true;
}
