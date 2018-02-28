#include "Ship.h"

std::unordered_map<std::string, Starpom::SS_ShipAgent*> Starpom::SS_ShipAgent::blueprints;

Starpom::Ship::Ship() : ss_agent(nullptr)
{
}

void Starpom::Ship::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (ss_agent != nullptr && Starpom::GameMode::GM_STAR_SYSTEM == Starpom::GameInfo::o().GetMode())
	{
		ss_agent->Update(dt);
	}
}

ok::GameObject * Starpom::Ship::Duplicate(ok::GameObject * _clone)
{
	Starpom::Ship* __clone = new Starpom::Ship();
	Starpom::Ship* __clone_source = static_cast<Starpom::Ship*>(_clone);

	ok::GameObject::Duplicate(__clone);

	//copy _clone to __clone
	
	__clone->ss_agent = static_cast<Starpom::SS_ShipAgent*>(__clone_source->ss_agent->Duplicate(__clone_source->ss_agent));

	return __clone;
}

glm::vec3 Starpom::Ship::GetPosition()
{
	if (ss_agent != nullptr && Starpom::GameMode::GM_STAR_SYSTEM == Starpom::GameInfo::o().GetMode())
	{
		return ss_agent->GetPosition();
	}

	return glm::vec3();
}

void Starpom::SS_ShipAgent::Update(float dt)
{
	ok::GameObject::Update(dt);
}

ok::GameObject * Starpom::SS_ShipAgent::Duplicate(ok::GameObject * _clone)
{
	Starpom::SS_ShipAgent* __clone = new Starpom::SS_ShipAgent();
	ok::GameObject::Duplicate(__clone);

	//copy _clone to __clone

	return __clone;
}
