#include "Material.h"

ok::graphics::Material::Material(ok::graphics::Shader* shader, int texture_slots_count)
{
	_shader = shader;

	_texture_slots.resize(texture_slots_count);

	int i = 0;
	for (auto&& slot : _texture_slots)
	{
		slot.first = i;
		slot.second = nullptr;
		i++;
	}
}

void ok::graphics::Material::SetTexture(int texture_slot_index, ok::graphics::Texture * texture)
{
	_texture_slots[texture_slot_index].second = texture;
}

void ok::graphics::Material::SetTexture(const std::string& sampler_name, ok::graphics::Texture* texture)
{
	int texture_channel_index;
	_shader->GetSampler(sampler_name, texture_channel_index);

	for (auto&& slot : _texture_slots)
	{
		if (slot.first == texture_channel_index)
		{
			slot.second = texture;
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
		if (slot.second == nullptr)
		_shader->BindTexture((unsigned int)0, slot.first);
		else
		_shader->BindTexture(slot.second->getNativeHandle(), slot.first);
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
