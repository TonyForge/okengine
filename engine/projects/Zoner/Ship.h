#pragma once

#include "IGame.h"
#include "IShip.h"
#include "ISpace.h"
#include "..\..\Input.h"
#include "ShipCommands.h"

namespace Zoner
{

	class Ship : public Zoner::IShip
	{
	public:
		void Relocate(Zoner::ISpace* to);
		void RelocationComplete();
		Zoner::ISpace*& Location();
		Zoner::ISpace* location = nullptr;

		void Update(float dt);

		void ClickOnceAt(glm::vec2 space_xy, bool ignore_objects = false);

		void Player_UpdateDecisions(float dt);
		void NPC_MakeDecisions();

		virtual void* GetPtr(int id);

		virtual void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element);
		virtual void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element);

	protected:
	private:

		//parallel commands
		Zoner::Cmd_Ship_MoveTo _cmd_ship_moveto;
	};
}