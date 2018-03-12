#pragma once

#include "..\..\Behaviour.h"
#include "..\..\GameObject.h"
#include "..\..\Texture.h"
#include "..\..\MeshRenderer.h"
#include "..\..\Material.h"
#include "..\..\Shader.h"
#include "..\..\Camera.h"
#include "GameInfo.h"
#include "Action.h"

namespace Starpom
{
	enum class Ship_ID_Type
	{
		unknown,
		sector,
		system,
		planet,
		star,
		asteroid,
		spaceship,
		station,
		wormhole
	};

	class Ship_ID
	{
	public:
		Ship_ID();
		const std::string& ShipIDToString();

		Starpom::Ship_ID_Type ship_id_type;

		static const unsigned int ship_id_length = 4;
		unsigned int ship_id[ship_id_length];
		std::string ship_id_string;
		static unsigned int ship_global_id_next[ship_id_length]; //must be saved at save game and loaded back at load game
	};

	bool operator ==(const Starpom::Ship_ID& left, const Starpom::Ship_ID& right);

	//SS - Star System - режим звездной системы (локации с планетами и звездами)

	class SS_ShipMaterial_Spaceship : public ok::Behaviour
	{
	public:
		void Update(float dt);
		void OnMaterialBind(ok::graphics::Material& material);
		ok::Behaviour* Duplicate(ok::Behaviour * _clone = nullptr);

		ok::graphics::Texture* _Maps;
		ok::graphics::Texture* _Normals;
		ok::graphics::Texture* _Facture;

		glm::vec4 _FactureTransform;
		glm::vec4 _ColorMain_0;
		glm::vec4 _ColorMain_1;
		glm::vec4 _ColorMain_DarkMultiplier;
		glm::vec4 _ColorSec_0;
		glm::vec4 _ColorSec_1;
		glm::vec4 _ColorSet3_0;
		glm::vec4 _ColorSet3_1;
	protected:
	private:
	};

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
	class SS_ShipAgent : public ok::GameObject, public Starpom::Ship_ID
	{
	public:
		void Update(float dt);
		ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);

		static std::unordered_map<std::string, Starpom::SS_ShipAgent*> blueprints;

		//Actions
		class Action_FlyInSS : public Starpom::ContiniousAction
		{
		public:
			Action_FlyInSS(Starpom::SS_ShipAgent* _actor);

			Starpom::SS_ShipAgent* actor;

			Starpom::SmoothPath path;
			void OnTickProgress();
		};

		std::shared_ptr<ok::PoolContainer<Starpom::ActionOwner>> actions;
	protected:
	private:
	};

	/*
	Ship - это космический объект (обновляется и не зависит от текущего режима, содержит агентов для каждого из режимов)
	*/
	class Ship : public ok::GameObject, public Starpom::Ship_ID
	{
	public:
		//---
		Ship();
		virtual void Update(float dt);
		ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);

		//---
		glm::vec3 GetPosition();

		//Agents
		Starpom::SS_ShipAgent* ss_agent;

		//Properties
		ok::String name;

		//Nested classes
		class Universe
		{
		public:
			std::unordered_map<std::string, Starpom::Ship*> sectors;
			std::unordered_map<std::string, Starpom::Ship*> systems;
			std::unordered_map<std::string, Starpom::Ship*> planets;
			std::unordered_map<std::string, Starpom::Ship*> stars;
			std::unordered_map<std::string, Starpom::Ship*> asteroids;
			std::unordered_map<std::string, Starpom::Ship*> spaceships;
			std::unordered_map<std::string, Starpom::Ship*> stations;
			std::unordered_map<std::string, Starpom::Ship*> wormholes;

			Starpom::Ship* Find(Starpom::Ship_ID id);
		};

		static Starpom::Ship::Universe* universe;
	protected:
	private:
	};
}