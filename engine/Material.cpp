#include "Material.h"

ok::graphics::Material::Material(ok::graphics::Shader* shader, int texture_slots_count)
{
	_shader = shader;

	_texture_slots.resize(texture_slots_count);

	int i = 0;
	for (auto&& slot : _texture_slots)
	{
		slot.first = i;
		slot.second.texture = nullptr;
		slot.second.smooth = true;
		slot.second.wrapping = GL_REPEAT;
		slot.second.background_color = ok::Color(0.f,0.f,0.f,1.f);
		i++;
	}
}

void ok::graphics::Material::SetTexture(int texture_slot_index, ok::graphics::Texture * texture)
{
	auto& _slot_data = _texture_slots[texture_slot_index].second;

	if (_slot_data.fixed_asset == false)
	_texture_slots[texture_slot_index].second.texture = texture;
}

void ok::graphics::Material::SetTexture(const std::string& sampler_name, ok::graphics::Texture* texture)
{
	int texture_channel_index;
	_shader->GetSampler(sampler_name, texture_channel_index);

	for (auto&& slot : _texture_slots)
	{
		if (slot.first == texture_channel_index && slot.second.fixed_asset == false)
		{
			slot.second.texture = texture;
		}
	}
}

/*void ok::graphics::Material::SetTexture(ok::String sampler_name, ok::String texture_asset)
{
	_textures[_shader->GetSampler(sampler_name)] = ok::Assets::instance().GetTexture(texture_asset);
}*/

void ok::graphics::Material::Update(float dt)
{
}

void ok::graphics::Material::Bind(ok::graphics::ShaderAliasDispatcher* dispatcher)
{
	_shader->Bind(dispatcher);

	for (auto&& slot : _texture_slots)
	{
		if (slot.second.texture == nullptr)
		ok::graphics::Texture::BindTexture(nullptr, slot.first);
		else
		{
			ok::graphics::Texture::BindTexture(slot.second.texture, slot.first);
			slot.second.texture->SetProperties((_texture_linear_filter_allowed) ? slot.second.smooth : false, slot.second.wrapping, slot.second.background_color);
		}
		
	}
}

bool ok::graphics::Material::GetTransparent()
{
	return _shader->options.isTransparentEnabled;
}

void ok::graphics::Material::LinkSlotToSampler(const std::string & sampler_name, int texture_slot_index)
{
	int texture_channel_index;
	_shader->GetSampler(sampler_name, texture_channel_index);

	_texture_slots[texture_slot_index].first = texture_channel_index;
}

void ok::graphics::Material::SetSlotProperties(int texture_slot_index, bool smooth, GLenum wrapping, ok::Color background_color, bool fixed_asset)
{
	ok::graphics::MaterialTextureSlot& slot = _texture_slots[texture_slot_index].second;

	slot.smooth = smooth;
	slot.wrapping = wrapping;
	slot.background_color = background_color;
	slot.fixed_asset = fixed_asset;
}

void ok::graphics::Material::BindSubroutines(ok::graphics::ShaderAliasDispatcher * dispatcher)
{
	_shader->BindSubroutines(dispatcher);
}

unsigned int ok::graphics::Material::GetSubroutineIndex(GLenum shadertype, const std::string & name)
{
	return _shader->GetSubroutine(shadertype, _shader->GetSubroutineDirectAccessIndex(shadertype, name));
}

ok::graphics::Shader * ok::graphics::Material::GetShader()
{
	return _shader;
}

void ok::graphics::Material::AllowTextureLinearFilter()
{
	_texture_linear_filter_allowed = true;
}

void ok::graphics::Material::ForbidTextureLinearFilter()
{
	_texture_linear_filter_allowed = false;
}
