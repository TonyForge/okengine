#pragma once

#include "IGame.h"
#include "IObject.h"
#include "..\..\Behaviour.h"

namespace Kripta
{
	class Object : public Kripta::IObject
	{
	public:
		virtual void PickUpObject(Kripta::Object* obj);

		int level = 0;
		int hp;

		int grid_x;
		int grid_y;

		int action_grid_x;
		int action_grid_y;
		Kripta::ObjActionID action_id = Kripta::ObjActionID::Idle;

		Kripta::ObjectID id;
		Kripta::LinWay movement_path;

		virtual void PostUpdate(float dt);
		void SetLevel(int level);
		void Place(int grid_x, int grid_y);
		void Kick(int attack_level);
		void DrawHealthbar();
	private:
	protected:
	};

	class TurnController : public ok::Behaviour
	{
	public:
		TurnController();
		~TurnController();
		void Update(float dt);
		static bool turn_in_progress;
		static int turn_members_decision_made;
		static int turn_members_ready;
		static int turn_members_total;
		static int turn_members_died;

		float mv_x, mv_y;
		bool turn_decision_made = false;
	};
}