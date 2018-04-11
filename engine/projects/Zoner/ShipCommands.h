#pragma once

#include "Commands.h"
#include "UID.h"
#include "IShip.h"
#include "ISpace.h"
#include "..\..\Pool.h"

namespace Zoner
{
	namespace Cmd_Groups
	{
		const int Movement = 1000;
	}
	
	class Cmd_Ship_WaitArrival : public Zoner::ICommand
	{
	public:
		void ReturnToPool();
		bool Execute();

		glm::vec2 destination;
		Zoner::IShip* owner;

		static Zoner::Cmd_Ship_WaitArrival* New();
		std::shared_ptr<ok::PoolContainer<Zoner::Cmd_Ship_WaitArrival>> container;
		static ok::Pool<Zoner::Cmd_Ship_WaitArrival> pool;
	};

	class Cmd_Ship_MoveTo : public Zoner::ICommand
	{
	public:
		void OnEnterList();
		bool Execute();
		void OnExitList();

		void PassTime(float hours_passed);
		void ApplyPassedTime();

		Zoner::IShip* owner;
		glm::vec2 destination;
		bool destination_reached;
	};

	class Cmd_Ship_Relocate : public Zoner::ICommand
	{
	public:
		void ReturnToPool();
		bool Execute();

		Zoner::ISpace* destination;
		glm::vec2 destination_position;
		Zoner::IShip* owner;

		static Zoner::Cmd_Ship_Relocate* New();
		std::shared_ptr<ok::PoolContainer<Zoner::Cmd_Ship_Relocate>> container;
		static ok::Pool<Zoner::Cmd_Ship_Relocate> pool;
	};

	//mode A - maksimalnoe sbligenie
	//mode B - dergatsa na rasstoyanii vistrela iz vseh orudiy
	//mode C - avtomaticheskaya bitva
	class Cmd_Ship_Pursuit : public Zoner::ICommand
	{
	public:
		Zoner::IShip* owner;
		Zoner::IShip* target;

		bool close_up_mode;
		bool keep_attack_distance_mode;
		bool automatic_battle_mode;
	};
}