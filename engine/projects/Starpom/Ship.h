#pragma once

#include "..\..\Behaviour.h"
#include "..\..\GameObject.h"
#include "..\..\Texture.h"
#include "..\..\MeshRenderer.h"
#include "..\..\Material.h"
#include "..\..\Shader.h"
#include "..\..\Camera.h"
#include "GameInfo.h"

namespace Starpom
{
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