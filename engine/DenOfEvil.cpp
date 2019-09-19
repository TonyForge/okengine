#include "DenOfEvil.h"

#ifdef DENOFEVIL

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DOE::DOEGame game;

	LOG(INFO) << "Den Of Evil initiated...";
	game.Run();

	return 0;
}

#endif