#pragma once

#include "IActor.h"


namespace ZonerX
{
	class IPlanet : public ZonerX::IActor
	{
	public:
		virtual void OnDayBegin() = 0;
		virtual void OnDayEnd() = 0;
	private:
	protected:
	};
}