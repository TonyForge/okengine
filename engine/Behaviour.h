#pragma once

#include "Transform.h"

namespace ok
{
	class GameObject;
	class Behaviour;
}

namespace ok
{
	class Behaviour_GameObject_Assignee
	{
	public:
		Behaviour_GameObject_Assignee(ok::GameObject * _gameObject, ok::Behaviour * _behaviour);
	};

	class Behaviour
	{
	public:
		bool enabled;
		Behaviour();

		virtual void Update(float dt);
		virtual ok::Behaviour* Duplicate(ok::Behaviour * _clone = nullptr);
		virtual void OnGameObjectDuplicateEnd();
		ok::Transform& transform();
		ok::GameObject& gameObject();

		static bool RenderingDisabled;
	protected:
	private:
		friend class ok::Behaviour_GameObject_Assignee;
		ok::GameObject* _gameObject;
	};
}