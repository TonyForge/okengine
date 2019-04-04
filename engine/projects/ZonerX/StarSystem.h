#pragma once

#include "Interfaces.h"

namespace ZonerX
{
	class StarSystemRenderer : public ok::Behaviour
	{
	public:
		virtual void Update(float dt);
		ZonerX::IStarSystem* star_system;
	};

	class StarSystem : public ZonerX::IStarSystem
	{
	public:
		StarSystem();
		virtual void OnDayBegin();
		virtual void OnDayEnd();
		virtual void Render(float dt);
	private:
	protected:
	};
}