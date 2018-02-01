#pragma once

#ifdef KRIPTA

#include "Application.h"
#include "projects\Kripta\Floor.h"

namespace Kripta
{
	class App : public ok::Application
	{
	public:
		App();
		void Init();
		void Update(float dt);
	protected:
	private:
	};
}


#endif