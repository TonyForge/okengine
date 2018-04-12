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

	//copy this to __clone
	__clone->bounder_axis = bounder_axis;
	__clone->bounder_center = bounder_center;

	return __clone;
}

Zoner::Collision::Point Zoner::ShipBlueprint::Bound(glm::vec3 world_position)
{
	BeginTransform(ok::TransformSpace::WorldSpace);
	world_position = world_position - GetPosition();
	EndTransform(false);

	glm::vec3 point = glm::vec3(glm::inverse(GetAbsoluteTransformMatrix()) * glm::vec4(world_position, 0.f));
	point = (point - bounder_center) / bounder_axis;
	point *= point;

	float k = point.x + point.y + point.z;

	if (k <= 1.f)
	{
		//collision detected
		return Zoner::Collision::Point(point, true);
	}
	
	return Zoner::Collision::Point();
}

float Zoner::ShipBlueprint::GetBounderMaxRadiusInWorldSpace()
{
	BeginTransform(ok::TransformSpace::WorldSpace);
	glm::vec3 world_radius_axis = GetScale() * bounder_axis;
	EndTransform(false);
	
	return glm::max(glm::max(world_radius_axis.x, world_radius_axis.y), world_radius_axis.z);
}

void Zoner::ShipBlueprint::CalculateBounder()
{
	glm::vec3 min_axis(0.f,0.f,0.f);
	glm::vec3 max_axis(0.f, 0.f, 0.f);

	_CalculateBounder(this, min_axis, max_axis);

	bounder_center = (min_axis + max_axis) * 0.5f;
	bounder_axis = max_axis - bounder_center;
}

void Zoner::ShipBlueprint::_CalculateBounder(ok::Transform * part, glm::vec3& min_axis, glm::vec3& max_axis)
{
	ok::graphics::MeshRenderer* mr = part->gameObject().GetComponent<ok::graphics::MeshRenderer>();

	if (mr != nullptr)
	{
		GLBufferInfo buf_info = mr->GetMesh()->GetVertices();
		
		struct f3
		{
			float x, y, z;
		};

		f3* positions = (f3*)(buf_info.vertices);

		glm::vec4 part_axis4;
		glm::vec3 part_axis;

		part_axis4.w = 0.f;

		for (int i = 0; i < buf_info.vertex_count; i++)
		{
			part_axis4.x = positions->x;
			part_axis4.y = positions->y;
			part_axis4.z = positions->z;

			part_axis = glm::vec3(part->GetAbsoluteTransformMatrix() * part_axis4);

			min_axis = glm::min(min_axis, part_axis);
			max_axis = glm::max(max_axis, part_axis);

			positions++;
		}
	}
	else
	{
		glm::vec3 part_axis;

		part->BeginTransform(ok::TransformSpace::WorldSpace);
			part_axis = part->GetPosition();
		part->EndTransform(false);
		
		min_axis = glm::min(min_axis, part_axis);
		max_axis = glm::max(max_axis, part_axis);
	}

	std::list<ok::Transform*>& childrens = part->GetChildrens();
	for (auto& child : childrens)
	{
		_CalculateBounder(child, min_axis, max_axis);
	}
}

Zoner::Collision::Point Zoner::IShip::Pick(glm::vec3 world_position)
{
	if (this_blueprint != nullptr)
	{
		UpdateChildrensTransform();
		return this_blueprint->Bound(world_position);
	}

	return Zoner::Collision::Point();
}

Zoner::IShip::IShip() : 
	engine_speed(24.f),
	trajectory_progress(0.f)
{
	acquire_uid();
	cmd_sequence.sleep = false;
	cmd_sequence.sequence = true;
	cmd_parallel.sleep = false;
	cmd_parallel.parallel = true;
}
