#include "Treasurer.h"

#ifdef TREASURER

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Treasurer::Game game;
	/*Kripta::Game game;*/

	LOG(INFO) << "Treasurer initiated...";
	game.Run();
	//game.Run();

	return 0;
}

#endif