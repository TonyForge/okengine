#include "UI.h"

void ok::ui::BeginUI(int screen_width, int screen_height)
{
	if (o()._batch == nullptr)
	{
		o()._batch = new ok::graphics::SpriteBatch();
		o()._batch->shader_alias_dispatcher_mirrors.push_back(&o());
		o()._batch->SetMaterial("SpriteBatch.UIMaterial.xml");
	}

	o()._camera.SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), -1, 1);

	o()._camera.BeginTransform();
	o()._camera.SetPosition(glm::vec3(0.f, 0.f, -1.f));
	o()._camera.SetForward(glm::vec3(0.f, 0.f, 1.f));
	o()._camera.EndTransform(false);

	ok::graphics::Camera::PushCamera(&o()._camera);

	PushResetTransform();

	o()._mouse_xy = o()._camera.ScreenToWorldPosition(glm::vec3(ok::Input::o().MouseXY(), 0.f));

	o()._batch->BatchBegin();
	o()._effects_power.push_back(1.f);
	o()._effects_grayscale.push_back(0.f);

	if (ok::Input::o().KeyPressed(ok::MKey::Left))
	{
		o()._deactivated_widget = o()._active_widget;
		o()._active_widget = nullptr;
	}
	
	PushNonActivable();
}

void ok::ui::EndUI()
{
	PopNonActivable();

	o()._batch->BatchEnd();
	o()._effects_power.pop_back();
	o()._effects_grayscale.pop_back();

	PopResetTransform();

	ok::graphics::Camera::PopCamera();
}

void ok::ui::PushCrop(float left, float top, float width, float height)
{
	glm::mat3 inv_m = glm::inverse(o()._transform_stack.back());

	glm::vec2 left_top = inv_m * glm::vec3(left, top, 1.f);
	glm::vec2 right_bottom = inv_m * glm::vec3(left + width, top + height, 1.f);

	o()._camera.BeginScissorTest(
		static_cast<int>(left_top.x),
		static_cast<int>(left_top.y),
		static_cast<int>(glm::abs(right_bottom.x - left_top.x)),
		static_cast<int>(glm::abs(right_bottom.y - left_top.y))
	);
}

void ok::ui::PopCrop()
{
	o()._batch->BatchEnd();
	o()._camera.EndScissorTest();

	o()._batch->BatchBegin();
}

void ok::ui::PushTranslate(float x, float y)
{
	o()._transform_stack.push_back(glm::translate(o()._transform_stack.back(), glm::vec2(x, y)));
}

void ok::ui::PopTranslate()
{
	o()._transform_stack.pop_back();
}

void ok::ui::PushRotate(float angle_degrees_ccw, float around_x, float around_y)
{
	if (around_x != 0.f || around_y != 0.f)
	{
		o()._transform_stack.push_back (
			glm::translate(
				glm::rotate(
					glm::translate(
						o()._transform_stack.back(),
						glm::vec2(around_x, around_y)
					),
					glm::radians(-angle_degrees_ccw)
				),
				glm::vec2(-around_x, -around_y)
			)
		);
	}
	else
	{
		o()._transform_stack.push_back(
			glm::rotate(
				o()._transform_stack.back(),
				glm::radians(-angle_degrees_ccw)
			)
		);
	}
}

void ok::ui::PopRotate()
{
	o()._transform_stack.pop_back();
}

void ok::ui::PushScale(float scale_x, float scale_y, float around_x, float around_y)
{
	if (around_x != 0.f || around_y != 0.f)
	{
		o()._transform_stack.push_back(
			glm::translate(
				glm::scale(
					glm::translate(
						o()._transform_stack.back(),
						glm::vec2(around_x, around_y)
					),
					glm::vec2(scale_x,scale_y)
				),
				glm::vec2(-around_x, -around_y)
			)
		);
	}
	else
	{
		o()._transform_stack.push_back(
			glm::scale(
				o()._transform_stack.back(),
				glm::vec2(scale_x, scale_y)
			)
		);
	}
}

void ok::ui::PopScale()
{
	o()._transform_stack.pop_back();
}

void ok::ui::PushSRT(float x, float y, float hotspot_x, float hotspot_y, float angle_degrees_ccw, float scale_x, float scale_y)
{
	glm::mat3& m_scale =
		glm::translate(
			glm::scale(
				glm::translate(
					glm::mat3(1.f),
					glm::vec2(hotspot_x, hotspot_y)
				),
				glm::vec2(scale_x, scale_y)
			),
			glm::vec2(-hotspot_x, -hotspot_y)
		);

	glm::mat3& m_rotate =
		glm::translate(
			glm::rotate(
				glm::translate(
					glm::mat3(1.f),
					glm::vec2(hotspot_x, hotspot_y)
				),
				glm::radians(-angle_degrees_ccw)
			),
			glm::vec2(-hotspot_x, -hotspot_y)
		);

	glm::mat3& m_translate =
		glm::translate(
			glm::mat3(1.f),
			glm::vec2(x - hotspot_x, y - hotspot_y)
		);

	o()._transform_stack.push_back(
		o()._transform_stack.back() * (m_translate * (m_rotate * m_scale))
	);
}

void ok::ui::PopSRT()
{
	o()._transform_stack.pop_back();
}

void ok::ui::PushResetTransform()
{
	o()._transform_stack.push_back(glm::mat3(1.f));
}

void ok::ui::PopResetTransform()
{
	o()._transform_stack.pop_back();
}

void ok::ui::PushNonActivable(bool enabled)
{
	o()._non_activable_stack.push_back(enabled);
}

void ok::ui::PopNonActivable()
{
	o()._non_activable_stack.pop_back();
}

ok::ui::widget_state & ok::ui::Image(ok::ui::widget_ptr widget, ok::graphics::Texture * texture, float x, float y, float width, float height)
{
	if (width == -1.f) width = static_cast<float>(texture->GetSize().x);
	if (height == -1.f) height = static_cast<float>(texture->GetSize().y);

	o()._fill_widget_state(widget, x, y, width, height);

	//PushScale
	ok::graphics::SpriteInfo sprite;
	sprite.rect = ok::graphics::TextureRect(texture);
	sprite.flip_y = false;
	
	sprite.scale.x = width / texture->GetSize().x;
	sprite.scale.y = height / texture->GetSize().y;

	sprite.hotspot = glm::vec2((-x / texture->GetSize().x) / sprite.scale.x, (-y / texture->GetSize().y) / sprite.scale.y);

	o()._batch->Draw(&sprite, o()._transform_stack.back());

	return o()._widget_state;
}

ok::ui::widget_state & ok::ui::Dummy(ok::ui::widget_ptr widget, float x, float y, float width, float height)
{
	o()._fill_widget_state(widget, x, y, width, height);

	return o()._widget_state;
}

void ok::ui::PushEffect_Power(float power)
{
	o()._batch->BatchEnd();
	o()._batch->BatchBegin();

	o()._effects_power.push_back(power);
}

void ok::ui::PopEffect_Power()
{
	o()._batch->BatchEnd();
	o()._batch->BatchBegin();

	o()._effects_power.pop_back();
}

void ok::ui::PushEffect_Grayscale(float power)
{
	o()._batch->BatchEnd();
	o()._batch->BatchBegin();

	o()._effects_grayscale.push_back(power);
}

void ok::ui::PopEffect_Grayscale()
{
	o()._batch->BatchEnd();
	o()._batch->BatchBegin();

	o()._effects_grayscale.pop_back();
}

float ok::ui::DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::Callback:
	{
		if (*callback_name_ptr == "effects_power")
			return _effects_power.back();
		else if (*callback_name_ptr == "effects_grayscale")
			return _effects_grayscale.back();
	}
	break;
	}

	unresolved_alias = true;
	return 0.0f;
}

ok::ui & ok::ui::instance()
{
	static ok::ui s;
	return s;
}

ok::ui & ok::ui::o()
{
	return instance();
}

void ok::ui::_fill_widget_state(ok::ui::widget_ptr widget, float left, float top, float width, float height)
{
	std::memset(&_widget_state, 0, sizeof(ok::ui::widget_state));

	glm::vec2 local_mouse = glm::inverse(_transform_stack.back()) * glm::vec3(_mouse_xy.x, _mouse_xy.y, 1.f);

	if (local_mouse.x >= left && local_mouse.y >= top &&
		local_mouse.x <= (left + width) && local_mouse.y <= (top + height))
	{
		_widget_state.mouse_inside = true;

		if (ok::Input::o().KeyPressed(ok::MKey::Left))
		{
			if (_active_widget != widget && _non_activable_stack.back() == false)
			{
				_deactivated_widget = _active_widget;
				_active_widget = widget;
				_widget_state.on_activate = true;
			}
		}
	}

	if (_active_widget == widget)
	{
		if (ok::Input::o().KeyDown(ok::MKey::Left))
		{
			_widget_state.mouse_down = true;
			_widget_state.mouse_key = ok::MKey::Left;
		}
	}

	if (_deactivated_widget == widget)
	{
		_widget_state.on_deactivate = true;
		_deactivated_widget = nullptr;
	}
}

ok::ui::widget_ptr ok::ui::widget::ptr()
{
	return this;
}
