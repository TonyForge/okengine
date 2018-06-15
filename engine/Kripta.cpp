#include "Kripta.h"

#ifdef KRIPTA

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Kripta::Game game;

	LOG(INFO) << "Kripta initiated...";
	game.Run();

	return 0;
}

#endif