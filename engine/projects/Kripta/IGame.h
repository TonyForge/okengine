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
	class IGame : public ok::Application
	{
	public:
		virtual ok::graphics::RenderTarget* GetScreenBuffer() = 0;
		virtual void LoadRoom(ok::String path) = 0;
		ok::graphics::Camera* camera;
		ok::graphics::Camera* pp_camera;

		ok::graphics::SpriteBatch* sprite_batch;
		ok::graphics::LineBatch* line_batch;
	};
}