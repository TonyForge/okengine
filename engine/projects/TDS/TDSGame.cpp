#include "TDSGame.h"

NSTDS::ITDSGame* NSTDS::ITDSGame::_instance = nullptr;

NSTDS::TDSGame::TDSGame()
{
	_settings_file_name = "tds.settings.xml";
	_instance = this;
}

void NSTDS::TDSGame::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);
}

void NSTDS::TDSGame::Update(float dt)
{
}