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
		bool Execute(float dt);

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

		void PassTime(float hours_passed);
		void ApplyPassedTime();
		void OnNewDay();

		Zoner::IShip* owner;
		glm::vec2 destination;
	};

	class Cmd_Ship_Relocate : public Zoner::ICommand
	{
	public:
		void ReturnToPool();
		bool Execute(float dt);

		Zoner::ISpace* destination;
		glm::vec2 destination_position;
		Zoner::IShip* owner;

		static Zoner::Cmd_Ship_Relocate* New();
		std::shared_ptr<ok::PoolContainer<Zoner::Cmd_Ship_Relocate>> container;
		static ok::Pool<Zoner::Cmd_Ship_Relocate> pool;
	};

	class Cmd_Ship_Pursuit : public Zoner::ICommand
	{
	public:
		Zoner::IShip* target;
	};
}