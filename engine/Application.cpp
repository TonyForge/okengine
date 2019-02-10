#include "Application.h"

ok::ApplicationProperties ok::Application::Properties;

ok::Application::Application()
{
	_settings_file_name = "settings.xml";
}


void ok::Application::Run()
{
	LoadSettings();

	sf::ContextSettings contextSettings;
	contextSettings.antialiasingLevel = antialiasing;
	contextSettings.depthBits = screen_depth_bits;
	contextSettings.stencilBits = screen_stencil_bits;
	//OpenGL 4.0 (was 3.3)
	contextSettings.majorVersion = 4;
	contextSettings.minorVersion = 0;
	contextSettings.attributeFlags = sf::ContextSettings::Core;
	contextSettings.sRgbCapable = false;

	sf::Uint32 style;
	if (fullscreen)
		style = sf::Style::Fullscreen;
	else
		style = sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close;

	window = new sf::Window(sf::VideoMode(screen_width, screen_height, screen_depth_bits), window_title, style, contextSettings);

	window->setVerticalSyncEnabled(vsync);
	window->setFramerateLimit(0);
	window->setActive(true);
	glewInit();

	ok::graphics::Camera::SetGLViewport(0, 0, window_width, window_height);

	ok::graphics::Camera::_viewport_x = 0;
	ok::graphics::Camera::_viewport_y = 0;
	ok::graphics::Camera::_viewport_w = static_cast<int>(window_width);
	ok::graphics::Camera::_viewport_h = static_cast<int>(window_height);

	ok::graphics::SpriteBatch* fixed_resolution_batch = nullptr;

	if (fixed_resolution)
	{
		if (fixed_resolution_resample_enabled)
		{
			fixed_resolution_framebuffer = new ok::graphics::RenderTarget(screen_width, screen_height, true, false, false, true);
			fixed_resolution_resample_framebuffer = new ok::graphics::RenderTarget(
				static_cast<int>(glm::round(static_cast<float>(screen_width) * fixed_resolution_resample_scale)), 
				static_cast<int>(glm::round(static_cast<float>(screen_height) * fixed_resolution_resample_scale)),
				true, true, true, true);
		}
		else
		{
			fixed_resolution_framebuffer = new ok::graphics::RenderTarget(screen_width, screen_height, true, true, true, true);
		}

		fixed_resolution_batch = new ok::graphics::SpriteBatch(1);
	}
	else fixed_resolution_framebuffer = nullptr;

	//-------------------------------
	Init();
	//-------------------------------

	bool running = true;
	float viewport_x = 0.f, viewport_y = 0.f, viewport_w = (float)window_width, viewport_h = (float)window_height;

	sf::Clock timer;
	float time_delta = 0.f;
	float seconds_per_tick = 1.f / framerate;

	//Simulate Window Resized Event At Startup
	window_width = window->getSize().x;
	window_height = window->getSize().y;

	if (keep_aspect_ratio)
	{
		float screen_to_window_scale = glm::min(static_cast<float>(window_width) / static_cast<float>(screen_width), static_cast<float>(window_height) / static_cast<float>(screen_height));

		viewport_w = static_cast<float>(screen_width) * screen_to_window_scale;
		viewport_h = static_cast<float>(screen_height) * screen_to_window_scale;

		viewport_x = (static_cast<float>(window_width) - viewport_w) * 0.5f;
		viewport_y = (static_cast<float>(window_height) - viewport_h) * 0.5f;

		ok::graphics::Camera::SetGLViewport(viewport_x, viewport_y, viewport_w, viewport_h);

		ok::graphics::Camera::_viewport_x = static_cast<int>(viewport_x);
		ok::graphics::Camera::_viewport_y = static_cast<int>(viewport_y);
		ok::graphics::Camera::_viewport_w = static_cast<int>(viewport_w);
		ok::graphics::Camera::_viewport_h = static_cast<int>(viewport_h);
	}
	else
	{
		ok::graphics::Camera::SetGLViewport(0, 0, window_width, window_height);

		ok::graphics::Camera::_viewport_x = 0;
		ok::graphics::Camera::_viewport_y = 0;
		ok::graphics::Camera::_viewport_w = window_width;
		ok::graphics::Camera::_viewport_h = window_height;

		viewport_x = 0;
		viewport_y = 0;
		viewport_w = static_cast<float>(window_width);
		viewport_h = static_cast<float>(window_height);
	}

	//Application Loop
	while (running)
	{
		sf::Event e;

		while (window->pollEvent(e))
		{
			switch (e.type)
			{
			case sf::Event::Closed :
				{
					running = false;
				}
				break;
				case sf::Event::Resized :
				{
					window_width = e.size.width;
					window_height = e.size.height;

					if (keep_aspect_ratio)
					{
						float screen_to_window_scale = glm::min(static_cast<float>(window_width) / static_cast<float>(screen_width), static_cast<float>(window_height) / static_cast<float>(screen_height));

						viewport_w = static_cast<float>(screen_width) * screen_to_window_scale;
						viewport_h = static_cast<float>(screen_height) * screen_to_window_scale;

						viewport_x = (static_cast<float>(window_width) - viewport_w) * 0.5f;
						viewport_y = (static_cast<float>(window_height) - viewport_h) * 0.5f;

						ok::graphics::Camera::SetGLViewport(viewport_x, viewport_y, viewport_w, viewport_h);

						ok::graphics::Camera::_viewport_x = static_cast<int>(viewport_x);
						ok::graphics::Camera::_viewport_y = static_cast<int>(viewport_y);
						ok::graphics::Camera::_viewport_w = static_cast<int>(viewport_w);
						ok::graphics::Camera::_viewport_h = static_cast<int>(viewport_h);
					}
					else
					{
						ok::graphics::Camera::SetGLViewport(0, 0, window_width, window_height);

						ok::graphics::Camera::_viewport_x = 0;
						ok::graphics::Camera::_viewport_y = 0;
						ok::graphics::Camera::_viewport_w = window_width;
						ok::graphics::Camera::_viewport_h = window_height;

						viewport_x = 0;
						viewport_y = 0;
						viewport_w = static_cast<float>(window_width);
						viewport_h = static_cast<float>(window_height);
					}
				}
				break;
				case sf::Event::KeyPressed :
				{
					if (e.key.code != sf::Keyboard::Unknown)
						ok::Input::instance().OnKeyDown(static_cast<int>(e.key.code));
				}
				break;
				case sf::Event::KeyReleased :
				{
					if (e.key.code != sf::Keyboard::Unknown)
						ok::Input::instance().OnKeyUp(static_cast<int>(e.key.code));
				}
				break;
				case sf::Event::MouseButtonPressed :
				{
					ok::Input::instance().OnMouseDown(static_cast<int>(e.mouseButton.button));
				}
				break;
				case sf::Event::MouseButtonReleased:
				{
					ok::Input::instance().OnMouseUp(static_cast<int>(e.mouseButton.button));
				}
				break;
				case sf::Event::MouseMoved :
				{
					ok::Input::instance().OnMouseMove((e.mouseMove.x - viewport_x) / viewport_w, (e.mouseMove.y - viewport_y) / viewport_h, (float)screen_width, (float)screen_height);
				}
				break;
			}
		}

		time_delta += timer.getElapsedTime().asSeconds();
		timer.restart();

		if (time_delta >= seconds_per_tick)
		{
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			if (fixed_resolution)
			{
				if (fixed_resolution_resample_enabled)
				{
					fixed_resolution_resample_framebuffer->BindTarget();
				}
				else
				{
					fixed_resolution_framebuffer->BindTarget();
				}
			}

			glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);

			if (auto_clear_depth_enabled)
			{
				glDepthMask(GL_TRUE);
				glClearDepth(depth_clear_with_value);
			}

			if (auto_clear_stencil_enabled)
			{
				glClearStencil(stencil_clear_with_value);
			}
			
			glClear(GL_COLOR_BUFFER_BIT | ((auto_clear_depth_enabled) ? GL_DEPTH_BUFFER_BIT : 0) | ((auto_clear_stencil_enabled) ? GL_STENCIL_BUFFER_BIT : 0));

			//-----------------------------------
			Properties.app_instance = this;

			Properties.screen_width = screen_width;
			Properties.screen_height = screen_height;
			
			Properties.time_delta = time_delta;
			//-----------------------------------

			Update(time_delta);

			//-----------------------------------

			if (fixed_resolution)
			{
				while (ok::graphics::Camera::GetCurrent() != nullptr)
				{
					ok::graphics::Camera::PopCamera();
				}

				if (fixed_resolution_resample_enabled)
				{
					fixed_resolution_resample_framebuffer->UnbindTarget();

					ok::graphics::Camera resample_camera(ok::graphics::CameraCoordinateSystem::ScreenCenter);
					resample_camera.SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), -2.f, -1.f);

					ok::graphics::Camera::PushCamera(&resample_camera);
						fixed_resolution_framebuffer->BindTarget();

						fixed_resolution_batch->BatchBegin(1.5f);
							ok::graphics::Texture fixed_resolution_framebuffer_texture = ok::graphics::Texture(fixed_resolution_resample_framebuffer);
							fixed_resolution_batch->Draw(&fixed_resolution_framebuffer_texture, glm::vec2(0.f, 0.f), glm::vec2(screen_width, screen_height), false);
						fixed_resolution_batch->BatchEnd();

						fixed_resolution_framebuffer->UnbindTarget();
					ok::graphics::Camera::PopCamera();
				}
				else
				{
					fixed_resolution_framebuffer->UnbindTarget();
				}

				if (keep_aspect_ratio)
				{
					ok::graphics::Camera::SetGLViewport(viewport_x, viewport_y, viewport_w, viewport_h);

					ok::graphics::Camera::_viewport_x = static_cast<int>(viewport_x);
					ok::graphics::Camera::_viewport_y = static_cast<int>(viewport_y);
					ok::graphics::Camera::_viewport_w = static_cast<int>(viewport_w);
					ok::graphics::Camera::_viewport_h = static_cast<int>(viewport_h);
				}
				else
				{
					ok::graphics::Camera::SetGLViewport(0, 0, window_width, window_height);

					ok::graphics::Camera::_viewport_x = 0;
					ok::graphics::Camera::_viewport_y = 0;
					ok::graphics::Camera::_viewport_w = window_width;
					ok::graphics::Camera::_viewport_h = window_height;
				}

				//Draw fixed resolution render target containing game screen to window framebuffer
				//it size is 2x2 texels because opengl default coordinate space is -1 .. 1, so it 2 texels wide
				fixed_resolution_batch->BatchBegin(0.f);
					ok::graphics::Texture fixed_resolution_framebuffer_texture = ok::graphics::Texture(fixed_resolution_framebuffer);
					fixed_resolution_batch->Draw(&fixed_resolution_framebuffer_texture, glm::vec2(0.f, 0.f), glm::vec2(2.f, 2.f), false);
				fixed_resolution_batch->BatchEnd();
			}
			else
			{
				while (ok::graphics::Camera::GetCurrent() != nullptr)
				{
					ok::graphics::Camera::PopCamera();
				}
			}

			window->display();

			ok::Input::instance().Update();

			time_delta = 0.f;
		}

		if (_application_shutdown == true) running = false;
	}

	if (fixed_resolution)
	{
		delete fixed_resolution_batch;
		delete fixed_resolution_framebuffer;
	}

	window->close();
	delete window;
}

void ok::Application::Init()
{
}

void ok::Application::Update(float dt)
{
}

void ok::Application::Shutdown()
{
	_application_shutdown = true;
}


void ok::Application::LoadSettings()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(_settings_file_name.c_str());

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* inner_elem;
	elem = doc.FirstChildElement("settings")->FirstChildElement("application");

	window_title = elem->Attribute("title");
	
	ok::Assets::instance().assets_root_folder = elem->FirstChildElement("assets")->Attribute("root_folder");

	elem = elem->FirstChildElement("screen");

	screen_width = elem->IntAttribute("width", 640);
	screen_height = elem->IntAttribute("height", 480);
	window_width = screen_width;
	window_height = screen_height;
	screen_depth_bits = elem->IntAttribute("depth", 24);
	screen_stencil_bits = elem->IntAttribute("stencil", 8);
	antialiasing = elem->IntAttribute("antialiasing", 0);
	framerate = elem->FloatAttribute("framerate", 60);
	vsync = elem->BoolAttribute("vsync", true);
	keep_aspect_ratio = elem->BoolAttribute("keep_aspect_ratio", true);
	fixed_resolution = elem->BoolAttribute("fixed_resolution", true);
	fixed_resolution_resample_enabled = elem->BoolAttribute("fixed_resolution_resample_enabled", false);
	fixed_resolution_resample_scale = elem->FloatAttribute("fixed_resolution_resample_scale", 1.0f);
	ok::graphics::Camera::_fixed_resolution_resample_scale = fixed_resolution_resample_scale;
	ok::graphics::Camera::_fixed_resolution_enabled = fixed_resolution;
	ok::graphics::Camera::_keep_aspect_ratio_enabled = keep_aspect_ratio;

	fullscreen = elem->BoolAttribute("fullscreen", false);

	inner_elem = elem->FirstChildElement("background");
	background_color = ok::Color(inner_elem->IntAttribute("r"), inner_elem->IntAttribute("g"), inner_elem->IntAttribute("b"), 255);

	inner_elem = elem->FirstChildElement("depth_clear_with");
	depth_clear_with_value = inner_elem->FloatAttribute("value", 0.f);

	inner_elem = elem->FirstChildElement("stencil_clear_with");
	stencil_clear_with_value = inner_elem->IntAttribute("value", 0);

	EnableFeature(ok::ApplicationFeature::AutoClearDepth);
	EnableFeature(ok::ApplicationFeature::AutoClearStencil);
}

void ok::Application::EnableFeature(ok::ApplicationFeature feature)
{
	switch (feature)
	{
	case ok::ApplicationFeature::AutoClearDepth :
	{
		auto_clear_depth_enabled = true;
	}
	break;
	case ok::ApplicationFeature::AutoClearStencil:
	{
		auto_clear_stencil_enabled = true;
	}
	break;
	}
}

void ok::Application::DisableFeature(ok::ApplicationFeature feature)
{
	switch (feature)
	{
	case ok::ApplicationFeature::AutoClearDepth:
	{
		auto_clear_depth_enabled = false;
	}
	break;
	case ok::ApplicationFeature::AutoClearStencil:
	{
		auto_clear_stencil_enabled = false;
	}
	break;
	}
}
