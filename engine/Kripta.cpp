#include "Kripta.h"

#ifdef KRIPTA

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Kripta::App app;

	LOG(INFO) << "Test";
	app.Run();

	return 0;
}

Kripta::App::App()
{
	_settings_file_name = "kripta.settings.xml";
}

ok::GameObject go;
ok::graphics::Camera camera(ok::graphics::CameraCoordinateSystem::ScreenCenter);

void Kripta::App::Init()
{
	Kripta::Maze::LoadMeshes();

	go.AddComponent(new ok::graphics::MeshRenderer());
	go.GetComponent<ok::graphics::MeshRenderer>()->SetMaterial(ok::Assets::instance().GetMaterial("KriptaWallMaterial.xml"));
	go.GetComponent<ok::graphics::MeshRenderer>()->SetMesh(ok::Assets::instance().GetMesh("walltest.mesh"));
	go.GetComponent<ok::graphics::MeshRenderer>()->SetLayer(0);
	go.GetComponent<ok::graphics::MeshRenderer>()->SetStage(ok::graphics::LayeredRendererStage::Stage);
	camera.SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), 1.f, 1000.f);

	go.BeginTransform();
	{
		go.SetPosition(glm::vec3(0.f, 0.f, 0.f));
		go.SetScale(glm::vec3(100.f, 100.f, 100.f));
	}
	go.EndTransform(true);
}

glm::vec3 eye_pos(0.0f,0.0f,0.0f);

void Kripta::App::Update(float dt)
{
	float speed = 1.f;

	if (ok::Input::o().KeyDown(ok::KKey::A))
	{
		eye_pos.x -= speed * dt;
	}
	if (ok::Input::o().KeyDown(ok::KKey::D))
	{
		eye_pos.x += speed * dt;
	}
	if (ok::Input::o().KeyDown(ok::KKey::W))
	{
		eye_pos.y -= speed * dt;
	}
	if (ok::Input::o().KeyDown(ok::KKey::S))
	{
		eye_pos.y += speed * dt;
	}

	go.GetComponent<ok::graphics::MeshRenderer>()->GetMaterial()->GetShader()->SetUniform("eye_pos", eye_pos);

	camera.BeginTransform();
	{
		camera.SetPosition(glm::vec3(eye_pos.x*100.0f, eye_pos.y*100.0f, -500.f));
		camera.SetForward(glm::vec3(0.f, 0.f, 1.f));
		camera.SetUp(glm::vec3(0.f, 1.f, 0.f));
	}
	camera.EndTransform(true);

	ok::graphics::Camera::PushCamera(&camera);
	{	
		go.Update(dt);
		ok::graphics::LayeredRenderer::instance().Flush();
	}
	ok::graphics::Camera::PopCamera();
}

#endif