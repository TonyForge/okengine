#include "TextBatch.h"

ok::Quad ok::graphics::TextBatch2D::quad;

ok::graphics::TextBatch2D::TextBatch2D(int screen_width, int screen_height, int size)
{
	batch_size = size;

	glBO = new GLBufferObject(size * 4);
	glBO->AddVertexBuffer(GL_DYNAMIC_DRAW, 4); //position vec2 + texcoord vec2 = vec4
	glBO->AddIndexBuffer(GL_STATIC_DRAW, size * 2);

	positions_and_texcoords = &(glBO->GetVertexBuffer(0).vertices[0]);
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
	
	_mat = ok::AssetsBasic::instance().GetMaterial("TextBatch2D.DefaultMaterial.xml");

	_filters_indexes.Undefined = _mat->GetSubroutineIndex(GL_FRAGMENT_SHADER, "FilterUndefined");
	_filters_indexes.Default = _mat->GetSubroutineIndex(GL_FRAGMENT_SHADER, "FilterDefault");
	_filters_indexes.Gradient = _mat->GetSubroutineIndex(GL_FRAGMENT_SHADER, "FilterGradient");
	_filters_indexes.InnerGlow = _mat->GetSubroutineIndex(GL_FRAGMENT_SHADER, "FilterInnerGlow");
	_filters_indexes.InnerShadow = _mat->GetSubroutineIndex(GL_FRAGMENT_SHADER, "FilterInnerShadow");
	_filters_indexes.OuterGlow = _mat->GetSubroutineIndex(GL_FRAGMENT_SHADER, "FilterOuterGlow");
	_filters_indexes.OuterShadow = _mat->GetSubroutineIndex(GL_FRAGMENT_SHADER, "FilterOuterShadow");

	_tex = nullptr;
	_font = nullptr;
	batch_quads_in_use = 0;

	_camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::Screen);
	_camera->SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), -1, 1);

	_camera->BeginTransform();
	_camera->SetPosition(glm::vec3(0.f, 0.f, -1.f));
	_camera->SetForward(glm::vec3(0.f, 0.f, 1.f));
	_camera->EndTransform();

	_brush_align_horizontal = ok::graphics::TextAlign::Left;
	_brush_align_vertical = ok::graphics::TextAlign::Top;

	_early_batch_flush = false;
	_custom_brush_size_proportions_enabled = false;
	_line_spacing_scale = 1.f;
	_clip_rect_enabled = false;
}

ok::graphics::TextBatch2D::~TextBatch2D()
{
	delete glBO;
}

void ok::graphics::TextBatch2D::BatchBegin()
{
	if (batch_quads_in_use != 0 || _font == nullptr)
	{
		BatchEnd();
	}

	/*if (!_early_batch_flush)
	{
		_batch_total_rect.SetXYWH(0, 0, 0, 0);
	}*/
	
}

void ok::graphics::TextBatch2D::BatchEnd()
{
	if (batch_quads_in_use == 0 || _font == nullptr)
	{
		batch_quads_in_use = 0;
		return;
	}

	ok::graphics::Camera::PushCamera(_camera);
	//setup shader
	_mat->SetTexture(0, _tex);
	_mat->Bind(this);
	//pass uniforms to shader here...

	glBO->ReloadVertexBuffer(0); //reload positions and texcoords

	//bind buffers
	glBO->Bind();

	if (_clip_rect_enabled)
	{
		_camera->BeginScissorTest(static_cast<int>(_clip_rect.GetX()), static_cast<int>(_clip_rect.GetY()), static_cast<int>(_clip_rect.GetWidth()), static_cast<int>(_clip_rect.GetHeight()));
	}

	
	//reset filters
	for (int i = 0; i < ok::graphics::TextBatch2D_ShaderFiltersIndexes::FiltersLimit; i++)
	{
		_filters_enabled[i] = _filters_indexes.Undefined;
	}

	//outer shadow filter
	if (_brush_outer_shadow_color.a != 0.f)
	{
		_filters_enabled[0] = _filters_indexes.OuterShadow;
		_mat->BindSubroutines(this);
		glDrawElements(GL_TRIANGLES, batch_quads_in_use * 6, GL_UNSIGNED_INT, 0);
	}

	//outer glow filter
	if (_brush_outer_glow_color.a != 0.f)
	{
		_filters_enabled[0] = _filters_indexes.OuterGlow;
		_mat->BindSubroutines(this);
		glDrawElements(GL_TRIANGLES, batch_quads_in_use * 6, GL_UNSIGNED_INT, 0);
	}

	//color, bold, gradient, inner shadow, inner glow filters
	_filters_enabled[0] = _filters_indexes.Default;
	if (_brush_gradient_colors[0].a != 0.f && _brush_gradient_colors[1].a != 0.f) _filters_enabled[1] = _filters_indexes.Gradient;
	if (_brush_inner_shadow_color.a != 0.f) _filters_enabled[2] = _filters_indexes.InnerShadow;
	if (_brush_inner_glow_color.a != 0.f) _filters_enabled[3] = _filters_indexes.InnerGlow;
	_mat->BindSubroutines(this);
	glDrawElements(GL_TRIANGLES, batch_quads_in_use * 6, GL_UNSIGNED_INT, 0);


	if (_clip_rect_enabled)
	{
		_camera->EndScissorTest();
	}

	glBO->Unbind();

	batch_quads_in_use = 0;
	ok::graphics::Camera::PopCamera();
}

void ok::graphics::TextBatch2D::SetBrushFont(ok::graphics::Font * font)
{
	_font = font;
	_tex = _font->GetTexture();

	ok::Color clr;
	float values[4];

	if (_font->IsBrushSizeOverrideEnabled())
	{
		if (_font->IsBrushSizeOverrideCustomProportionsEnabled())
		{
			_font->GetBrushSizeOverride(values[0], values[1]);
			SetBrushSize(values[0], values[1]);
		}
		else
		{
			_font->GetBrushSizeOverride(values[0]);
			SetBrushSize(values[0]);
		}
	}

	_font->GetBrushColor(clr);
	SetBrushColor(clr);

	_font->GetBrushBold(values[0]);
	SetBrushBold(values[0]);

	_font->GetBrushItalic(values[0]);
	SetBrushItalic(values[0]);

	_font->GetBrushInnerGlow(clr, values[0], values[1]);
	SetBrushInnerGlow(clr, values[0], values[1]);

	_font->GetBrushOuterGlow(clr, values[0], values[1]);
	SetBrushOuterGlow(clr, values[0], values[1]);

	_font->GetBrushInnerShadow(clr, values[0], values[1], values[2], values[3]);
	SetBrushInnerShadow(clr, values[0], values[1], values[2], values[3]);

	_font->GetBrushOuterShadow(clr, values[0], values[1], values[2], values[3]);
	SetBrushOuterShadow(clr, values[0], values[1], values[2], values[3]);
}

void ok::graphics::TextBatch2D::SetBrushColor(ok::Color color)
{
	_brush_color = color;
}

void ok::graphics::TextBatch2D::SetBrushBold(float power)
{
	_brush_bold_power = power;
}

void ok::graphics::TextBatch2D::SetBrushItalic(float power)
{
	float rescale_x = 1.0f;

	if (_custom_brush_size_proportions_enabled)
	{
		rescale_x = _brush_px_size.x / _font->GetGlyphMaxWidth();
	}
	else
	{
		rescale_x = _brush_px_size.y / _font->GetGlyphMaxHeight();
	}

	_brush_italic_shift = _font->GetGlyphMaxWidth() * rescale_x * power;
}

void ok::graphics::TextBatch2D::SetBrushOuterGlow(ok::Color color, float radius, float blur)
{
	_brush_outer_glow_color = color;

	radius = 0.45f*(1.0f - _brush_bold_power) + (1.0f - 0.45f*(1.0f - _brush_bold_power))*radius;

	_brush_outer_glow_min_range = 1.0f - glm::clamp(radius, 0.f, 1.f);
	_brush_outer_glow_max_range = glm::clamp(_brush_outer_glow_min_range + blur, 0.f, 1.f);
}

void ok::graphics::TextBatch2D::SetBrushInnerGlow(ok::Color color, float radius, float blur)
{
	_brush_inner_glow_color = color;

	radius = 0.45f*(1.0f - _brush_bold_power) + (1.0f - 0.45f*(1.0f - _brush_bold_power))*radius;
	blur = (1.0f - 0.45f*(1.0f - _brush_bold_power)) * blur;

	_brush_inner_glow_min_range = glm::clamp(radius, 0.f, 1.f);
	_brush_inner_glow_max_range = glm::clamp(_brush_inner_glow_min_range + glm::clamp(blur, 0.f, 1.f),0.f,1.f);
}

void ok::graphics::TextBatch2D::SetBrushOuterShadow(ok::Color color, float radius, float blur, float shift_x_power, float shift_y_power)
{
	_brush_outer_shadow_color = color;

	radius = 0.45f*(1.0f - _brush_bold_power) + (1.0f - 0.45f*(1.0f - _brush_bold_power))*radius;

	_brush_outer_shadow_min_range = 1.0f - glm::clamp(radius, 0.f, 1.f);
	_brush_outer_shadow_max_range = glm::clamp(_brush_outer_shadow_min_range + blur, 0.f, 1.f);

	_brush_outer_shadow_shift_x_power = -shift_x_power * (_font->GetGlyphMaxWidth() / static_cast<float>(_tex->GetSize().x));
	_brush_outer_shadow_shift_y_power = -shift_y_power * (_font->GetGlyphMaxHeight() / static_cast<float>(_tex->GetSize().y));
}

void ok::graphics::TextBatch2D::SetBrushInnerShadow(ok::Color color, float radius, float blur, float shift_x_power, float shift_y_power)
{
	_brush_inner_shadow_color = color;

	radius = 0.45f*(1.0f - _brush_bold_power) + (1.0f - 0.45f*(1.0f - _brush_bold_power))*radius;
	blur = (1.0f - 0.45f*(1.0f - _brush_bold_power)) * blur;

	_brush_inner_shadow_min_range = glm::clamp(radius, 0.f, 1.f);
	_brush_inner_shadow_max_range = glm::clamp(_brush_inner_shadow_min_range + glm::clamp(blur, 0.f, 1.f), 0.f, 1.f);

	_brush_inner_shadow_shift_x_power = -shift_x_power * (_font->GetGlyphMaxWidth() / static_cast<float>(_tex->GetSize().x));
	_brush_inner_shadow_shift_y_power = -shift_y_power * (_font->GetGlyphMaxHeight() / static_cast<float>(_tex->GetSize().y));
}

void ok::graphics::TextBatch2D::SetBrushGradient(ok::Color color_up, ok::Color color_down)
{
	_brush_gradient_colors[0] = color_up;
	_brush_gradient_colors[1] = color_down;
}

void ok::graphics::TextBatch2D::SetBrushAlignVertical(ok::graphics::TextAlign align)
{
	_brush_align_vertical = align;
}

void ok::graphics::TextBatch2D::SetBrushAlignHorizontal(ok::graphics::TextAlign align)
{
	_brush_align_horizontal = align;
}

void ok::graphics::TextBatch2D::SetBrushPosition(float x, float y)
{
	_brush_position.x = x;
	_brush_position.y = y;
}

void ok::graphics::TextBatch2D::SetBrushPosition(glm::vec2 pos)
{
	_brush_position = pos;
}

void ok::graphics::TextBatch2D::GetBrushPosition(float & out_x, float & out_y)
{
	out_x = _brush_position.x;
	out_y = _brush_position.y;
}

void ok::graphics::TextBatch2D::GetBrushPosition(glm::vec2 & out_pos)
{
	out_pos = _brush_position;
}

glm::vec2 ok::graphics::TextBatch2D::GetBrushPosition()
{
	return _brush_position;
}

void ok::graphics::TextBatch2D::SetBrushSize(float px_width, float px_height)
{
	_custom_brush_size_proportions_enabled = true;
	_brush_px_size.x = px_width;
	_brush_px_size.y = px_height;
}

void ok::graphics::TextBatch2D::SetBrushSize(float px_height)
{
	_custom_brush_size_proportions_enabled = false;
	_brush_px_size.y = px_height;
}

void ok::graphics::TextBatch2D::SetLineSpacingScale(float scale)
{
	_line_spacing_scale = scale;
}

void ok::graphics::TextBatch2D::SetBrushScale(float scale_x, float scale_y)
{
	_brush_scale.x = scale_x;
	_brush_scale.y = scale_y;
}

void ok::graphics::TextBatch2D::SetBrushScale(glm::vec2 scale)
{
	_brush_scale = scale;
}

void ok::graphics::TextBatch2D::GetBrushScale(float & out_scale_x, float & out_scale_y)
{
	out_scale_x = _brush_scale.x;
	out_scale_y = _brush_scale.y;
}

void ok::graphics::TextBatch2D::GetBrushScale(glm::vec2 & out_scale)
{
	out_scale = _brush_scale;
}

glm::vec2 ok::graphics::TextBatch2D::GetBrushScale()
{
	return _brush_scale;
}

void ok::graphics::TextBatch2D::SetClipRectEnabled(bool enabled)
{
	_clip_rect_enabled = enabled;
}

void ok::graphics::TextBatch2D::SetClipRect(ok::Rect2Df rect)
{
	_clip_rect = rect;
}

void ok::graphics::TextBatch2D::Draw(ok::String & text, int from, int to)
{
	if (text.isEmpty()) return;

	if (to < 0) to = text.getSize() - 1;

	const unsigned int* characters = &(text.getData()[from]);
	int characters_count = (to - from)+1;
	float _initial_brush_position_x = _brush_position.x;

	unsigned int charcode;

	// collect all new lines
	bool _no_lf_in_last_line = false;
	int _line_begin = 0;
	int _line_end = 0;

	_newline_ranges.resize(0);

	int _last_cr_i = -1;
	int _last_lf_i = -2;

	for (int i = 0; i < characters_count; i++)
	{
		charcode = characters[i];

		if (charcode == 13) _last_cr_i = i;
		if (charcode == 10) _last_lf_i = i;

		if (_last_lf_i == i)
		{
			//new line
			_line_end = i - 1;
			if (_last_cr_i == (_last_lf_i - 1)) _line_end = i - 2;

			if (_line_end >= _line_begin)
			{
				_newline_ranges.push_back(std::pair<int, int>(from+_line_begin, from+_line_end));
			}

			_line_begin = i + 1;
		}
	}

	if (_last_lf_i != (characters_count - 1))
	{
		_no_lf_in_last_line = true;
		_newline_ranges.push_back(std::pair<int, int>(from + _line_begin, from + (characters_count - 1)));
	}


	// render lines
	float rescale_x = 1.0f;
	float rescale_y = 1.0f;

	if (_custom_brush_size_proportions_enabled)
	{
		rescale_x = _brush_px_size.x / _font->GetGlyphMaxWidth();
		rescale_y = _brush_px_size.y / _font->GetGlyphMaxHeight();
	}
	else
	{
		rescale_y = _brush_px_size.y / _font->GetGlyphMaxHeight();
		rescale_x = rescale_y;
	}

	
	int lines_counter = 0;

	for (auto&& line : _newline_ranges)
	{
		from = line.first;
		to = line.second;

		characters = &(text.getData()[from]);
		characters_count = (to - from) + 1;

		//calculate text align

		if (_brush_align_vertical == ok::graphics::TextAlign::Top)
		{
			_brush_position.y += _font->GetGlyphMaxHeight() * rescale_y;
		}
		else if (_brush_align_vertical == ok::graphics::TextAlign::Center)
		{
			_brush_position.y += _font->GetGlyphMaxHeight() * rescale_y * 0.5f;
		}
		else if (_brush_align_vertical == ok::graphics::TextAlign::Bottom)
		{
			//do nothing
		}

		if (_brush_align_horizontal == ok::graphics::TextAlign::Left)
		{
			//do nothing
		}
		else if (_brush_align_horizontal == ok::graphics::TextAlign::Center)
		{
			float total_advance = 0.0f;

			for (int i = 0; i < characters_count - 1; i++)
			{
				charcode = characters[i];

				if (charcode == 9) //tab
				{
					total_advance += _font->GetGlyphByCharCode(32).advance * 4;
				}
				else
				{
					total_advance += _font->GetGlyphByCharCode(charcode).advance;
				}

			}

			charcode = characters[characters_count - 1];

			if (charcode == 9) //tab
			{
				total_advance += _font->GetGlyphByCharCode(32).advance * 3;
				total_advance += _font->GetGlyphByCharCode(32).bounds.GetWidth();
			}
			else
			{
				total_advance += _font->GetGlyphByCharCode(charcode).bounds.GetWidth();
			}

			_brush_position.x -= total_advance * rescale_x * 0.5f;
		}
		else if (_brush_align_horizontal == ok::graphics::TextAlign::Right)
		{
			float total_advance = 0.0f;

			for (int i = 0; i < characters_count; i++)
			{
				charcode = characters[i];

				if (charcode == 9) //tab
				{
					total_advance += _font->GetGlyphByCharCode(32).advance * 4;
				}
				else
				{
					total_advance += _font->GetGlyphByCharCode(charcode).advance;
				}
			}

			_brush_position.x -= total_advance * rescale_x;
		}

		//render quads

		_early_batch_flush = false;

		for (int i = 0; i < characters_count; i++)
		{
			charcode = characters[i];

			ok::graphics::FontGlyph& glyph = _font->GetGlyphByCharCode(charcode);

			if (charcode == 32) //space
			{

			}
			else if (charcode == 9) //tab
			{
				_brush_position.x -= glyph.advance*rescale_x; //remove tab shift
				_brush_position.x += _font->GetGlyphByCharCode(32).advance*rescale_x * 4; //add four space shifts
			}
			else
			{
				quad.SetCenter(glm::vec2(
					(glyph.texture_inner_rect.GetLeft() - glyph.bounds.GetLeft()) / glyph.texture_rect.GetWidth(),
					(glyph.texture_inner_rect.GetBottom() + glyph.bounds.GetBottom()) / glyph.texture_rect.GetHeight()));

				quad.SetSize(glyph.texture_rect.GetSize());
				quad.SetUVRect(glyph.texture_rect.GetXY(), glyph.texture_rect.GetSize(), _tex->GetSize());

				quad.SetTransform(glm::vec2(_brush_position.x, _brush_position.y), glm::vec2(rescale_x, rescale_y));

				if (batch_quads_in_use == batch_size)
				{
					_early_batch_flush = true;
					BatchEnd();
					BatchBegin();
					_early_batch_flush = false;
				}

				PushQuad();
			}

			_brush_position.x += glyph.advance*rescale_x;
		}

		//advance to next line

		lines_counter++;

		if (lines_counter == _newline_ranges.size() && _no_lf_in_last_line == true)
		{
			if (_brush_align_vertical == ok::graphics::TextAlign::Top)
			{
				_brush_position.y -= _font->GetGlyphMaxHeight() * rescale_y * _line_spacing_scale;
			}

			if (_brush_align_vertical == ok::graphics::TextAlign::Center)
			{
				_brush_position.y -= _font->GetGlyphMaxHeight() * rescale_y * 0.5f * _line_spacing_scale;
			}
		}
		else
		{
			_brush_position.x = _initial_brush_position_x;

			if (lines_counter == _newline_ranges.size())
			{
				if (_brush_align_vertical == ok::graphics::TextAlign::Bottom)
				{
					_brush_position.y += _font->GetGlyphMaxHeight() * rescale_y * _line_spacing_scale;
				}
				
				if (_brush_align_vertical == ok::graphics::TextAlign::Center)
				{
					_brush_position.y += _font->GetGlyphMaxHeight() * rescale_y * 0.5f * _line_spacing_scale;
				}
			}
			else
			{
				_brush_position.y += _font->GetGlyphMaxHeight() * rescale_y * _line_spacing_scale;
			}
		}
	
	}
}

glm::mat4 ok::graphics::TextBatch2D::DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
		case ok::graphics::ShaderAliasReference::ModelViewProjectionMatrix :
		{
			return ok::graphics::Camera::GetCurrent()->GetVPMatrix();
		}
		break;
		default:
		{

		}
		break;
	}

	return glm::mat4();
}

glm::vec4 ok::graphics::TextBatch2D::DispatchAliasVec4(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
		case ok::graphics::ShaderAliasReference::Callback :
		{
			if (*callback_name_ptr == "fx0") //brush_color
			{
				return _brush_color;
			}
			else if (*callback_name_ptr == "fx3") //brush_gradient_color_up
			{
				return _brush_gradient_colors[0];
			}
			else if (*callback_name_ptr == "fx4") //brush_gradient_color_down
			{
				return _brush_gradient_colors[1];
			}
			else if (*callback_name_ptr == "fx5") //brush_outer_glow_color
			{
				return _brush_outer_glow_color;
			}
			else if (*callback_name_ptr == "fx7") //brush_inner_glow_color
			{
				return _brush_inner_glow_color;
			}
			else if (*callback_name_ptr == "fx9") //brush_inner_shadow_color
			{
				return _brush_inner_shadow_color;
			}
			else if (*callback_name_ptr == "fx10") //brush_inner_glow
			{
				return glm::vec4(_brush_inner_shadow_min_range, _brush_inner_shadow_max_range, _brush_inner_shadow_shift_x_power, _brush_inner_shadow_shift_y_power);
			}
			else if (*callback_name_ptr == "fx11") //brush_inner_glow
			{
				return _brush_outer_shadow_color;
			}
			else if (*callback_name_ptr == "fx12") //brush_inner_glow
			{
				return glm::vec4(_brush_outer_shadow_min_range, _brush_outer_shadow_max_range, _brush_outer_shadow_shift_x_power, _brush_outer_shadow_shift_y_power);
			}
		}
		break;
	}

	return glm::vec4();
}

glm::vec2 ok::graphics::TextBatch2D::DispatchAliasVec2(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if (*callback_name_ptr == "fx6") //brush_outer_glow
		{
			return glm::vec2(_brush_outer_glow_min_range, _brush_outer_glow_max_range);
		}
		else if (*callback_name_ptr == "fx8") //brush_inner_glow
		{
			return glm::vec2(_brush_inner_glow_min_range, _brush_inner_glow_max_range);
		}
	}
	break;
	}

	return glm::vec2();
}

float ok::graphics::TextBatch2D::DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if (*callback_name_ptr == "fx1") //brush_bold_power
		{
			return _brush_bold_power;
		}
		else if (*callback_name_ptr == "fx2") //brush_italic_power
		{
			return _brush_italic_shift;
		}
	}
	break;
	}

	return 0.0f;
}

std::pair<unsigned int*, int> ok::graphics::TextBatch2D::DispatchAliasSubroutineArray(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if (*callback_name_ptr == "current_filters[0]")
		{
			return std::pair<unsigned int*, int>(_filters_enabled, ok::graphics::TextBatch2D_ShaderFiltersIndexes::FiltersLimit);
		}
	}
	break;
	}

	return std::pair<unsigned int*, int>(nullptr, 0);
}

void ok::graphics::TextBatch2D::PushQuad()
{
	float* ptr = nullptr;

	ptr = &(positions_and_texcoords[batch_quads_in_use * quad_xyuv_data_size]);

	memcpy(ptr + 12, &quad.vertices[3], sizeof(float) * 2);
	memcpy(ptr + 14, &quad.uvs[3], sizeof(float) * 2);
	memcpy(ptr + 8, &quad.vertices[2], sizeof(float) * 2);
	memcpy(ptr + 10, &quad.uvs[2], sizeof(float) * 2);
	memcpy(ptr + 4, &quad.vertices[1], sizeof(float) * 2);
	memcpy(ptr + 6, &quad.uvs[1], sizeof(float) * 2);
	memcpy(ptr, &quad.vertices[0], sizeof(float) * 2);
	memcpy(ptr+2, &quad.uvs[0], sizeof(float) * 2);

	batch_quads_in_use++;
}
