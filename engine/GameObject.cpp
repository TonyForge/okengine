#include "GameObject.h"

ok::GameObject::GameObject()
{
	ok::Behaviour_GameObject_Assignee(this, this);
}

void ok::GameObject::AddComponent(ok::Behaviour * component)
{
	_components.push_back(component);
	ok::Behaviour_GameObject_Assignee(this, component);
}

void ok::GameObject::RemoveComponent(ok::Behaviour * component)
{
	_components.erase(std::remove(_components.begin(), _components.end(), component), _components.end());
}

void ok::GameObject::Update(float dt)
{
	if (enabled)
	{
		for (auto&& _component : _components)
		{
			_component->Update(dt);
		}
	}
}

void ok::GameObject::MoveComponentUp(ok::Behaviour * component)
{
	int i = 0;

	for (auto&& _component : _components)
	{
		if ((i != 0) && (_component == component))
		{
			std::swap(_components[i], _components[i - 1]);
			break;
		}
		i++;
	}
}

void ok::GameObject::MoveComponentDown(ok::Behaviour * component)
{
	size_t i = 0;
	size_t i_max = (_components.size()) - 1;

	for (auto&& _component : _components)
	{
		if ((i != i_max) && (_component == component))
		{
			std::swap(_components[i], _components[i + 1]);
			break;
		}
		i++;
	}
}

ok::GameObject * ok::GameObject::Duplicate(ok::GameObject* _clone)
{
	if (_clone == nullptr)
	_clone = new ok::GameObject();

	_clone->name = name;
	_clone->name_hash = name_hash;
	_clone->keepWorldTransform = keepWorldTransform;

	for (auto&& _component : _components)
	{
		_clone->AddComponent(_component->Duplicate());
	}

	for (auto&& _children : _childrens)
	{
		_clone->AddChild(((ok::GameObject*)_children)->Duplicate());
	}
	
	for (auto&& _component : _clone->_components)
	{
		_component->OnGameObjectDuplicateEnd();
	}

	return _clone;
}
