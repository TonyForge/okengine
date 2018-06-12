#pragma once

#include "Camera.h"
#include "Transform.h"
#include "Pool.h"
#include "AssetsBasic.h"
#include "SpriteBatch.h"
#include "Input.h"
#include "Material.h"
#include "GameObject.h"
#include "TextBatch.h"
#include "LayeredRenderer.h"

namespace ok
{
	class ui : public ok::graphics::ShaderAliasDispatcher
	{
	public:
		struct widget;
		typedef ok::ui::widget* widget_ptr;
		struct widget 
		{
			ok::ui::widget_ptr ptr();
		};
		
		struct widget_state
		{
			bool mouse_inside;
			bool mouse_down;
			ok::MKey mouse_key;
			glm::vec2 mouse_relative_pos;

			bool on_activate;
			bool on_deactivate;
		};

		static void BeginUI(int screen_width, int screen_height);
		static void EndUI();
		static void FlushBatch();

		static void PushCrop(float left, float top, float width, float height);
		static void PopCrop();

		static void PushTranslate(float x, float y);
		static void PopTranslate();

		static void PushRotate(float angle_degrees_ccw, float around_x = 0.f, float around_y = 0.f);
		static void PopRotate();

		static void PushScale(float scale_x, float scale_y, float around_x = 0.f, float around_y = 0.f);
		static void PopScale();

		static void PushSRT(float x, float y, float hotspot_x, float hotspot_y, float angle_degrees_ccw, float scale_x, float scale_y);
		static void PopSRT();

		static void PushResetTransform();
		static void PopResetTransform();

		static void PushNonActivable(bool enabled = false);
		static void PopNonActivable();

		static void EnableSmooth();
		static void DisableSmooth();

		static ok::ui::widget_state& Image(ok::ui::widget_ptr widget, ok::graphics::Texture* texture, float x = 0.f, float y = 0.f, float width = -1.f, float height = -1.f);
		static ok::ui::widget_state& Image(ok::ui::widget_ptr widget, ok::graphics::SpriteInfo* sprite, float x = 0.f, float y = 0.f, float width = -1.f, float height = -1.f);

		static void Text(
			ok::graphics::TextCache* text_cache,
			float x = 0.f, float y = 0.f,
			ok::graphics::TextAlign halign = ok::graphics::TextAlign::Left,
			ok::graphics::TextAlign valign = ok::graphics::TextAlign::Top
		);
		static ok::graphics::TextBatch2D& GetTextBatch();

		//no rotations, scales here... smooth disable recommended
		static ok::ui::widget_state& Blit(ok::ui::widget_ptr widget, ok::graphics::Texture* texture, float x = 0.f, float y = 0.f);
		static ok::ui::widget_state& Blit(ok::ui::widget_ptr widget, ok::graphics::SpriteInfo* sprite, float x = 0.f, float y = 0.f);

		static void Model(ok::GameObject* model, glm::vec3 euler_angles, glm::vec3 scale, float x = 0.f, float y = 0.f);
		static void Model(ok::GameObject* model, glm::vec3 euler_angles_pre, glm::vec3 euler_angles_post, glm::vec3 scale, float x = 0.f, float y = 0.f);
		static void Model(ok::GameObject* model, glm::vec3 euler_angles_pre, glm::vec3 euler_angles, glm::vec3 euler_angles_post, glm::vec3 scale, float x = 0.f, float y = 0.f);

		static ok::ui::widget_state& Dummy(ok::ui::widget_ptr widget, float x, float y, float width, float height);
		static ok::ui::widget_state& ScrollHorizontal(
			ok::ui::widget_ptr widget, float x, float y, float width, float height,
			int items_visible_count, int items_total,
			int& out__items_visible_first_index,
			float& out__scroll_button_relative_position,
			float& out__scroll_button_relative_size
			);
		//static void BeginWidget();
		//static void EndWidget();

		static void PushEffect_Power(float power);
		static void PopEffect_Power();

		static void PushEffect_Grayscale(float power);
		static void PopEffect_Grayscale();

		static void PushEffect_Fade(float power);
		static void PopEffect_Fade();

		static ok::ui::widget_state& ws();

		float DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type);

		static ok::ui& instance();
		static ok::ui& o();
	private:
		ui() : 
			_camera(ok::graphics::CameraCoordinateSystem::Screen),
			_batch(nullptr),
			_text_batch(nullptr)
		{}
		~ui() {}
		ok::ui(ok::ui const&) : 
			_camera(ok::graphics::CameraCoordinateSystem::Screen),
			_batch(nullptr),
			_text_batch(nullptr)
		{}
		ok::ui& operator= (ok::ui const&) {}

		bool _smooth_enabled = true;
		ok::graphics::Material* _default_material;
		ok::graphics::SpriteBatch* _batch;
		ok::graphics::TextBatch2D* _text_batch;

		std::vector<glm::mat3> _transform_stack;
		ok::graphics::Camera _camera;
		glm::vec3 _mouse_xy;
		glm::vec2 _mem_xy;

		ok::ui::widget_ptr _active_widget = nullptr;
		ok::ui::widget_ptr _deactivated_widget = nullptr;
		ok::ui::widget_state _widget_state;

		std::vector<float> _effects_power;
		std::vector<float> _effects_grayscale;
		std::vector<float> _effects_fade;

		std::vector<bool> _non_activable_stack;

		void _fill_widget_state(ok::ui::widget_ptr widget, float left, float top, float width, float height);
	protected:
	};
}