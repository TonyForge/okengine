#include "ZonerX.h"

#ifdef ZONERX

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ZonerX::Game game;

	LOG(INFO) << "ZonerX initiated...";
	game.Run();

	return 0;
}

#endif