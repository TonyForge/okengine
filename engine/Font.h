#pragma once

#include "Texture.h"
#include "RenderTarget.h"
#include "Rect2D.h"
#include "Color.h"

namespace ok
{
	namespace graphics
	{
		class FontGlyph
		{
		public:
			FontGlyph();
			ok::Rect2Di texture_rect;
			ok::Rect2Di texture_inner_rect;
			ok::Rect2Df bounds;
			float advance;
			bool unknown;
		protected:
		private:
		};

		class InternalFont
		{
		public:
			InternalFont();
			void SetTexture(ok::graphics::Texture* tex);
			ok::graphics::Texture* GetTexture();
			std::vector<ok::graphics::FontGlyph>& GetGlyphs();
			std::vector<unsigned int>& GetGlyphsCharCodes();
			ok::graphics::FontGlyph& GetGlyph(unsigned int charcode);
			ok::Rect2Df& GetFontBounds();
		protected:
		private:
			ok::graphics::Texture* _tex;
			std::vector<ok::graphics::FontGlyph> _glyphs;
			std::vector<unsigned int> _glyphs_charcodes;
			ok::graphics::FontGlyph unknown_glyph;
			ok::Rect2Df _font_bounds;
		};

		//texcoords

		class Font
		{
		public:
			Font();

			void SetInternalFont(ok::graphics::InternalFont* internal_font);
			
			float GetGlyphMaxWidth();
			float GetGlyphMaxHeight();
			float GetNewLineShift();
			ok::graphics::Texture* GetTexture();
			ok::graphics::FontGlyph& GetGlyphByCharCode(unsigned int charcode);

			void SetBrushSizeOverride(bool enabled);
			void SetBrushSizeOverride(bool enabled, float px_width, float px_height);
			void SetBrushSizeOverride(bool enabled, float px_height);

			void SetBrushColor(ok::Color color);
			void SetBrushBold(float power);
			void SetBrushItalic(float power);
			void SetBrushOuterGlow(ok::Color color, float radius, float blur);
			void SetBrushInnerGlow(ok::Color color, float radius, float blur);
			void SetBrushOuterShadow(ok::Color color, float radius, float blur, float shift_x_power = 0.f, float shift_y_power = 0.f);
			void SetBrushInnerShadow(ok::Color color, float radius, float blur, float shift_x_power, float shift_y_power);
			void SetBrushGradient(ok::Color color_up, ok::Color color_down);

			bool IsBrushSizeOverrideEnabled();
			bool IsBrushSizeOverrideCustomProportionsEnabled();
			void GetBrushSizeOverride(float& px_height);
			void GetBrushSizeOverride(float& px_width, float& px_height);

			void GetBrushColor(ok::Color& color);
			void GetBrushBold(float& power);
			void GetBrushItalic(float& power);
			void GetBrushOuterGlow(ok::Color& color, float& radius, float& blur);
			void GetBrushInnerGlow(ok::Color& color, float& radius, float& blur);
			void GetBrushOuterShadow(ok::Color& color, float& radius, float& blur, float& shift_x_power, float& shift_y_power);
			void GetBrushInnerShadow(ok::Color& color, float& radius, float& blur, float& shift_x_power, float& shift_y_power);
			void GetBrushGradient(ok::Color& color_up, ok::Color& color_down);

		protected:
		private:
			ok::graphics::InternalFont* _internal_font;

			bool _brush_size_override_enabled;
			float _brush_size_override_px_width;
			float _brush_size_override_px_height;
			bool _brush_size_override_custom_proportions_enabled;

			ok::Color _brush_color;
			float _brush_bold_power;
			float _brush_italic_power;
			ok::Color _brush_outer_glow_color;
			float _brush_outer_glow_radius;
			float _brush_outer_glow_blur;
			ok::Color _brush_inner_glow_color;
			float _brush_inner_glow_radius;
			float _brush_inner_glow_blur;
			ok::Color _brush_outer_shadow_color;
			float _brush_outer_shadow_radius;
			float _brush_outer_shadow_blur;
			float _brush_outer_shadow_shift_x_power;
			float _brush_outer_shadow_shift_y_power;
			ok::Color _brush_inner_shadow_color;
			float _brush_inner_shadow_radius;
			float _brush_inner_shadow_blur;
			float _brush_inner_shadow_shift_x_power;
			float _brush_inner_shadow_shift_y_power;
			ok::Color _brush_gradient_colors[2];
		};
	}
}