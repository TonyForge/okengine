#pragma once

#include "..\..\GameObject.h"

namespace Kripta
{
	enum class ObjectID
	{
		Unknown,
		Hero,
		Goblin,
		Tomb,
		GoldPile,
		HealthPotion,
		GoldenGuard,
		Stair
	};

	enum class ObjActionID
	{
		Wait,
		Idle,
		Act,
		Move,
		Attack,
		FalseMove,
		Pickup,
		ToStair,
		ToDoor,
		MoveAndPickup
	};

	class IObject : public ok::GameObject
	{
	public:
		virtual void PostUpdate(float dt) = 0;
		virtual ~IObject();
		virtual void SetLevel(int level) = 0;
		virtual void Place(int grid_x, int grid_y) = 0;
		virtual void Kick(int attack_level) = 0;
		bool dead = false;
	private:
	protected:
	};
}