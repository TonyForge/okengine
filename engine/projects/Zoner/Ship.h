#pragma once

#include "IGame.h"
#include "IShip.h"
#include "ISpace.h"
#include "..\..\Input.h"

namespace Zoner
{
	class Ship : public Zoner::IShip
	{
	public:
		void PassTime(float hours_passed);
		void ApplyPassedTime();
		void OnNewDay();

		void Relocate(Zoner::ISpace* to);
		Zoner::ISpace*& Location();
		Zoner::ISpace* location = nullptr;

		void Update(float dt);

		void ClickOnceAt(glm::vec2 space_xy, bool ignore_objects = false);

		void Player_UpdateDecisions(float dt);
	protected:
	private:
	};
}