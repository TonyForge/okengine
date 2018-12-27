#pragma once

#include "IGame.h"
#include "IObject.h"
#include "..\..\Behaviour.h"

namespace Kripta
{
	class TurnController : public ok::Behaviour
	{
	public:
		void Update(float dt);
		bool turn_in_progress = false;
		static int turn_members;
	};

	class Object : public Kripta::IObject
	{
	public:
		int level;
		int hp;

		int grid_x;
		int grid_y;

		Kripta::ObjectID id;
	private:
	protected:
	};
}