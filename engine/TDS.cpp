#include "TDS.h"

#ifdef TDS

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	NSTDS::TDSGame game;

	LOG(INFO) << "TDS initiated...";
	game.Run();

	return 0;
}

#endif