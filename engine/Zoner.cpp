#include "Zoner.h"

#ifdef ZONER

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Zoner::Game game;

	LOG(INFO) << "Zoner initiated...";
	game.Run();

	return 0;
}

#endif
