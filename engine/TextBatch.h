#pragma once

#include "AssetsBasic.h"
#include "Quad.h"
#include "GLBufferObject.h"
#include "Camera.h"
#include "Color.h"

namespace ok
{
	namespace graphics
	{
		enum class TextAlign
		{
			Left,
			Right,
			Top,
			Bottom,
			Center
		};

		class TextBatch2D_ShaderFiltersIndexes
		{
		public:
			unsigned int Undefined;
			unsigned int Default;
			unsigned int Gradient;
			unsigned int InnerGlow;
			unsigned int InnerShadow;
			unsigned int OuterGlow;
			unsigned int OuterShadow;
			static const int FiltersLimit = 4;
		};

		class TextBatch2D;
			
		class TextCache
		{
		public:
			float GetWidth();
			float GetHeight();
		protected:
		private:
			friend class ok::graphics::TextBatch2D;
			std::vector<float> _cached_shader_settings_data;
			std::vector<float> _cached_positions_and_texcoords;
			glm::vec2 _brush_advance;
			ok::Rect2Df _total_rect;
		};

		class TextBatch2D : public ok::graphics::ShaderAliasDispatcher
		{
		public:
			TextBatch2D(int screen_width, int screen_height, int size = 1000);
			~TextBatch2D();

			void ChangeResolution(int screen_width, int screen_height);

			void CacheBegin();
			std::unique_ptr<ok::graphics::TextCache> CacheEnd();

			void BatchBegin();
			void BatchEnd();

			void ResetTotalRect();
			ok::Rect2Df GetTotalRect();

			//this settings can not be changed inside BatchBegin BatchEnd block (because they are uniforms)
			void SetBrushFont(ok::graphics::Font* font);
			void SetBrushColor(ok::Color color);
			void SetBrushBold(float power);
			void SetBrushItalic(float power);
			void SetBrushOuterGlow(ok::Color color, float radius, float blur);
			void SetBrushInnerGlow(ok::Color color, float radius, float blur);
			void SetBrushOuterShadow(ok::Color color, float radius, float blur, float shift_x_power = 0.f, float shift_y_power = 0.f);
			void SetBrushInnerShadow(ok::Color color, float radius, float blur, float shift_x_power, float shift_y_power);
			void SetBrushGradient(ok::Color color_up, ok::Color color_down);

			//this settings dynamically changes when symbols processed, and can be changed by user inside BatchBegin BatchEnd block
			void SetBrushAlignVertical(ok::graphics::TextAlign align);
			void SetBrushAlignHorizontal(ok::graphics::TextAlign align);
			void SetBrushPosition(float x, float y);
			void SetBrushPosition(glm::vec2 pos);
			void GetBrushPosition(float& out_x, float& out_y);
			void GetBrushPosition(glm::vec2& out_pos);
			glm::vec2 GetBrushPosition();

			void SetBrushSize(float px_width, float px_height);
			void SetBrushSize(float px_height);
			void SetLineSpacingScale(float scale);
			void SetRowSpacingPx(float px);

			void SetClipRectEnabled(bool enabled);
			void SetClipRect(ok::Rect2Df rect);

			void Draw(ok::String& text, int from = 0, int to = -1);

			//Caches itself never batched with each other or other quads,
			//so no need to wrap them in BatchBegin, BatchEnd
			void Draw(ok::graphics::TextCache* cache);

			glm::mat4 DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type);
			std::pair<unsigned int*, int> DispatchAliasSubroutineArray(ok::graphics::ShaderAliasReference alias_type);
			std::pair<float*, int> DispatchAliasFloatArray(ok::graphics::ShaderAliasReference alias_type);
		protected:
		private:
			void CacheFlush();
			void PushQuad();

			static ok::Quad quad;
			static const unsigned int quad_xyuv_data_size = 4 * 4; //4 vertices, 4 floats XYUV per vertex
			static const unsigned int quad_index_data_size = 3 * 2; //2 triangles, 3 vertices per triangle, 1 ints INDEX per vertex

			int batch_quads_in_use;
			int batch_size;

			GLBufferObject* glBO;
			float* positions_and_texcoords;
			int* indices;

			ok::graphics::Font* _font;
			ok::graphics::Texture* _tex;

			ok::graphics::Material* _mat;
			ok::graphics::TextBatch2D_ShaderFiltersIndexes _filters_indexes;
			unsigned int _filters_enabled[ok::graphics::TextBatch2D_ShaderFiltersIndexes::FiltersLimit];

			std::vector<std::pair<int, int>> _newline_ranges;
			bool _early_batch_flush;
			ok::Rect2Df _total_rect;
			ok::Rect2Df _clip_rect;

			bool _clip_rect_enabled;

			ok::Color _brush_color;
			float _brush_bold_power;
			float _brush_italic_shift;
			ok::Color _brush_outer_glow_color;
			ok::Color _brush_inner_glow_color;
			ok::Color _brush_outer_shadow_color;
			ok::Color _brush_inner_shadow_color;
			
			float _brush_outer_glow_min_range;
			float _brush_outer_glow_max_range;

			float _brush_inner_glow_min_range;
			float _brush_inner_glow_max_range;

			float _brush_outer_shadow_min_range;
			float _brush_outer_shadow_max_range;

			float _brush_inner_shadow_min_range;
			float _brush_inner_shadow_max_range;

			float _brush_outer_shadow_shift_x_power;
			float _brush_outer_shadow_shift_y_power;

			float _brush_inner_shadow_shift_x_power;
			float _brush_inner_shadow_shift_y_power;

			ok::Color _brush_gradient_colors[2];

			static const int _shader_settings_data_size = 42;
			std::vector<float> _shader_settings_data;

			ok::graphics::TextAlign _brush_align_horizontal;
			ok::graphics::TextAlign _brush_align_vertical;

			glm::vec2 _brush_position;
			glm::vec2 _brush_px_size;
			bool _custom_brush_size_proportions_enabled;
			float _line_spacing_scale;
			float _row_spacing_px;

			ok::graphics::Camera* _camera;
			ok::graphics::TextCache* _cache;

			bool _batch_in_progress = false;
		};
	}
}