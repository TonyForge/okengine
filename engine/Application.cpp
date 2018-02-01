#include "Application.h"

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

	glViewport(0, 0, window_width, window_height);

	ok::graphics::Camera::_viewport_x = 0;
	ok::graphics::Camera::_viewport_y = 0;
	ok::graphics::Camera::_viewport_w = static_cast<int>(window_width);
	ok::graphics::Camera::_viewport_h = static_cast<int>(window_height);

	ok::graphics::SpriteBatch* fixed_resolution_batch = nullptr;

	if (fixed_resolution)
	{
		fixed_resolution_framebuffer = new ok::graphics::RenderTarget(screen_width, screen_height, true, true, true, true);
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

		glViewport((GLint)viewport_x, (GLint)viewport_y, (GLint)viewport_w, (GLint)viewport_h);

		ok::graphics::Camera::_viewport_x = static_cast<int>(viewport_x);
		ok::graphics::Camera::_viewport_y = static_cast<int>(viewport_y);
		ok::graphics::Camera::_viewport_w = static_cast<int>(viewport_w);
		ok::graphics::Camera::_viewport_h = static_cast<int>(viewport_h);
	}
	else
	{
		glViewport((GLint)0, (GLint)0, (GLint)window_width, (GLint)window_height);

		ok::graphics::Camera::_viewport_x = 0;
		ok::graphics::Camera::_viewport_y = 0;
		ok::graphics::Camera::_viewport_w = static_cast<int>(window_width);
		ok::graphics::Camera::_viewport_h = static_cast<int>(window_height);
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

						glViewport((GLint)viewport_x, (GLint)viewport_y, (GLint)viewport_w, (GLint)viewport_h);

						ok::graphics::Camera::_viewport_x = static_cast<int>(viewport_x);
						ok::graphics::Camera::_viewport_y = static_cast<int>(viewport_y);
						ok::graphics::Camera::_viewport_w = static_cast<int>(viewport_w);
						ok::graphics::Camera::_viewport_h = static_cast<int>(viewport_h);
					}
					else
					{
						glViewport((GLint)0, (GLint)0, (GLint)window_width, (GLint)window_height);

						ok::graphics::Camera::_viewport_x = 0;
						ok::graphics::Camera::_viewport_y = 0;
						ok::graphics::Camera::_viewport_w = static_cast<int>(window_width);
						ok::graphics::Camera::_viewport_h = static_cast<int>(window_height);
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
				fixed_resolution_framebuffer->BindTarget();
				glViewport((GLint)0, (GLint)0, (GLint)screen_width, (GLint)screen_height);

				ok::graphics::Camera::_viewport_x = 0;
				ok::graphics::Camera::_viewport_y = 0;
				ok::graphics::Camera::_viewport_w = static_cast<int>(screen_width);
				ok::graphics::Camera::_viewport_h = static_cast<int>(screen_height);
			}

			glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
			glClear(GL_COLOR_BUFFER_BIT);

			glClearDepth(0.f);
			glClearStencil(0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			//-----------------------------------

			Update(time_delta);

			//-----------------------------------

			if (fixed_resolution)
			{
				fixed_resolution_framebuffer->UnbindTarget();

				if (keep_aspect_ratio)
				{
					glViewport((GLint)viewport_x, (GLint)viewport_y, (GLint)viewport_w, (GLint)viewport_h);

					ok::graphics::Camera::_viewport_x = static_cast<int>(viewport_x);
					ok::graphics::Camera::_viewport_y = static_cast<int>(viewport_y);
					ok::graphics::Camera::_viewport_w = static_cast<int>(viewport_w);
					ok::graphics::Camera::_viewport_h = static_cast<int>(viewport_h);
				}
				else
				{
					glViewport((GLint)0, (GLint)0, (GLint)window_width, (GLint)window_height);

					ok::graphics::Camera::_viewport_x = 0;
					ok::graphics::Camera::_viewport_y = 0;
					ok::graphics::Camera::_viewport_w = static_cast<int>(window_width);
					ok::graphics::Camera::_viewport_h = static_cast<int>(window_height);
				}


				while (ok::graphics::Camera::GetCurrent() != nullptr)
				{
					ok::graphics::Camera::PopCamera();
				}

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


void ok::Application::LoadSettings()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(_settings_file_name.c_str());

	tinyxml2::XMLElement* elem;
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
	ok::graphics::Camera::_fixed_resolution_enabled = fixed_resolution;
	ok::graphics::Camera::_keep_aspect_ratio_enabled = keep_aspect_ratio;

	fullscreen = elem->BoolAttribute("fullscreen", false);

	elem = elem->FirstChildElement("background");
	background_color = ok::Color((unsigned char)elem->IntAttribute("r"), (unsigned char)elem->IntAttribute("g"), (unsigned char)elem->IntAttribute("b"), 255);
}
