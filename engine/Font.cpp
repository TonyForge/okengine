#include "Font.h"

ok::graphics::InternalFont::InternalFont()
{
	unknown_glyph.unknown = true;
	_font_bounds.SetYUp(true);
}

void ok::graphics::InternalFont::SetTexture(ok::graphics::Texture * tex)
{
	_tex = tex;
}

ok::graphics::Texture * ok::graphics::InternalFont::GetTexture()
{
	return _tex;
}

std::vector<ok::graphics::FontGlyph>& ok::graphics::InternalFont::GetGlyphs()
{
	return _glyphs;
}

std::vector<unsigned int>& ok::graphics::InternalFont::GetGlyphsCharCodes()
{
	return _glyphs_charcodes;
}

ok::graphics::FontGlyph& ok::graphics::InternalFont::GetGlyph(unsigned int charcode)
{
	std::vector<unsigned int>::iterator char_pos = std::find(_glyphs_charcodes.begin(), _glyphs_charcodes.end(), charcode);

	if (char_pos == _glyphs_charcodes.end())
	{
		return unknown_glyph;
	}
	return _glyphs[static_cast<size_t>(char_pos - _glyphs_charcodes.begin())/*std::distance(_glyphs_charcodes.begin(), char_pos)*/];
}

ok::Rect2Df & ok::graphics::InternalFont::GetFontBounds()
{
	return _font_bounds;
}

ok::graphics::Font::Font()
{
	_internal_font = nullptr;

	_brush_size_override_enabled = false;
	_brush_size_override_custom_proportions_enabled = false;
	_brush_size_override_px_width = -1;
	_brush_size_override_px_height = -1;
	_brush_color = ok::Color(0.f,0.f,0.f,1.f);
	_brush_bold_power = 0.f;
	_brush_italic_power = 0.f;
	_brush_outer_glow_color = ok::Color(0.f, 0.f, 0.f, 0.f);
	_brush_outer_glow_radius = 0.f;
	_brush_outer_glow_blur = 0.f;
	_brush_inner_glow_color = ok::Color(0.f, 0.f, 0.f, 0.f);
	_brush_inner_glow_radius = 0.f;
	_brush_inner_glow_blur = 0.f;
	_brush_outer_shadow_color = ok::Color(0.f, 0.f, 0.f, 0.f);
	_brush_outer_shadow_radius = 0.f;
	_brush_outer_shadow_blur = 0.f;
	_brush_outer_shadow_shift_x_power = 0.f;
	_brush_outer_shadow_shift_y_power = 0.f;
	_brush_inner_shadow_color = ok::Color(0.f, 0.f, 0.f, 0.f);
	_brush_inner_shadow_radius = 0.f;
	_brush_inner_shadow_blur = 0.f;
	_brush_inner_shadow_shift_x_power = 0.f;
	_brush_inner_shadow_shift_y_power = 0.f;
	_brush_gradient_colors[0] = ok::Color(0.f, 0.f, 0.f, 0.f);
	_brush_gradient_colors[1] = ok::Color(0.f, 0.f, 0.f, 0.f);
}

void ok::graphics::Font::SetInternalFont(ok::graphics::InternalFont * internal_font)
{
	_internal_font = internal_font;
}

float ok::graphics::Font::GetGlyphMaxWidth()
{
	return _internal_font->GetFontBounds().GetWidth();
}

float ok::graphics::Font::GetGlyphMaxHeight()
{
	return _internal_font->GetFontBounds().GetHeight();
}

float ok::graphics::Font::GetNewLineShift()
{
	ok::Rect2Df& bounds = _internal_font->GetFontBounds();
	return bounds.GetHeight();
}

ok::graphics::Texture * ok::graphics::Font::GetTexture()
{
	return _internal_font->GetTexture();
}

ok::graphics::FontGlyph & ok::graphics::Font::GetGlyphByCharCode(unsigned int charcode)
{
	return _internal_font->GetGlyph(charcode);
}

void ok::graphics::Font::SetBrushSizeOverride(bool enabled)
{
	_brush_size_override_enabled = enabled;
}

void ok::graphics::Font::SetBrushSizeOverride(bool enabled, float px_width, float px_height)
{
	_brush_size_override_enabled = enabled;
	_brush_size_override_custom_proportions_enabled = true;
	_brush_size_override_px_width = px_width;
	_brush_size_override_px_height = px_height;
}

void ok::graphics::Font::SetBrushSizeOverride(bool enabled, float px_height)
{
	_brush_size_override_enabled = enabled;
	_brush_size_override_custom_proportions_enabled = false;
	_brush_size_override_px_height = px_height;
}

void ok::graphics::Font::SetBrushColor(ok::Color color)
{
	_brush_color = color;
}

void ok::graphics::Font::SetBrushBold(float power)
{
	_brush_bold_power = power;
}

void ok::graphics::Font::SetBrushItalic(float power)
{
	_brush_italic_power = power;
}

void ok::graphics::Font::SetBrushOuterGlow(ok::Color color, float radius, float blur)
{
	_brush_outer_glow_color = color;
	_brush_outer_glow_radius = radius;
	_brush_outer_glow_blur = blur;
}

void ok::graphics::Font::SetBrushInnerGlow(ok::Color color, float radius, float blur)
{
	_brush_inner_glow_color = color;
	_brush_inner_glow_radius = radius;
	_brush_inner_glow_blur = blur;
}

void ok::graphics::Font::SetBrushOuterShadow(ok::Color color, float radius, float blur, float shift_x_power, float shift_y_power)
{
	_brush_outer_shadow_color = color;
	_brush_outer_shadow_radius = radius;
	_brush_outer_shadow_blur = blur;
	_brush_outer_shadow_shift_x_power = shift_x_power;
	_brush_outer_shadow_shift_y_power = shift_y_power;
}

void ok::graphics::Font::SetBrushInnerShadow(ok::Color color, float radius, float blur, float shift_x_power, float shift_y_power)
{
	_brush_inner_shadow_color = color;
	_brush_inner_shadow_radius = radius;
	_brush_inner_shadow_blur = blur;
	_brush_inner_shadow_shift_x_power = shift_x_power;
	_brush_inner_shadow_shift_y_power = shift_y_power;
}

void ok::graphics::Font::SetBrushGradient(ok::Color color_up, ok::Color color_down)
{
	_brush_gradient_colors[0] = color_up;
	_brush_gradient_colors[1] = color_down;
}

bool ok::graphics::Font::IsBrushSizeOverrideEnabled()
{
	return _brush_size_override_enabled;
}

bool ok::graphics::Font::IsBrushSizeOverrideCustomProportionsEnabled()
{
	return _brush_size_override_custom_proportions_enabled;
}

void ok::graphics::Font::GetBrushSizeOverride(float & px_height)
{
	px_height = _brush_size_override_px_height;
}

void ok::graphics::Font::GetBrushSizeOverride(float & px_width, float & px_height)
{
	px_width = _brush_size_override_px_width;
	px_height = _brush_size_override_px_height;
}

//------------


void ok::graphics::Font::GetBrushColor(ok::Color& color)
{
	color = _brush_color;
}

void ok::graphics::Font::GetBrushBold(float& power)
{
	power = _brush_bold_power;
}

void ok::graphics::Font::GetBrushItalic(float& power)
{
	power = _brush_italic_power;
}

void ok::graphics::Font::GetBrushOuterGlow(ok::Color & color, float & radius, float & blur)
{
	color = _brush_outer_glow_color;
	radius = _brush_outer_glow_radius;
	blur = _brush_outer_glow_blur;
}

void ok::graphics::Font::GetBrushInnerGlow(ok::Color & color, float & radius, float & blur)
{
	color = _brush_inner_glow_color;
	radius = _brush_inner_glow_radius;
	blur = _brush_inner_glow_blur;
}

void ok::graphics::Font::GetBrushOuterShadow(ok::Color & color, float & radius, float & blur, float & shift_x_power, float & shift_y_power)
{
	color = _brush_outer_shadow_color;
	radius = _brush_outer_shadow_radius;
	blur = _brush_outer_shadow_blur;
	shift_x_power = _brush_outer_shadow_shift_x_power;
	shift_y_power = _brush_outer_shadow_shift_y_power;
}

void ok::graphics::Font::GetBrushInnerShadow(ok::Color & color, float & radius, float & blur, float & shift_x_power, float & shift_y_power)
{
	color = _brush_inner_shadow_color;
	radius = _brush_inner_shadow_radius;
	blur = _brush_inner_shadow_blur;
	shift_x_power = _brush_inner_shadow_shift_x_power;
	shift_y_power = _brush_inner_shadow_shift_y_power;
}

void ok::graphics::Font::GetBrushGradient(ok::Color & color_up, ok::Color & color_down)
{
	color_up = _brush_gradient_colors[0];
	color_down = _brush_gradient_colors[1];
}

ok::graphics::FontGlyph::FontGlyph()
{
	texture_rect.SetYUp(false);
	texture_inner_rect.SetYUp(false);
	bounds.SetYUp(true);
}
