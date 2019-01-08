#pragma once

#include "..\..\Application.h"
#include "..\..\Assets.h"
#include "..\..\RenderTarget.h"
#include "..\..\VisibilityMap.h"
#include "..\..\FOVMap.h"
#include "..\..\SpriteBatch.h"
#include "..\..\LineBatch.h"

namespace Kripta
{
	class IObject;

	class RoomPickData
	{
	public:
		Kripta::IObject* floor_obj = nullptr;
		Kripta::IObject* place_obj = nullptr;
		bool wall = false;
		bool floor = false;
		bool door = false;
		bool stair = false;
	};

	class LinWay
	{
	public:
		std::vector<glm::vec2> waypoints;
		int waypoints_used = 0;
		float path_progress = 0.f;

		void Reset()
		{
			waypoints.resize(0);
			waypoints_used = 0;
			path_progress = 0.f;
		}

		void Add(glm::vec2 waypoint)
		{
			waypoints.push_back(waypoint);
			waypoints_used++;
		}

		glm::vec2 Pick(float progress)
		{
			if (progress >= 1.f) return waypoints[waypoints_used - 1];

			float l = (progress * static_cast<float>(waypoints_used));
			if (glm::floor(l) >= waypoints_used-1) return waypoints[waypoints_used - 1];

			return waypoints[static_cast<int>(glm::floor(l))] +
				(l - glm::floor(l))*(waypoints[static_cast<int>(glm::floor(l)) + 1] - waypoints[static_cast<int>(glm::floor(l))]);
		}

		void RestartPath()
		{
			path_progress = 0.f;
		}

		bool IsEndOfPath()
		{
			return path_progress >= 1.f;
		}

		glm::vec2 StepPath(float step_progress)
		{
			glm::vec2 result = Pick(path_progress);
			path_progress += step_progress;
			if (path_progress >= 1.f) path_progress = 1.f;
			return result;
		}
	};

	class IGame : public ok::Application
	{
	public:
		virtual void CreateTombForMe(Kripta::IObject* me) = 0;
		virtual void PushToPostUpdateList(Kripta::IObject* obj) = 0;
		virtual void PushToDeathList(Kripta::IObject* obj) = 0;
		virtual bool GetFov(int grid_x, int grid_y) = 0;
		virtual int TurnStage() = 0;
		virtual void BlockFloor(int grid_x, int grid_y, Kripta::IObject* owner) = 0;
		virtual void BlockGrid(int grid_x, int grid_y, Kripta::IObject* owner) = 0;
		virtual void SetHeroXY(float x, float y) = 0;
		virtual glm::vec2 GetHeroXY() = 0;
		virtual Kripta::RoomPickData PickRoom(int grid_x, int grid_y) = 0;
		virtual ok::graphics::RenderTarget* GetScreenBuffer() = 0;
		virtual void LoadRoom(ok::String path) = 0;
		ok::graphics::Camera* camera;
		ok::graphics::Camera* pp_camera;

		ok::graphics::SpriteBatch* sprite_batch;
		ok::graphics::TextBatch2D* text_batch;
		ok::graphics::LineBatch* line_batch;
		ok::graphics::Font* font_def;

		static Kripta::IGame* instance;
		int turn_number = 0;
	};
}