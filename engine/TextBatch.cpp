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

	_cache = nullptr;
	_tex = nullptr;
	_font = nullptr;
	batch_quads_in_use = 0;

	_camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::Screen);
	_camera->SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), -1, 1);

	_camera->BeginTransform();
	_camera->SetPosition(glm::vec3(0.f, 0.f, -1.f));
	_camera->SetForward(glm::vec3(0.f, 0.f, 1.f));
	_camera->EndTransform(false);

	_brush_align_horizontal = ok::graphics::TextAlign::Left;
	_brush_align_vertical = ok::graphics::TextAlign::Top;

	_early_batch_flush = false;
	_custom_brush_size_proportions_enabled = false;
	_line_spacing_scale = 1.f;
	_row_spacing_px = 0.f;
	_clip_rect_enabled = false;

	_shader_settings_data.resize(_shader_settings_data_size);

	SetBrushPosition(0.f, 0.f);
	SetBrushSize(32);

	ResetTotalRect();
}

ok::graphics::TextBatch2D::~TextBatch2D()
{
	delete glBO;
}

void ok::graphics::TextBatch2D::ChangeResolution(int screen_width, int screen_height)
{
	_camera->SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), -1, 1);
}

void ok::graphics::TextBatch2D::CacheBegin()
{
	batch_quads_in_use = 0;
	
	_cache = new ok::graphics::TextCache();
	_cache->_brush_advance = _brush_position;

	ResetTotalRect();
}

std::unique_ptr<ok::graphics::TextCache> ok::graphics::TextBatch2D::CacheEnd()
{
	if (batch_quads_in_use != 0)
		CacheFlush();

	_cache->_cached_shader_settings_data = _shader_settings_data;
	_cache->_brush_advance = _brush_position - _cache->_brush_advance;
	_cache->_total_rect = _total_rect;

	ok::graphics::TextCache* _result = _cache;
	_cache = nullptr;

	batch_quads_in_use = 0;

	return std::unique_ptr<ok::graphics::TextCache>(_result);
}

void ok::graphics::TextBatch2D::BatchBegin()
{
	if (batch_quads_in_use != 0 || _font == nullptr)
	{
		BatchEnd();
	}

	_batch_in_progress = true;
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

	_batch_in_progress = false;
}

void ok::graphics::TextBatch2D::ResetTotalRect()
{
	_total_rect.ResetMerge();
}

ok::Rect2Df ok::graphics::TextBatch2D::GetTotalRect()
{
	return _total_rect;
}

void ok::graphics::TextBatch2D::SetBrushFont(ok::graphics::Font * font)
{
	_font = font;
	_tex = _font->GetTexture();

	ok::Color clr,clr2;
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

	_font->GetBrushGradient(clr, clr2);
	SetBrushGradient(clr, clr2);
}

void ok::graphics::TextBatch2D::SetBrushColor(ok::Color color)
{
	_brush_color = color;

	_shader_settings_data[0] = _brush_color.r;
	_shader_settings_data[1] = _brush_color.g;
	_shader_settings_data[2] = _brush_color.b;
	_shader_settings_data[3] = _brush_color.a;
}

void ok::graphics::TextBatch2D::SetBrushBold(float power)
{
	_brush_bold_power = power;

	_shader_settings_data[4] = _brush_bold_power;
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

	_shader_settings_data[5] = _brush_italic_shift;
}

void ok::graphics::TextBatch2D::SetBrushOuterGlow(ok::Color color, float radius, float blur)
{
	_brush_outer_glow_color = color;

	radius = 0.45f*(1.0f - _brush_bold_power) + (1.0f - 0.45f*(1.0f - _brush_bold_power))*radius;

	_brush_outer_glow_min_range = 1.0f - glm::clamp(radius, 0.f, 1.f);
	_brush_outer_glow_max_range = glm::clamp(_brush_outer_glow_min_range + blur, 0.f, 1.f);

	_shader_settings_data[14] = _brush_outer_glow_color.r;
	_shader_settings_data[15] = _brush_outer_glow_color.g;
	_shader_settings_data[16] = _brush_outer_glow_color.b;
	_shader_settings_data[17] = _brush_outer_glow_color.a;
	_shader_settings_data[18] = _brush_outer_glow_min_range;
	_shader_settings_data[19] = _brush_outer_glow_max_range;
}

void ok::graphics::TextBatch2D::SetBrushInnerGlow(ok::Color color, float radius, float blur)
{
	_brush_inner_glow_color = color;

	radius = 0.45f*(1.0f - _brush_bold_power) + (1.0f - 0.45f*(1.0f - _brush_bold_power))*radius;
	blur = (1.0f - 0.45f*(1.0f - _brush_bold_power)) * blur;

	_brush_inner_glow_min_range = glm::clamp(radius, 0.f, 1.f);
	_brush_inner_glow_max_range = glm::clamp(_brush_inner_glow_min_range + glm::clamp(blur, 0.f, 1.f),0.f,1.f);

	_shader_settings_data[20] = _brush_inner_glow_color.r;
	_shader_settings_data[21] = _brush_inner_glow_color.g;
	_shader_settings_data[22] = _brush_inner_glow_color.b;
	_shader_settings_data[23] = _brush_inner_glow_color.a;
	_shader_settings_data[24] = _brush_inner_glow_min_range;
	_shader_settings_data[25] = _brush_inner_glow_max_range;
}

void ok::graphics::TextBatch2D::SetBrushOuterShadow(ok::Color color, float radius, float blur, float shift_x_power, float shift_y_power)
{
	_brush_outer_shadow_color = color;

	radius = 0.45f*(1.0f - _brush_bold_power) + (1.0f - 0.45f*(1.0f - _brush_bold_power))*radius;

	_brush_outer_shadow_min_range = 1.0f - glm::clamp(radius, 0.f, 1.f);
	_brush_outer_shadow_max_range = glm::clamp(_brush_outer_shadow_min_range + blur, 0.f, 1.f);

	_brush_outer_shadow_shift_x_power = -shift_x_power * (_font->GetGlyphMaxWidth() / static_cast<float>(_tex->GetSize().x));
	_brush_outer_shadow_shift_y_power = -shift_y_power * (_font->GetGlyphMaxHeight() / static_cast<float>(_tex->GetSize().y));

	_shader_settings_data[34] = _brush_outer_shadow_color.r;
	_shader_settings_data[35] = _brush_outer_shadow_color.g;
	_shader_settings_data[36] = _brush_outer_shadow_color.b;
	_shader_settings_data[37] = _brush_outer_shadow_color.a;
	_shader_settings_data[38] = _brush_outer_shadow_min_range;
	_shader_settings_data[39] = _brush_outer_shadow_max_range;
	_shader_settings_data[40] = _brush_outer_shadow_shift_x_power;
	_shader_settings_data[41] = _brush_outer_shadow_shift_y_power;
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

	_shader_settings_data[26] = _brush_inner_shadow_color.r;
	_shader_settings_data[27] = _brush_inner_shadow_color.g;
	_shader_settings_data[28] = _brush_inner_shadow_color.b;
	_shader_settings_data[29] = _brush_inner_shadow_color.a;
	_shader_settings_data[30] = _brush_inner_shadow_min_range;
	_shader_settings_data[31] = _brush_inner_shadow_max_range;
	_shader_settings_data[32] = _brush_inner_shadow_shift_x_power;
	_shader_settings_data[33] = _brush_inner_shadow_shift_y_power;
}

void ok::graphics::TextBatch2D::SetBrushGradient(ok::Color color_up, ok::Color color_down)
{
	_brush_gradient_colors[0] = color_up;
	_brush_gradient_colors[1] = color_down;

	_shader_settings_data[6] = _brush_gradient_colors[0].r;
	_shader_settings_data[7] = _brush_gradient_colors[0].g;
	_shader_settings_data[8] = _brush_gradient_colors[0].b;
	_shader_settings_data[9] = _brush_gradient_colors[0].a;
	_shader_settings_data[10] = _brush_gradient_colors[1].r;
	_shader_settings_data[11] = _brush_gradient_colors[1].g;
	_shader_settings_data[12] = _brush_gradient_colors[1].b;
	_shader_settings_data[13] = _brush_gradient_colors[1].a;
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

void ok::graphics::TextBatch2D::SetRowSpacingPx(float px)
{
	_row_spacing_px = px;
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

			_brush_position.x -= (total_advance * rescale_x + _row_spacing_px * (characters_count - 2)) * 0.5f;
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

			_brush_position.x -= total_advance * rescale_x + _row_spacing_px * (characters_count - 1);
		}

		//render quads

		_early_batch_flush = false;

		for (int i = 0; i < characters_count; i++)
		{
			charcode = characters[i];

			ok::graphics::FontGlyph& glyph = _font->GetGlyphByCharCode(charcode);

			if (charcode == 32) //space
			{
				_total_rect.Merge(ok::Rect2Df(_brush_position.x + glyph.bounds.GetLeft()*rescale_x, _brush_position.y - (glyph.bounds.GetHeight() + glyph.bounds.GetBottom())*rescale_y, glyph.bounds.GetWidth()*rescale_x, glyph.bounds.GetHeight()*rescale_y));
			}
			else if (charcode == 9) //tab
			{
				glyph = _font->GetGlyphByCharCode(32);
				_total_rect.Merge(ok::Rect2Df(_brush_position.x + glyph.bounds.GetLeft()*rescale_x, _brush_position.y - (glyph.bounds.GetHeight() + glyph.bounds.GetBottom())*rescale_y, glyph.bounds.GetWidth()*rescale_x*4.f, glyph.bounds.GetHeight()*rescale_y));

				_brush_position.x += glyph.advance * rescale_x * 3;
			}
			else
			{
				quad.SetCenter(glm::vec2(
					(glyph.texture_inner_rect.GetLeft() - glyph.bounds.GetLeft()) / glyph.texture_rect.GetWidth(),
					(glyph.texture_inner_rect.GetBottom() + glyph.bounds.GetBottom()) / glyph.texture_rect.GetHeight()));

				quad.SetSize(glyph.texture_rect.GetSize());
				quad.SetUVRect(glyph.texture_rect.GetXY(), glyph.texture_rect.GetSize(), _tex->GetSize());

				quad.SetTransform(glm::vec2(_brush_position.x, _brush_position.y), glm::vec2(rescale_x, rescale_y));

				_total_rect.Merge(ok::Rect2Df(_brush_position.x+glyph.bounds.GetLeft()*rescale_x, _brush_position.y - (glyph.bounds.GetHeight()+glyph.bounds.GetBottom())*rescale_y, glyph.bounds.GetWidth()*rescale_x, glyph.bounds.GetHeight()*rescale_y));

				if (batch_quads_in_use == batch_size)
				{
					_early_batch_flush = true;
					if (_cache == nullptr)
					{
						BatchEnd();
						BatchBegin();
					}
					else
					{
						CacheFlush();
					}
					_early_batch_flush = false;
				}

				PushQuad();
			}

			_brush_position.x += glyph.advance*rescale_x + _row_spacing_px;
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

void ok::graphics::TextBatch2D::Draw(ok::graphics::TextCache * cache)
{
	bool _dont_forget_restart_batch = false;

	if (_batch_in_progress == true)
	{
		BatchEnd(); //ensure that all previous rendering will be flushed
		_dont_forget_restart_batch = true;
	}

	ok::graphics::Camera::PushCamera(_camera);

	_camera->BeginTransform();
	_camera->SetPosition(glm::vec3(-_brush_position.x, -_brush_position.y, -1.f));
	_camera->EndTransform(false);

	if (_clip_rect_enabled)
	{
		_camera->BeginScissorTest(static_cast<int>(_clip_rect.GetX()), static_cast<int>(_clip_rect.GetY()), static_cast<int>(_clip_rect.GetWidth()), static_cast<int>(_clip_rect.GetHeight()));
	}

	_mat->SetTexture(0, _tex);

	_cache = cache;
		_mat->Bind(this);
	_cache = nullptr;

	int cache_position = 0;
	int cache_chunk_size;
	int cache_quads_count = cache->_cached_positions_and_texcoords.size() / quad_xyuv_data_size;
	float* vbo = glBO->GetVertexBuffer(0).vertices;

	while (cache_quads_count > 0)
	{
		cache_chunk_size = batch_size;
		cache_quads_count -= batch_size;
		if (cache_quads_count < 0) { cache_chunk_size += cache_quads_count; cache_quads_count = 0; }

		batch_quads_in_use = cache_chunk_size;

		memcpy(vbo, &(cache->_cached_positions_and_texcoords[cache_position * quad_xyuv_data_size]), sizeof(float) * cache_chunk_size * quad_xyuv_data_size);

		cache_position += cache_chunk_size;

		glBO->ReloadVertexBuffer(0);

		glBO->Bind();

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

		glBO->Unbind();
	}

	if (_clip_rect_enabled)
	{
		_camera->EndScissorTest();
	}

	_camera->BeginTransform();
	_camera->SetPosition(glm::vec3(0.f, 0.f, -1.f));
	_camera->EndTransform(false);

	ok::graphics::Camera::PopCamera();

	_total_rect.Merge(ok::Rect2Df(cache->_total_rect.GetX() + _brush_position.x, cache->_total_rect.GetY() + _brush_position.y, cache->_total_rect.GetWidth(), cache->_total_rect.GetHeight()));
	_brush_position += cache->_brush_advance;
	batch_quads_in_use = 0;

	if (_dont_forget_restart_batch)
	{
		BatchBegin();
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
	}

	unresolved_alias = true;
	return glm::mat4();
}

std::pair<unsigned int*, int> ok::graphics::TextBatch2D::DispatchAliasSubroutineArray(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if (*callback_name_ptr == "filters[0]")
		{
			return std::pair<unsigned int*, int>(_filters_enabled, ok::graphics::TextBatch2D_ShaderFiltersIndexes::FiltersLimit);
		}
	}
	break;
	}

	unresolved_alias = true;
	return std::pair<unsigned int*, int>(nullptr, 0);
}

std::pair<float*, int> ok::graphics::TextBatch2D::DispatchAliasFloatArray(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if (*callback_name_ptr == "fx[0]")
		{
			if (_cache == nullptr)
			return std::pair<float*, int>(_shader_settings_data.data(), _shader_settings_data_size);
			else
			return std::pair<float*, int>(_cache->_cached_shader_settings_data.data(), _shader_settings_data_size);
		}
	}
	break;
	}

	unresolved_alias = true;
	return std::pair<float*, int>();
}

void ok::graphics::TextBatch2D::CacheFlush()
{
	int _cache_position = _cache->_cached_positions_and_texcoords.size() - 1;
	if (_cache_position < 0) _cache_position = 0;

	_cache->_cached_positions_and_texcoords.resize(_cache->_cached_positions_and_texcoords.size() + batch_quads_in_use * quad_xyuv_data_size);
	memcpy(&(_cache->_cached_positions_and_texcoords.data()[_cache_position]), positions_and_texcoords, sizeof(float) * batch_quads_in_use * quad_xyuv_data_size);

	batch_quads_in_use = 0;
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

float ok::graphics::TextCache::GetWidth()
{
	return _total_rect.GetWidth();
}

float ok::graphics::TextCache::GetHeight()
{
	return _total_rect.GetHeight();
}
