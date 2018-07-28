#include "MeshRenderer.h"

ok::graphics::MeshRenderer::MeshRenderer() : _renderer_request(this)
{
	OnMaterialBind = nullptr;
}

ok::graphics::Mesh * ok::graphics::MeshRenderer::GetMesh()
{
	return _mesh;
}

void ok::graphics::MeshRenderer::SetMesh(ok::graphics::Mesh * mesh)
{
	_mesh = mesh;
}

void ok::graphics::MeshRenderer::SetMesh(ok::String mesh_asset, bool prefer_hardware_skin)
{
	SetMesh(ok::AssetsBasic::instance().GetMesh(mesh_asset, prefer_hardware_skin));
}

void ok::graphics::MeshRenderer::SetMaterial(ok::graphics::Material * material)
{
	_material = material;

	if (_material->GetTransparent())
	{
		_mesh->EnableTrianglesSort();
		_mesh->SortTriangles();
		_mesh->UpdateIndices();
	}
}

void ok::graphics::MeshRenderer::SetMaterial(ok::String material_asset)
{
	SetMaterial(ok::AssetsBasic::instance().GetMaterial(material_asset));
}

ok::graphics::Material * ok::graphics::MeshRenderer::GetMaterial()
{
	return _material;
}

void ok::graphics::MeshRenderer::Update(float dt)
{
	if (RenderingDisabled == true) return;

	if (gameObject().IsVisible())
	ok::graphics::LayeredRenderer::instance().Render(_renderer_request);
}

void ok::graphics::MeshRenderer::Render()
{
	if (_material->GetTransparent())
	{
		//select right sorted triangles group
		glm::vec3 camera_direction = ok::graphics::Camera::GetCurrent()->GetForward();

		std::vector<float> axis_weights;
		axis_weights.reserve(6);

		axis_weights.push_back(glm::dot(transform().GetRight(), camera_direction));
		axis_weights.push_back(glm::dot(transform().GetUp(), camera_direction));
		axis_weights.push_back(glm::dot(transform().GetForward(), camera_direction));
		axis_weights.push_back(-axis_weights[0]);
		axis_weights.push_back(-axis_weights[1]);
		axis_weights.push_back(-axis_weights[2]);

		_mesh->SwitchIndices(static_cast<int>(std::min_element(axis_weights.begin(), axis_weights.end()) - axis_weights.begin())/*std::distance(axis_weights.begin(), std::min_element(axis_weights.begin(), axis_weights.end()))*/);
	}

	_material->Bind(this);
	if (OnMaterialBind != nullptr) OnMaterialBind(*_material);
	_mesh->Bind();

	glDrawElements(GL_TRIANGLES, _mesh->GetTrianglesCount()*3, GL_UNSIGNED_INT, 0);

	_mesh->Unbind();
	//_material->Unbind();
}

void ok::graphics::MeshRenderer::SetStage(ok::graphics::LayeredRendererStage stage)
{
	_renderer_request.stage = stage;
}

ok::graphics::LayeredRendererStage ok::graphics::MeshRenderer::GetStage()
{
	return _renderer_request.stage;
}

void ok::graphics::MeshRenderer::SetLayer(int layer)
{
	_renderer_request.layer = layer;
}

int ok::graphics::MeshRenderer::GetLayer()
{
	return _renderer_request.layer;
}

ok::Behaviour * ok::graphics::MeshRenderer::Duplicate(ok::Behaviour * _clone)
{
	ok::graphics::MeshRenderer* __clone = new ok::graphics::MeshRenderer();
	ok::Behaviour::Duplicate(__clone);
	
	__clone->callback_name_ptr = callback_name_ptr;
	__clone->_material = _material;
	__clone->_mesh = _mesh;

	__clone->_renderer_request.layer = _renderer_request.layer;
	__clone->_renderer_request.stage = _renderer_request.stage;
	__clone->_renderer_request.transparent = _renderer_request.transparent;
	__clone->_renderer_request.world_space_position = _renderer_request.world_space_position;

	__clone->OnMaterialBind = OnMaterialBind;

	return __clone;
}

glm::mat4 ok::graphics::MeshRenderer::DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::ModelViewProjectionMatrix :
	{
		return ok::graphics::Camera::GetCurrent()->GetVPMatrix() * transform().GetAbsoluteTransformMatrix();
	}
	break;
	case ok::graphics::ShaderAliasReference::ViewProjectionMatrix:
	{
		return ok::graphics::Camera::GetCurrent()->GetVPMatrix();
	}
	break;
	case ok::graphics::ShaderAliasReference::ModelMatrix:
	{
		return transform().GetAbsoluteTransformMatrix();
	}
	break;
	}

	unresolved_alias = true;
	return glm::mat4(1.f);
}

glm::vec2 ok::graphics::MeshRenderer::DispatchAliasVec2(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return glm::vec2();
}

float ok::graphics::MeshRenderer::DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if ((*callback_name_ptr).find("morph_blend_") != std::string::npos)
		{
			return _mesh->GetMorphBlends(static_cast<int>((*callback_name_ptr).at(12) - '0'));
		}
	}
	break;
	}

	unresolved_alias = true;
	return 0.f;
}

std::pair<glm::mat4*, int> ok::graphics::MeshRenderer::DispatchAliasMat4Array(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
		case ok::graphics::ShaderAliasReference::Bones:
		{
			std::vector<glm::mat4>& _bones = _mesh->GetSkinBones();
			return std::pair<glm::mat4*, int>(_bones.data(), static_cast<int>(_bones.size()));
		}
		break;
	}

	unresolved_alias = true;
	return std::pair<glm::mat4*, int>(nullptr, 0);
}
