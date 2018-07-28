#include "GameObject.h"

ok::GameObject::GameObject()
{
	ok::Behaviour_GameObject_Assignee(this, this);
	ok::Transform_GameObject_Assignee(this, this);
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
		if (IsChildrenVisibilityOverriden() && (false == IsVisible())) ok::Behaviour::RenderingDisabled = true;

		std::list<ok::Transform*>::iterator it = _childrens.begin();
		std::list<ok::Transform*>::iterator it_end = _childrens.end();

		while (it != it_end)
		{
			ok::GameObject* child = static_cast<ok::GameObject*>(*it);
			child->Update(dt);
			it++;
		}

		for (auto&& _component : _components)
		{
			_component->Update(dt);
		}

		if (IsChildrenVisibilityOverriden() && (false == IsVisible())) ok::Behaviour::RenderingDisabled = false;
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
	_clone->keepWorldTransform = false;//keepWorldTransform;
	ok::Transform::CopyPaste(*this, *_clone, false);
	//_clone->transform

	for (auto&& _component : _components)
	{
		_clone->AddComponent(_component->Duplicate());
	}

	for (auto&& _children : _childrens)
	{
		ok::GameObject* dubl = ((ok::GameObject*)_children)->Duplicate();

		/*if (dubl->_relativeScale != _children->_relativeScale)
		{
			dubl = dubl;
		}

		glm::vec3 pre_scale_source = _children->_absoluteScale;
		glm::vec3 pre_scale_target = dubl->_absoluteScale;

		if (dubl->name == "EMT_ENGINE")
		{
			dubl = dubl;
		}*/

		//dubl->keepWorldTransform = false;

		_clone->AddChild(dubl);

		//glm::vec3 post_scale_target = dubl->_absoluteScale;

		/*if (pre_scale_source != post_scale_target)
		{
			dubl = dubl;
		}*/

		//_clone->AddChild(((ok::GameObject*)_children)->Duplicate());
	}
	
	for (auto&& _component : _clone->_components)
	{
		_component->OnGameObjectDuplicateEnd();
	}

	return _clone;
}

std::vector<ok::Behaviour*>& ok::GameObject::GetComponents()
{
	return _components;
}
