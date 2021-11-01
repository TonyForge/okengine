#pragma once

#include "..\..\Application.h"
#include "..\..\Assets.h"
#include "..\..\Camera.h"
#include "..\..\Random.h"
#include "..\..\Input.h"
#include "..\..\GameObject.h"
#include "..\..\TextBatch.h"

namespace NSTDS
{
	class Global
	{
	public:
		Global()
		{
		}
	};

	static NSTDS::Global global;

	class ITDSGame : public ok::Application
	{
	public:
		static NSTDS::ITDSGame* instance() { return _instance; }
	protected:
		static NSTDS::ITDSGame* _instance;
	private:
	};
}