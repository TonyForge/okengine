#include "ZonaX.h"

#ifdef ZONAX

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ZonaX::Game game;

	LOG(INFO) << "ZonaX initiated...";
	game.Run();

	return 0;
}

#endif