#include "Application.h"
#include "Assets.h"
#include "Quad.h"


INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ok::Application app;

	
	//ok::engine::Quad qd;

	//qd.SetTransform(glm::vec3(1.0f, 1.0f, 0.0f), 90.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	LOG(INFO) << "Test";
	//ok::engine::Assets::instance();
	app.Run();

	return 0;
}