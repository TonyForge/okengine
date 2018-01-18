#include "Application.h"

ok::Application::Application()
{
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

ok::graphics::SpriteBatch* my_sprite_batch;
ok::graphics::TextureRect my_sprite_batch_texrect;
ok::graphics::Camera my_camera(ok::graphics::CameraCoordinateSystem::ScreenCenter);
float camera_shift_x = 0;
float camera_shift_y = 0;
float camera_shift_z = -500;

float near_plane_z = 1.0f;
float far_plane_z = 1000.0f;

void ok::Application::Init()
{
	my_sprite_batch = new ok::graphics::SpriteBatch(10);

	my_sprite_batch_texrect.texture = ok::Assets::instance().GetTexture("test.png");
	my_sprite_batch_texrect.left = 0;
	my_sprite_batch_texrect.top = 0;
	my_sprite_batch_texrect.width = 100;
	my_sprite_batch_texrect.height = 100;
	my_sprite_batch_texrect.uv_rect = glm::vec4(0.f, 0.f, 1.f, 1.f);
}

void ok::Application::Update(float dt)
{
	ok::graphics::SpriteBatch& sprite_batch = *my_sprite_batch;

	float speed = 150.0f*dt;

	if (ok::Input::instance().KeyDown(ok::KKey::W))
	{
		camera_shift_z += speed;
	}

	if (ok::Input::instance().KeyDown(ok::KKey::S))
	{
		camera_shift_z -= speed;
	}

	if (ok::Input::instance().KeyDown(ok::KKey::D))
	{
		camera_shift_x += speed;
	}

	if (ok::Input::instance().KeyDown(ok::KKey::A))
	{
		camera_shift_x -= speed;
	}

	if (ok::Input::instance().KeyDown(ok::KKey::Q))
	{
		camera_shift_y += speed;
	}

	if (ok::Input::instance().KeyDown(ok::KKey::Z))
	{
		camera_shift_y -= speed;
	}

	if (ok::Input::instance().KeyDown(ok::KKey::F))
	{
		if (ok::Input::instance().KeyDown(ok::KKey::G))
		{
			far_plane_z += 1.0f;
		}

		if (ok::Input::instance().KeyDown(ok::KKey::B))
		{
			far_plane_z -= 1.0f;
		}
	}

	if (ok::Input::instance().KeyDown(ok::KKey::N))
	{
		if (ok::Input::instance().KeyDown(ok::KKey::J))
		{
			near_plane_z += 1.0f;
		}

		if (ok::Input::instance().KeyDown(ok::KKey::M))
		{
			near_plane_z -= 1.0f;
		}
	}

	if (far_plane_z > 2000.0f) far_plane_z = 2000.0f;
	if (far_plane_z < 0.0) far_plane_z = 0.0f;
	if (near_plane_z > far_plane_z) near_plane_z = far_plane_z;
	if (near_plane_z < 0) near_plane_z = 0.0f;

	my_camera.SetProjectionPersp((float)(screen_width), (float)(screen_height), 45.0f, near_plane_z, far_plane_z);
	//cam.SetProjectionOrtho((float)(screen_width), (float)(screen_height), near_plane_z, far_plane_z);

	my_camera.BeginTransform(ok::TransformSpace::WorldSpace);
	my_camera.SetPosition(glm::vec3(camera_shift_x, camera_shift_y, camera_shift_z));
	//cam.SetRotation(glm::vec3(sh1*360.0f, 0.0f, 0.0f));
	//cam.SetForward(glm::vec3(1.0f,0.0f,1.0f));
	my_camera.SetForward((glm::vec3(0.0f, 0.0f, 0.0f) - my_camera.GetPosition()));
	my_camera.EndTransform();


	ok::graphics::Camera::PushCamera(&my_camera);


	sprite_batch.BatchBegin(-1.0f);

	sprite_batch.Draw(&my_sprite_batch_texrect, glm::vec2(0.f, 0.f), 0.f*360.0f, glm::vec2(1.0f, 1.0f)*(1.0f + 0.f), true);
	sprite_batch.Draw(&my_sprite_batch_texrect, glm::vec2(100.0f*0.f, 0.f), 0.f, glm::vec2(1.0f, 1.0f)*(1.0f + 1.0f*0.f), true);


	sprite_batch.BatchEnd();


	ok::graphics::LayeredRenderer::instance().Flush();
	

}

void ok::Application::LoadSettings()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("settings.xml");

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
