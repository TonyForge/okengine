#pragma once

#include "UID.h"
#include "IGame.h"
#include "..\..\GameObject.h"
#include "..\..\Behaviour.h"
#include "..\..\Texture.h"
#include "..\..\Material.h"
#include "..\..\Camera.h"
#include "..\..\MeshRenderer.h"
#include "Collision.h"
#include "SmoothPath.h"
#include "Commands.h"
#include "RequestsDispatcher.h"
#include "IItem.h"

namespace Zoner
{
	class ISpace;

	enum class ShipType
	{
		ST_Spacecraft,
		ST_Planet,
		ST_Star,
		ST_Jumphole
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
		float GetBounderMaxRadiusInWorldSpace();

		void CalculateBounder();
	private:
		void _CalculateBounder(ok::Transform* part, glm::vec3& min_axis, glm::vec3& max_axis);
	};

	class IShip : public ok::GameObject, public Zoner::UID, public Zoner::RequestsDispatcher, public Zoner::ISaveable
	{
	public:
		IShip();

		virtual void Relocate(Zoner::ISpace* to) = 0;
		virtual void RelocationComplete() = 0;
		virtual Zoner::ISpace*& Location() = 0;

		virtual void ClickOnceAt(glm::vec2 space_xy, bool ignore_objects = false) = 0;

		Zoner::ShipType this_type;
		Zoner::ShipBlueprint* this_blueprint;
		Zoner::SmoothPath trajectory;

		Zoner::IItem* this_item; //this is the key to all properties of object (spacecrafts, planets, everything keeps its properties as items).

		Zoner::CommandsList cmd_sequence;
		Zoner::CommandsList cmd_parallel;

		//tmp begin
		float engine_speed; //distance per hour
		float trajectory_progress;
		bool afterburner_enabled;
		//tmp end

		bool isInViewport = true;
		bool isNPC = true;
		bool relocationInProgress = false;
		glm::vec2 relocationDestinationPosition;
		
		virtual Zoner::Collision::Point Pick(glm::vec3 world_position);
		Zoner::IShip* picked_object = nullptr;
		int picked_object_picks_counter = 0;

		virtual void Player_UpdateDecisions(float dt) = 0;
		virtual void NPC_MakeDecisions() = 0;
		
		virtual void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;
		virtual void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;

		Zoner::IItem* FindItem(Zoner::UID& item_uid);
		void CollectItems(std::map<Zoner::UID, Zoner::IItem*>& _collection);

		virtual void OnItemIn(Zoner::IItem* item_ptr);
		virtual void OnItemOut(Zoner::IItem* item_ptr);
		std::map<Zoner::UID, Zoner::IItem*> this_items;

		ok::String _gameengine_id;
	protected:
	private:
	};
}