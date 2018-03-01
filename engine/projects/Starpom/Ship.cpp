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

void Starpom::SS_ShipMaterial_Spaceship::Update(float dt)
{
	ok::graphics::Material* material = gameObject().GetComponent<ok::graphics::MeshRenderer>()->GetMaterial();
	ok::graphics::Shader* shader = material->GetShader();
	
	ok::graphics::Camera* camera = ok::graphics::Camera::GetCurrent();

	shader->SetUniform("_CameraUp", camera->GetUp());
	shader->SetUniform("_CameraRight", camera->GetRight());

	material->SetTexture("_Maps", _Maps);
	material->SetTexture("_Normals", _Normals);
	material->SetTexture("_Facture", _Facture);

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
	Starpom::SS_ShipMaterial_Spaceship* __clone = new Starpom::SS_ShipMaterial_Spaceship();

	//copy _clone to __clone
	*__clone = *static_cast<Starpom::SS_ShipMaterial_Spaceship*>(_clone);

	return __clone;
}
