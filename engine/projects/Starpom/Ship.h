#pragma once

#include "..\..\GameObject.h"
#include "GameInfo.h"

namespace Starpom
{
	//SS - Star System - режим звездной системы (локации с планетами и звездами)

	/*
	SS_ShipAgent - это агент, визуальное представление космических объектов в режиме звездной системы.
	- корабли,
	- планеты,
	- станции,
	- звезды,
	- метеориты,
	- спасательные шлюпки,
	- торпеды
	*/
	class SS_ShipAgent : public ok::GameObject
	{
	public:
		void Update(float dt);
		ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);

		static std::unordered_map<std::string, Starpom::SS_ShipAgent*> blueprints;
	protected:
	private:
		
	};

	/*
	Ship - это космический объект (обновляется и не зависит от текущего режима, содержит агентов для каждого из режимов)
	*/
	class Ship : public ok::GameObject
	{
	public:
		Ship();
		void Update(float dt);
		ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);

		Starpom::SS_ShipAgent* ss_agent;
		glm::vec3 GetPosition();
	protected:
	private:
	};
}