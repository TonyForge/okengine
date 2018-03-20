#include "IShip.h"

void Zoner::ShipMaterial_SpacecraftFragment::Update(float dt)
{
}

void Zoner::ShipMaterial_SpacecraftFragment::OnMaterialBind(ok::graphics::Material & material)
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

ok::Behaviour * Zoner::ShipMaterial_SpacecraftFragment::Duplicate(ok::Behaviour * _clone)
{
	Zoner::ShipMaterial_SpacecraftFragment* __clone;

	if (_clone == nullptr)
	{
		__clone = new Zoner::ShipMaterial_SpacecraftFragment();
	}
	else
	{
		__clone = static_cast<Zoner::ShipMaterial_SpacecraftFragment*>(_clone);
	}

	//copy _clone to __clone
	*__clone = *static_cast<Zoner::ShipMaterial_SpacecraftFragment*>(this);

	return __clone;
}

ok::GameObject * Zoner::ShipBlueprint::Duplicate(ok::GameObject * _clone)
{
	Zoner::ShipBlueprint* __clone;

	if (_clone == nullptr)
	{
		__clone = new Zoner::ShipBlueprint();
	}
	else
	{
		__clone = static_cast<Zoner::ShipBlueprint*>(_clone);
	}

	ok::GameObject::Duplicate(__clone);

	//copy _clone to __clone

	return __clone;
}