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

void Kripta::App::Init()
{
}

void Kripta::App::Update(float dt)
{
}

#endif