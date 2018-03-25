#pragma once

#include "IGame.h"
#include "..\..\GameObject.h"
#include "..\..\Behaviour.h"
#include "..\..\Texture.h"
#include "..\..\Material.h"
#include "..\..\Camera.h"
#include "..\..\MeshRenderer.h"
#include "Collision.h"
#include "SmoothPath.h"

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

		//bounder is ellipsoid, axis alighed in local coordinate system
		glm::vec3 bounder_center;
		glm::vec3 bounder_axis;

		Zoner::Collision::Point Bound(glm::vec3 world_position);

		void CalculateBounder();
	private:
		void _CalculateBounder(ok::Transform* part, glm::vec3& min_axis, glm::vec3& max_axis);
	};

	

	class IShip : public ok::GameObject
	{
	public:
		IShip();

		virtual void PassTime(float hours_passed) = 0;
		virtual void ApplyPassedTime() = 0;
		virtual void OnNewDay() = 0;
		virtual void Relocate(Zoner::ISpace* to) = 0;
		virtual Zoner::ISpace*& Location() = 0;

		virtual void ClickOnceAt(glm::vec2 space_xy, bool ignore_objects = false) = 0;

		Zoner::ShipType this_type;
		Zoner::ShipBlueprint* this_blueprint;
		Zoner::SmoothPath trajectory;

		//tmp begin
		float engine_speed; //distance per hour
		float trajectory_progress;
		//tmp end

		bool isNPC = true;
		
		Zoner::Collision::Point Pick(glm::vec3 world_position);
		Zoner::IShip* picked_object = nullptr;
		int picked_object_picks_counter = 0;

		virtual void Player_UpdateDecisions(float dt) = 0;

		
	protected:
	private:
	};
}