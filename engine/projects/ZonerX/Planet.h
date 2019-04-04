#pragma once

#include "Interfaces.h"

namespace ZonerX
{
	class PlanetRenderer : public ok::Behaviour
	{
	public:
		virtual void Update(float dt);
		float local_time = 0.f;//0..1
	};

	class Planet : public ZonerX::IPlanet
	{
	public:
		Planet();
		virtual void OnDayBegin();
		virtual void OnDayEnd();
	private:
	protected:
	};
}