#pragma once

#include "..\..\Application.h"
#include "..\..\Assets.h"
#include "..\..\Camera.h"
#include "..\..\Random.h"
#include "..\..\Input.h"
#include "..\..\GameObject.h"
#include "..\..\TextBatch.h"

namespace DOE
{
	class Global
	{
	public:
		int g_tile_size = 32;
		int g_floor_size = 100;
		int g_screen_width = 1024;
		int g_screen_height = 768;
		float g_screen_width_f = 1024.f;
		float g_screen_height_f = 768.f;
		ok::graphics::Camera g_camera;

		Global() : g_camera(ok::graphics::CameraCoordinateSystem::ScreenCenter)
		{
			g_camera.SetProjectionOrtho(g_screen_width_f, g_screen_height_f, 1.f, 100.f);
		}
	};

	static DOE::Global global;

	class IDOEGame : public ok::Application
	{
	public:
		virtual void PlaceCamera(float x, float y) = 0;
		virtual void SetCameraSpeed(float speed) = 0;
		virtual void SetCameraTarget(float x, float y) = 0;
		virtual ok::Rect2Df GetCameraViewport() = 0;
		virtual ok::graphics::TextBatch2D* GetTextBatch() = 0;
		static DOE::IDOEGame* instance() { return _instance; }
	protected:
		static DOE::IDOEGame* _instance;
	private:
	};
}