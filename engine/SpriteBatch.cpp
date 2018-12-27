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
		BatchBegin(_default_z);
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

void ok::graphics::SpriteBatch::Draw(ok::graphics::SpriteInfo * sprite_info, glm::vec2 position, float rotation_degrees, glm::vec2 scale)
{
	if (batch_texture != nullptr && batch_texture != sprite_info->rect.texture)
	{
		BatchEnd();
	}

	batch_texture = sprite_info->rect.texture;

	if (batch_quads_in_use == batch_size)
	{
		BatchEnd();
	}
	if (batch_quads_in_use == 0)
	{
		BatchBegin(_default_z);
	}

	if (sprite_info->tint_power > 0.f)
	{
		sprite_info->tint_color.a = sprite_info->tint_power;
		quad.SetColors(sprite_info->tint_color);
	}
	
	quad.SetCenter(sprite_info->hotspot);
	quad.SetSize(glm::vec2(sprite_info->rect.width, sprite_info->rect.height));

	if (true == sprite_info->flip_x && true == sprite_info->flip_y)
		quad.SetUVRectFlipXY(sprite_info->rect.uv_rect);
	else if (true == sprite_info->flip_x)
		quad.SetUVRectFlipX(sprite_info->rect.uv_rect);
	else if (true == sprite_info->flip_y)
		quad.SetUVRectFlipY(sprite_info->rect.uv_rect);
	else
		quad.SetUVRect(sprite_info->rect.uv_rect);

	quad.SetTransform(glm::vec3(position.x, position.y, 0.0f), rotation_degrees, glm::vec3(sprite_info->scale.x*scale.x, sprite_info->scale.y*scale.y, 1.0f));

	PushQuad(sprite_info->tint_power == 0.f);
}

void ok::graphics::SpriteBatch::Draw(ok::graphics::Texture* tex, glm::vec2 position, glm::vec2 size, bool flip_y, glm::vec2 hotspot)
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
		BatchBegin(_default_z);
	}

	quad.SetCenter(hotspot);
	quad.SetSize(size);

	if (true == flip_y)
		quad.SetUVRectFlipY(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	else
		quad.SetUVRect(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	quad.SetTransform(glm::vec3(position.x, position.y, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	PushQuad();
}

void ok::graphics::SpriteBatch::Draw(ok::graphics::Texture * tex, glm::mat3 & transform_matrix, bool flip_y, glm::vec2 hotspot)
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
		BatchBegin(_default_z);
	}

	quad.SetCenter(hotspot);
	quad.SetSize(tex->GetSize());

	if (true == flip_y)
		quad.SetUVRectFlipY(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	else
		quad.SetUVRect(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	quad.SetTransform(transform_matrix);

	PushQuad();
}

void ok::graphics::SpriteBatch::Draw(ok::graphics::SpriteInfo * sprite_info, glm::mat3 & transform_matrix)
{
	if (batch_texture != nullptr && batch_texture != sprite_info->rect.texture)
	{
		BatchEnd();
	}

	batch_texture = sprite_info->rect.texture;

	if (batch_quads_in_use == batch_size)
	{
		BatchEnd();
	}
	if (batch_quads_in_use == 0)
	{
		BatchBegin(_default_z);
	}

	if (sprite_info->tint_power > 0.f)
	{
		sprite_info->tint_color.a = sprite_info->tint_power;
		quad.SetColors(sprite_info->tint_color);
	}

	quad.SetCenter(sprite_info->hotspot);
	quad.SetSize(glm::vec2(sprite_info->rect.width*sprite_info->scale.x, sprite_info->rect.height*sprite_info->scale.y));

	if (true == sprite_info->flip_x && true == sprite_info->flip_y)
		quad.SetUVRectFlipXY(sprite_info->rect.uv_rect);
	else if (true == sprite_info->flip_x)
		quad.SetUVRectFlipX(sprite_info->rect.uv_rect);
	else if (true == sprite_info->flip_y)
		quad.SetUVRectFlipY(sprite_info->rect.uv_rect);
	else
		quad.SetUVRect(sprite_info->rect.uv_rect);

	quad.SetTransform(transform_matrix);

	PushQuad(sprite_info->tint_power == 0.f);
}

void ok::graphics::SpriteBatch::Blit(ok::graphics::Texture * tex, int x, int y, glm::vec2 hotspot, bool flip_y)
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
		BatchBegin(_default_z);
	}

	glm::ivec2& tex_size = tex->GetSize();
	int i_hotspot_x = static_cast<int>(glm::floor(hotspot.x * static_cast<float>(tex_size.x)));
	int i_hotspot_y = static_cast<int>(glm::floor(hotspot.y * static_cast<float>(tex_size.y)));
	quad.Place(x - i_hotspot_x, y - i_hotspot_y, tex_size.x, tex_size.y);

	if (true == flip_y)
		quad.SetUVRectFlipY(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	else
		quad.SetUVRect(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	PushQuad();
}

void ok::graphics::SpriteBatch::Blit(ok::graphics::SpriteInfo * sprite_info, int x, int y)
{
	if (batch_texture != nullptr && batch_texture != sprite_info->rect.texture)
	{
		BatchEnd();
	}

	batch_texture = sprite_info->rect.texture;

	if (batch_quads_in_use == batch_size)
	{
		BatchEnd();
	}
	if (batch_quads_in_use == 0)
	{
		BatchBegin(_default_z);
	}

	if (sprite_info->tint_power > 0.f)
	{
		sprite_info->tint_color.a = sprite_info->tint_power;
		quad.SetColors(sprite_info->tint_color);
	}

	glm::ivec2& tex_size = glm::ivec2(sprite_info->rect.width, sprite_info->rect.height);
	int i_hotspot_x = static_cast<int>(glm::floor(sprite_info->hotspot.x * static_cast<float>(tex_size.x)));
	int i_hotspot_y = static_cast<int>(glm::floor(sprite_info->hotspot.y * static_cast<float>(tex_size.y)));
	quad.Place(x - i_hotspot_x, y - i_hotspot_y, tex_size.x, tex_size.y);


	if (true == sprite_info->flip_x && true == sprite_info->flip_y)
		quad.SetUVRectFlipXY(sprite_info->rect.uv_rect);
	else if (true == sprite_info->flip_x)
		quad.SetUVRectFlipX(sprite_info->rect.uv_rect);
	else if (true == sprite_info->flip_y)
		quad.SetUVRectFlipY(sprite_info->rect.uv_rect);
	else
		quad.SetUVRect(sprite_info->rect.uv_rect);

	PushQuad(sprite_info->tint_power == 0.f);
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
	}

	unresolved_alias = true;
	return glm::mat4(1.0f);
}

float ok::graphics::SpriteBatch::DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if (*callback_name_ptr == "default_z")
			return _default_z;
	}
	break;
	}

	unresolved_alias = true;
	return 0.0f;
}

void ok::graphics::SpriteBatch::PushQuad(bool zero_color_data)
{
	float* ptr = nullptr;
		
	ptr = &(positions[batch_quads_in_use * quad_position_data_size]);
	memcpy(ptr, quad.vertices, sizeof(float) * quad_position_data_size);

	ptr = &(texcoords[batch_quads_in_use * quad_texcoord_data_size]);
	memcpy(ptr, quad.uvs, sizeof(float) * quad_texcoord_data_size);

	if (zero_color_data == true)
	{
		ptr = &(colors[batch_quads_in_use * quad_color_data_size]);
		memset(ptr, 0, sizeof(float) * quad_color_data_size);
	}
	else
	{
		ptr = &(colors[batch_quads_in_use * quad_color_data_size]);
		memcpy(ptr, quad.colors_stride, sizeof(float) * quad_color_data_size);
	}


	batch_quads_in_use++;
}

void ok::graphics::SpriteAtlas::AddSprite(ok::graphics::SpriteInfo & sprite, ok::String & name)
{
	_items.push_back(sprite);
	_items_indexes[name] = _items.size() - 1;
}

void ok::graphics::SpriteAtlas::BeginSequence()
{
	_temp_sequence.begin = _items.size() - 1;
}

void ok::graphics::SpriteAtlas::EndSequence(ok::String & name)
{
	_temp_sequence.end = _items.size() - 1;

	_sequences[name] = _temp_sequence;
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

ok::graphics::SpriteInfo & ok::graphics::SpriteAtlas::PickSequence(ok::String & name, float pick, bool reverse)
{
	ok::graphics::SpriteAtlas::SpriteAtlasSequence& sequence = _sequences[name];

	return Pick(pick, sequence.begin, sequence.end, ok::graphics::SpriteAtlasPickMode::Loop, reverse);
}

ok::graphics::SpriteInfo & ok::graphics::SpriteAtlas::PickSequence(ok::String & name, float pick, ok::graphics::SpriteAtlasPickMode mode, bool reverse)
{
	ok::graphics::SpriteAtlas::SpriteAtlasSequence& sequence = _sequences[name];

	return Pick(pick, sequence.begin, sequence.end, mode, reverse);
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

ok::graphics::SpriteInfo::SpriteInfo() : 
	hotspot(0.5f,0.5f),
	scale(1.f,1.f),
	tint_color(0.f,0.f,0.f,0.f),
	tint_power(0.f),
	flip_x(false),
	flip_y(false)
{
}

ok::graphics::TextureRect::TextureRect() :
	texture(nullptr),
	left(0), top(0), width(0), height(0),
	uv_rect(0.f)
{
}

ok::graphics::TextureRect::TextureRect(ok::graphics::Texture * _texture, int _left, int _top, int _width, int _height) :
	texture(_texture),
	left(_left), top(_top), width(_width), height(_height)
{
	glm::ivec2 size = texture->GetSize();

	uv_rect.x = static_cast<float>(left) / static_cast<float>(size.x);
	uv_rect.y = static_cast<float>(top) / static_cast<float>(size.y);
	uv_rect.z = static_cast<float>(width) / static_cast<float>(size.x);
	uv_rect.w = static_cast<float>(height) / static_cast<float>(size.y);
}

ok::graphics::TextureRect::TextureRect(ok::graphics::Texture * _texture) :
	texture(_texture)
{
	glm::ivec2 size = texture->GetSize();

	uv_rect = glm::vec4(0.f, 0.f, 1.f, 1.f);
	left = 0; top = 0;
	width = size.x; height = size.y;
}
