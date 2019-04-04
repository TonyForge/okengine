#pragma once

#include "IActor.h"
#include "..\..\Camera.h"
#include "..\..\SpriteBatch.h"

namespace ZonerX
{
	enum class StarSystemPickRule
	{
		NearestOnly,
		FarthestOnly,
		FirstOnly,
		LastOnly,
		Everything
	};

	enum class StarSystemPickSortRule
	{
		FarToNear,
		NearToFar,
		Unsorted
	};

	class IStarSystem : public ZonerX::IActor
	{
	public:
		IStarSystem() : camera(ok::graphics::CameraCoordinateSystem::ScreenCenter),
						sprite_batch(1000)
		{
			camera.SetProjectionOrtho(1024, 768, 1000.f, 1.f);
			camera.BeginTransform();
			camera.SetPosition(glm::vec3(0.f, 0.f, 500.f));
			camera.EndTransform(true);
			//camera.LookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
		}

		virtual void OnDayBegin() = 0;
		virtual void OnDayEnd() = 0;
		virtual void Render(float dt) = 0;

		virtual std::vector<ZonerX::IActor*> Pick(float x, float y, float radius = -1, ZonerX::StarSystemPickRule pick_rule = ZonerX::StarSystemPickRule::Everything, ZonerX::ActorFamilyID family = ZonerX::ActorFamilyID::Any, ZonerX::StarSystemPickSortRule pick_sort_rule = ZonerX::StarSystemPickSortRule::Unsorted);

		std::vector<ZonerX::IActor*> ship_list;
		std::vector<ZonerX::IActor*> station_list; //can land
		std::vector<ZonerX::IActor*> planet_list; //can land
		std::vector<ZonerX::IActor*> jump_gate_list;
		std::vector<ZonerX::IActor*> jump_hole_list;

		std::vector<ZonerX::IActor*> meteorite_list;
		std::vector<ZonerX::IActor*> lootbox_list; //can pickup
		std::vector<ZonerX::IActor*> star_list;

		std::vector<ZonerX::IActor*> platform_list;

		ok::graphics::Camera camera;
		ok::graphics::SpriteBatch sprite_batch;
		
		std::vector<ZonerX::IActor*> destroy_list;
		std::vector<ZonerX::IActor*> income_list;
		std::vector<ZonerX::IActor*> outcome_list;
	protected:
	private:
	};
}