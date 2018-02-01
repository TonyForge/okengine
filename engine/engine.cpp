#ifdef OKENGINE

#include "Application.h"
#include "Assets.h"
#include "Quad.h"


INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ok::Application app;

	LOG(INFO) << "Test";
	app.Run();

	return 0;
}

#endif