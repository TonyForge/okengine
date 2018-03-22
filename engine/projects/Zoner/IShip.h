#pragma once

#include "IGame.h"
#include "..\..\GameObject.h"
#include "..\..\Behaviour.h"
#include "..\..\Texture.h"
#include "..\..\Material.h"
#include "..\..\Camera.h"

namespace Zoner
{
	class ISpace;

	enum class ShipType
	{
		Spacecraft,
		Planet,
		Star
	};

	class ShipMaterial_SpacecraftFragment : public ok::Behaviour
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

	class ShipBlueprint : public ok::GameObject
	{
	public:
		ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);
	};

	

	class IShip : public ok::GameObject
	{
	public:
		virtual void PassTime(float hours_passed) = 0;
		virtual void ApplyPassedTime() = 0;
		virtual void Relocate(Zoner::ISpace* to) = 0;
		virtual Zoner::ISpace*& Location() = 0;

		Zoner::ShipType this_type;
		Zoner::ShipBlueprint* this_blueprint;

		bool isNPC = true;
	protected:
	private:
	};
}