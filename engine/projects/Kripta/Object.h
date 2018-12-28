#pragma once

#include "IGame.h"
#include "IObject.h"
#include "..\..\Behaviour.h"

namespace Kripta
{
	class Object : public Kripta::IObject
	{
	public:
		int level;
		int hp;

		int grid_x;
		int grid_y;

		int action_grid_x;
		int action_grid_y;
		Kripta::ObjActionID action_id = Kripta::ObjActionID::Idle;

		Kripta::ObjectID id;
	private:
	protected:
	};

	class TurnController : public ok::Behaviour
	{
	public:
		void Update(float dt);
		void Turn();
		static bool turn_in_progress;
		static int turn_members_ready;
		static int turn_members_total;
	};
}