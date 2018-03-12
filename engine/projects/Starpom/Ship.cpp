#include "Ship.h"

std::unordered_map<std::string, Starpom::SS_ShipAgent*> Starpom::SS_ShipAgent::blueprints;
unsigned int Starpom::Ship_ID::ship_global_id_next[Starpom::Ship_ID::ship_id_length] = { 0,0,0,0 };

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
	Starpom::Ship* __clone;

	if (_clone == nullptr)
	{
		__clone = new Starpom::Ship();
	}
	else
	{
		__clone = static_cast<Starpom::Ship*>(_clone);
	}

	ok::GameObject::Duplicate(__clone);

	//copy _clone to __clone
	
	ss_agent->Duplicate(__clone->ss_agent);

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
	Starpom::SS_ShipAgent* __clone;
	
	if (_clone == nullptr)
	{
		__clone = new Starpom::SS_ShipAgent();
	}
	else
	{
		__clone = static_cast<Starpom::SS_ShipAgent*>(_clone);
	}
		
	ok::GameObject::Duplicate(__clone);

	//copy _clone to __clone

	return __clone;
}

void Starpom::SS_ShipMaterial_Spaceship::Update(float dt)
{
}

void Starpom::SS_ShipMaterial_Spaceship::OnMaterialBind(ok::graphics::Material & material)
{
	ok::graphics::Shader* shader = material.GetShader();

	ok::graphics::Camera* camera = ok::graphics::Camera::GetCurrent();

	shader->SetUniform("_CameraUp", camera->GetUp());
	shader->SetUniform("_CameraRight", camera->GetRight());

	material.SetTexture("_Maps", _Maps);
	material.SetTexture("_Normals", _Normals);
	material.SetTexture("_Facture", _Facture);

	shader->SetUniform("_FactureTransform", _FactureTransform);
	shader->SetUniform("_ColorMain_0", _ColorMain_0);
	shader->SetUniform("_ColorMain_1", _ColorMain_1);
	shader->SetUniform("_ColorMain_DarkMultiplier", _ColorMain_DarkMultiplier);
	shader->SetUniform("_ColorSec_0", _ColorSec_0);
	shader->SetUniform("_ColorSec_1", _ColorSec_1);
	shader->SetUniform("_ColorSet3_0", _ColorSet3_0);
	shader->SetUniform("_ColorSet3_1", _ColorSet3_1);
}

ok::Behaviour * Starpom::SS_ShipMaterial_Spaceship::Duplicate(ok::Behaviour * _clone)
{
	Starpom::SS_ShipMaterial_Spaceship* __clone;
	
	if (_clone == nullptr)
	{
		__clone = new Starpom::SS_ShipMaterial_Spaceship();
	}
	else
	{
		__clone = static_cast<Starpom::SS_ShipMaterial_Spaceship*>(_clone);
	}
	

	//copy _clone to __clone
	*__clone = *static_cast<Starpom::SS_ShipMaterial_Spaceship*>(this);

	return __clone;
}

Starpom::SS_ShipAgent::Action_FlyInSS::Action_FlyInSS(Starpom::SS_ShipAgent * _actor) : actor(_actor)
{
	tick = 0;
	cost = 1000;
}

void Starpom::SS_ShipAgent::Action_FlyInSS::OnTickProgress()
{
	//Move ship here
	actor->BeginTransform();
	actor->SetPosition(path.GetWaypoint(total_progress).position);
	actor->EndTransform(true);
}

Starpom::Ship_ID::Ship_ID() : ship_id_type(Starpom::Ship_ID_Type::unknown)
{
	memcpy(ship_id, ship_global_id_next, sizeof(ship_id));

	ship_id_string.resize(sizeof(ship_id));
	memcpy(&ship_id_string[0], ship_id, sizeof(ship_id));

	int i = 0;
	
	while (true)
	{
		ship_global_id_next[i]++;
		if (ship_global_id_next[i] == UINT_MAX)
		{
			ship_global_id_next[i] = 0;
			i++;
			if (i == ship_id_length) break;
		}
		else
		{
			break;
		}
	}

}

const std::string & Starpom::Ship_ID::ShipIDToString()
{
	return ship_id_string;
}

bool Starpom::operator==(const Starpom::Ship_ID & left, const Starpom::Ship_ID & right)
{
	return memcmp(left.ship_id, right.ship_id, sizeof(unsigned int)*Starpom::Ship_ID::ship_id_length) == 0;
}

Starpom::Ship * Starpom::Ship::Universe::Find(Starpom::Ship_ID id)
{

	return nullptr;
}
