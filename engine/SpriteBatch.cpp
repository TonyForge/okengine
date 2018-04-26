#include "SpriteBatch.h"

ok::Quad ok::graphics::SpriteBatch::quad;

ok::graphics::SpriteBatch::SpriteBatch(int size)
{
	batch_size = size;

	glBO = new GLBufferObject(size * 4);
	glBO->AddVertexBuffer(GL_DYNAMIC_DRAW, 2); //position
	glBO->AddVertexBuffer(GL_DYNAMIC_DRAW, 2); //texcoord
	glBO->AddVertexBuffer(GL_DYNAMIC_DRAW, 4); //color
	glBO->AddIndexBuffer(GL_STATIC_DRAW, size * 2);

	positions = glBO->GetVertexBuffer(0).vertices;
	texcoords = glBO->GetVertexBuffer(1).vertices;
	colors = glBO->GetVertexBuffer(2).vertices;
	indices = glBO->GetIndexBuffer(0).indices;

	int* ptr = nullptr;
	int index = 0;
	for (int i = 0; i < size; i++)
	{
		index = i * 4;
		ptr = &(indices[i * quad_index_data_size]);
		ptr[0] = index;
		ptr[1] = index + 1;
		ptr[2] = index + 2;
		ptr[3] = index;
		ptr[4] = index + 2;
		ptr[5] = index + 3;
	}
	
	glBO->ReloadIndexBuffer(0);
	
	default_material = ok::AssetsBasic::instance().GetMaterial("SpriteBatch.DefaultMaterial.xml");
	custom_material = nullptr;
	batch_texture = nullptr;
	batch_quads_in_use = 0;
}

ok::graphics::SpriteBatch::~SpriteBatch()
{
	delete glBO;
}

void ok::graphics::SpriteBatch::Draw(ok::graphics::TextureRect * tex_rect, glm::vec2 position, float rotation_degrees, glm::vec2 scale, bool flip_y, glm::vec2 hotspot)
{
	if (batch_texture != nullptr && batch_texture != tex_rect->texture)
	{
		BatchEnd();
	}

	batch_texture = tex_rect->texture;

	if (batch_quads_in_use == batch_size)
	{
		BatchEnd();
	}
	if (batch_quads_in_use == 0)
	{
		BatchBegin();
	}

	quad.SetCenter(hotspot);
	quad.SetSize(glm::vec2(tex_rect->width, tex_rect->height));

	if (true == flip_y)
		quad.SetUVRectFlipY(tex_rect->uv_rect);
	else
		quad.SetUVRect(tex_rect->uv_rect);

	quad.SetTransform(glm::vec3(position.x, position.y, 0.0f), rotation_degrees, glm::vec3(scale.x, scale.y, 1.0f));

	PushQuad();
}

void ok::graphics::SpriteBatch::Draw(ok::graphics::SpriteInfo * sprite_info, glm::vec2 position, float rotation_deg, glm::vec2 scale)
{

}

void ok::graphics::SpriteBatch::Draw(ok::graphics::Texture* tex, glm::vec2 position, glm::vec2 size, bool flip_y)
{
	if (batch_texture != nullptr && batch_texture != tex)
	{
		BatchEnd();
	}

	batch_texture = tex;

	if (batch_quads_in_use == batch_size)
	{
		BatchEnd();
	}
	if (batch_quads_in_use == 0)
	{
		BatchBegin();
	}

	quad.SetCenter(glm::vec2(0.5f,0.5f));
	quad.SetSize(size);

	if (true == flip_y)
		quad.SetUVRectFlipY(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	else
		quad.SetUVRect(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	quad.SetTransform(glm::vec3(position.x, position.y, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	PushQuad();
}

void ok::graphics::SpriteBatch::SetMaterial(ok::graphics::Material * material)
{
	custom_material = material;
}

void ok::graphics::SpriteBatch::SetMaterial(ok::String material_asset)
{
	SetMaterial(ok::AssetsBasic::instance().GetMaterial(material_asset));
}

void ok::graphics::SpriteBatch::BatchBegin(float default_z)
{
	if (batch_quads_in_use != 0)
	{
		BatchEnd();
	}

	_default_z = default_z;
}

void ok::graphics::SpriteBatch::BatchEnd()
{
	if (batch_quads_in_use == 0 || batch_texture == nullptr)
	{
		batch_texture = nullptr;
		batch_quads_in_use = 0;
		return;
	}

	ok::graphics::Material* current_material = default_material;

	if (custom_material != nullptr)
	{
		current_material = custom_material;
	}
		
	//setup shader
	current_material->SetTexture(0, batch_texture);
	current_material->Bind(this);

	glBO->ReloadVertexBuffer(0); //reload positions
	glBO->ReloadVertexBuffer(1); //reload texcoords
	glBO->ReloadVertexBuffer(2); //reload colors

	//bind buffers
	glBO->Bind();

	glDrawElements(GL_TRIANGLES, batch_quads_in_use * 6, GL_UNSIGNED_INT, 0);

	
	glBO->Unbind();

	batch_quads_in_use = 0;
	batch_texture = nullptr;
}

glm::mat4 ok::graphics::SpriteBatch::DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::ModelViewProjectionMatrix:
	{
		ok::graphics::Camera* camera = ok::graphics::Camera::GetCurrent();

		if (camera != nullptr)
			return camera->GetVPMatrix();
		else
			return glm::mat4(1.0f);
	}
	break;
	default:
	{
		return glm::mat4(1.0f);
	}
	break;
	}
}

float ok::graphics::SpriteBatch::DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if (*callback_name_ptr == "default_z")
			return _default_z;
		else return 0.0f;
	}
	break;
	default:
	{
		return 0.0f;
	}
	break;
	}
}

void ok::graphics::SpriteBatch::PushQuad()
{
	float* ptr = nullptr;
		
	ptr = &(positions[batch_quads_in_use * quad_position_data_size]);
	memcpy(ptr, quad.vertices, sizeof(float) * quad_position_data_size);

	ptr = &(texcoords[batch_quads_in_use * quad_texcoord_data_size]);
	memcpy(ptr, quad.uvs, sizeof(float) * quad_texcoord_data_size);

	ptr = &(colors[batch_quads_in_use * quad_color_data_size]);
	memcpy(ptr, quad.colors_stride, sizeof(float) * quad_color_data_size);

	batch_quads_in_use++;
}

void ok::graphics::SpriteAtlas::AddSprite(ok::graphics::SpriteInfo & sprite, ok::String & name)
{
	_items.push_back(sprite);
	_items_indexes[name] = _items.size() - 1;
}

ok::graphics::SpriteInfo & ok::graphics::SpriteAtlas::Get(ok::String & name)
{
	return _items[_items_indexes[name]];
}

ok::graphics::SpriteInfo * ok::graphics::SpriteAtlas::Search(ok::String & name)
{
	auto item = _items_indexes.find(name);

	if (item == _items_indexes.end())
	{
		//do nothing
	}
	else
	{
		return &_items[(*item).second];
	}
	
	return nullptr;
}

ok::graphics::SpriteInfo & ok::graphics::SpriteAtlas::Get(size_t index)
{
	return _items[index];
}

ok::graphics::SpriteInfo & ok::graphics::SpriteAtlas::Pick(float pick, bool reverse)
{
	return Pick(pick, -1, -1, ok::graphics::SpriteAtlasPickMode::Loop, reverse);
}

ok::graphics::SpriteInfo & ok::graphics::SpriteAtlas::Pick(float pick, ok::graphics::SpriteAtlasPickMode mode, bool reverse)
{
	return Pick(pick, -1, -1, mode, reverse);
}

ok::graphics::SpriteInfo & ok::graphics::SpriteAtlas::Pick(float pick, int first_index, int last_index, bool reverse)
{
	return Pick(pick, first_index, last_index, ok::graphics::SpriteAtlasPickMode::Loop, reverse);
}

ok::graphics::SpriteInfo & ok::graphics::SpriteAtlas::Pick(float pick, int first_index, int last_index, ok::graphics::SpriteAtlasPickMode mode, bool reverse)
{
	if (first_index == -1) first_index = 0;
	if (last_index == -1) last_index = _items.size() - 1;

	if (mode == ok::graphics::SpriteAtlasPickMode::Loop)
	{
		pick = glm::fract(pick);
	}
	else if (mode == ok::graphics::SpriteAtlasPickMode::Clamp)
	{
		pick = glm::clamp(pick, 0.f, 1.f);
	}
	else //if (mode == ok::graphics::SpriteAtlasPickMode::Pong)
	{
		float full_picks;
		pick = glm::modf(pick, full_picks);

		if (glm::mod(full_picks + 1.f, 2.f) > std::numeric_limits<float>::epsilon())
		{
			//do nothing
		}
		else
		{
			pick = 1.f - pick;
		}
	}

	if (reverse == true)
	{
		std::swap(first_index, last_index);
	}

	int frame = static_cast<int>(glm::floor(static_cast<float>(last_index - first_index) * glm::clamp(pick, 0.f, 1.f)));

	if (frame < 0)
	{
		frame = last_index + frame;
	}
	else
	{
		frame = first_index + frame;
	}

	return Get(static_cast<size_t>(frame));
}

int ok::graphics::SpriteAtlas::IndexOf(ok::String & name)
{
	auto item = _items_indexes.find(name);

	if (item == _items_indexes.end())
	{
		//do nothing
	}
	else
	{
		return (*item).second;
	}

	return -1;
}
