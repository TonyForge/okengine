#pragma once

#include "..\..\GameObject.h"

namespace ZonerX
{
	enum class ActorFamilyID
	{
		Any,
		Ship,
		Station,
		Planet,
		JumpGate,
		JumpHole,
		Meteorite,
		Lootbox,
		Star,
		Platform
	};

	class IActor
	{
	public:
		virtual void OnDayBegin() = 0;
		virtual void OnDayEnd() = 0;

		ZonerX::ActorFamilyID family_id;

		ok::GameObject render_object_node;
		ok::GameObject render_ui_node;

		float x, y, radius;
	protected:
	private:
	};
}