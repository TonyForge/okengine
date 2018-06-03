#pragma once

#include "Behaviour.h"
#include "Transform.h"

namespace ok
{
	class GameObject : public ok::Transform, public ok::Behaviour
	{
	public:
		GameObject();

		using ok::Behaviour::gameObject;

		void AddComponent(ok::Behaviour* component);
		void RemoveComponent(ok::Behaviour* component);
		virtual void Update(float dt);
		void MoveComponentUp(ok::Behaviour* component);
		void MoveComponentDown(ok::Behaviour* component);

		virtual ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);
		//std::vector<std::function<void()>> OnDuplicateEnd;

		std::vector<ok::Behaviour*>& GetComponents();

		template<class T>
		std::enable_if_t<std::is_base_of<ok::Behaviour, T>::value, T*> GetComponent();
	protected:
	private:
		std::vector<ok::Behaviour*> _components;
	};

	template<class T>
	inline std::enable_if_t<std::is_base_of<ok::Behaviour, T>::value, T*> ok::GameObject::GetComponent()
	{
		for (auto&& _component : _components)
		{
			if (std::type_index(typeid(T)) == std::type_index(typeid(*_component)))
				return static_cast<T*>(_component);
		}

		return nullptr;
	}
}