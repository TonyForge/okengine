#pragma once

#include "..\..\GameObject.h"

namespace Kripta
{
	enum class ObjectID
	{
		Unknown
	};

	enum class ObjActionID
	{
		Wait,
		Idle,
		Act,
		Move,
		Attack,
		FalseMove
	};

	class IObject : public ok::GameObject
	{
	public:
	private:
	protected:
	};
}