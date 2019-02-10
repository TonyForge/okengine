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
#include "Sound.h"
#include "UI.h"

namespace ok {

	enum class ApplicationFeature
	{
		AutoClearDepth,
		AutoClearStencil
	};

	class Application;

	class ApplicationProperties
	{
	public:
		int screen_width;
		int screen_height;
		float time_delta;
		ok::Application* app_instance;
	};

	class Application
	{
	public:
		Application();
		void Run();
		virtual void Init();
		virtual void Update(float dt);
		virtual void Shutdown();
		static ok::ApplicationProperties Properties;
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
		float depth_clear_with_value;
		int stencil_clear_with_value;
		

		bool fullscreen;
		bool vsync;
		bool keep_aspect_ratio;
		bool fixed_resolution;
		bool fixed_resolution_resample_enabled;
		float fixed_resolution_resample_scale;

		bool auto_clear_depth_enabled;
		bool auto_clear_stencil_enabled;

		float framerate;

		ok::Color background_color;

		ok::graphics::RenderTarget* fixed_resolution_framebuffer;
		ok::graphics::RenderTarget* fixed_resolution_resample_framebuffer;

		std::string _settings_file_name;
		void LoadSettings();
		void EnableFeature(ok::ApplicationFeature feature);
		void DisableFeature(ok::ApplicationFeature feature);
	private:

		bool _application_shutdown = false;
	};
}