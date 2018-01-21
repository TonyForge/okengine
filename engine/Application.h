#pragma once

#include "Color.h"
#include "Assets.h"
#include "tinyxml\tinyxml2.h"
#include "SpriteBatch.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Input.h"
#include "LayeredRenderer.h"
#include "MeshRenderer.h"
#include "TextBatch.h"
#include "Automator.h"
#include "Pool.h"

namespace ok {
	class Application
	{
	public:
		Application();
		void Run();
		void Init();
		void Update(float dt);
	protected:
		sf::Window* window;

		int window_width;
		int window_height;
		ok::String window_title;

		int screen_width;
		int screen_height;
		int screen_depth_bits;
		int screen_stencil_bits;
		int antialiasing;

		bool fullscreen;
		bool vsync;
		bool keep_aspect_ratio;
		bool fixed_resolution;

		float framerate;

		ok::Color background_color;

		ok::graphics::RenderTarget* fixed_resolution_framebuffer;

		void LoadSettings();
	private:
	};
}