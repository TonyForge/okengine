#include "Behaviour.h"

bool ok::Behaviour::RenderingDisabled = false;


ok::Behaviour::Behaviour() : enabled(true), _gameObject(nullptr)
{

}

void ok::Behaviour::Update(float dt)
{
}

ok::Behaviour * ok::Behaviour::Duplicate(ok::Behaviour * _clone)
{
	_clone->enabled = enabled;
	return _clone;
}

void ok::Behaviour::OnGameObjectDuplicateEnd()
{
}

ok::Transform & ok::Behaviour::transform()
{
	return *((ok::Transform*)_gameObject);
}

ok::GameObject & ok::Behaviour::gameObject()
{
	return *_gameObject;
}

ok::Behaviour_GameObject_Assignee::Behaviour_GameObject_Assignee(ok::GameObject * _gameObject, ok::Behaviour * _behaviour)
{
	_behaviour->_gameObject = _gameObject;
}
