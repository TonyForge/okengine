#include "UI.h"

void ok::ui::BeginUI(int screen_width, int screen_height)
{
	if (o()._text_batch == nullptr)
	{
		o()._text_batch = new ok::graphics::TextBatch2D(screen_width, screen_height);
	}
	else
	{
		o()._text_batch->ChangeResolution(screen_width, screen_height);
	}

	if (o()._batch == nullptr)
	{
		o()._batch = new ok::graphics::SpriteBatch();
		o()._batch->shader_alias_dispatcher_mirrors.push_back(&o());

		o()._default_material = ok::AssetsBasic::instance().GetMaterial("SpriteBatch.UIMaterial.xml");
		o()._batch->SetMaterial(o()._default_material);
	}

	o()._camera.SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), 1, 1000);

	o()._camera.BeginTransform();
	o()._camera.SetPosition(glm::vec3(0.f, 0.f, -500.f));
	o()._camera.EndTransform(false);


	ok::graphics::Camera::PushCamera(&o()._camera);

	PushResetTransform();

	o()._mouse_xy = o()._camera.ScreenToWorldPosition(glm::vec3(ok::Input::o().MouseXY(), 0.f));

	o()._batch->BatchBegin();
	o()._effects_power.push_back(1.f);
	o()._effects_grayscale.push_back(0.f);
	o()._effects_fade.push_back(0.f);

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
	o()._effects_fade.pop_back();

	PopResetTransform();

	ok::graphics::Camera::PopCamera();
}

void ok::ui::FlushBatch()
{
	o()._batch->BatchEnd();
	o()._batch->BatchBegin();
}

void ok::ui::PushCrop(float left, float top, float width, float height)
{
	//glm::mat3 inv_m = glm::inverse(o()._transform_stack.back());
	//glm::vec2 transformed_position = o()._transform_stack.back() * glm::vec3(x, y, 1.f);

	glm::mat3& m = o()._transform_stack.back();

	glm::vec2 left_top = m * glm::vec3(left, top, 1.f);
	glm::vec2 right_bottom = m * glm::vec3(left + width, top + height, 1.f);

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

void ok::ui::EnableSmooth()
{
	if (o()._smooth_enabled == true)
	{
		//do nothing
	}
	else
	{
		o()._batch->BatchEnd();
		o()._batch->BatchBegin();
		o()._smooth_enabled = true;
	}
	o()._default_material->AllowTextureLinearFilter();
}

void ok::ui::DisableSmooth()
{
	if (o()._smooth_enabled == true)
	{
		o()._batch->BatchEnd();
		o()._batch->BatchBegin();
		o()._smooth_enabled = false;
	}
	else
	{
		//do nothing
	}
	o()._default_material->ForbidTextureLinearFilter();
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

ok::ui::widget_state & ok::ui::Image(ok::ui::widget_ptr widget, ok::graphics::SpriteInfo * sprite, float x, float y, float width, float height)
{
	if (width == -1.f) width = static_cast<float>(sprite->rect.width);
	if (height == -1.f) height = static_cast<float>(sprite->rect.height);

	o()._fill_widget_state(widget, x, y, width, height);

	ok::graphics::SpriteInfo _sprite = *sprite;

	_sprite.scale.x *= width / sprite->rect.width;
	_sprite.scale.y *= height / sprite->rect.height;

	_sprite.hotspot += glm::vec2((-x / sprite->rect.width) / _sprite.scale.x, (-y / sprite->rect.height) / _sprite.scale.y);

	o()._batch->Draw(&_sprite, o()._transform_stack.back());

	return o()._widget_state;
}

void ok::ui::Text(
	ok::graphics::TextCache * text_cache,
	float x, float y,
	ok::graphics::TextAlign halign,
	ok::graphics::TextAlign valign)
{
	glm::vec2 transformed_position = o()._transform_stack.back() * glm::vec3(x, y, 1.f);

	if (halign == ok::graphics::TextAlign::Center)
	{
		transformed_position.x -= text_cache->GetWidth() * 0.5f;
	}
	else if (halign == ok::graphics::TextAlign::Right)
	{
		transformed_position.x -= text_cache->GetWidth();
	}

	if (valign == ok::graphics::TextAlign::Center)
	{
		transformed_position.y -= text_cache->GetHeight() * 0.5f;
	}
	else if (halign == ok::graphics::TextAlign::Bottom)
	{
		transformed_position.y -= text_cache->GetHeight();
	}

	o()._text_batch->SetBrushPosition(transformed_position);
	//o()._text_batch->SetBrushAlignHorizontal(halign);
	//o()._text_batch->SetBrushAlignHorizontal(valign);
	o()._text_batch->Draw(text_cache);
}

ok::graphics::TextBatch2D & ok::ui::GetTextBatch()
{
	if (o()._text_batch == nullptr)
	{
		o()._text_batch = new ok::graphics::TextBatch2D(0, 0);
	}

	return *o()._text_batch;
}

ok::ui::widget_state & ok::ui::Blit(ok::ui::widget_ptr widget, ok::graphics::Texture * texture, float x, float y)
{
	float width = static_cast<float>(texture->GetSize().x);
	float height = height = static_cast<float>(texture->GetSize().y);

	o()._fill_widget_state(widget, glm::floor(x), glm::floor(y), width, height);

	glm::vec2 transformed_position = o()._transform_stack.back() * glm::vec3(x, y, 1.f);
	o()._batch->Blit(
		texture,
		static_cast<int>(glm::floor(transformed_position.x)),
		static_cast<int>(glm::floor(transformed_position.y)),
		glm::vec2(0.f,0.f),
		false
	);

	return o()._widget_state;
}

ok::ui::widget_state & ok::ui::Blit(ok::ui::widget_ptr widget, ok::graphics::SpriteInfo * sprite, float x, float y)
{
	float width = static_cast<float>(sprite->rect.width);
	float height = static_cast<float>(sprite->rect.height);

	o()._fill_widget_state(widget, glm::floor(x), glm::floor(y), width, height);

	glm::vec2 transformed_position = o()._transform_stack.back() * glm::vec3(x, y, 1.f);
	o()._batch->Blit(sprite, static_cast<int>(glm::floor(transformed_position.x)), static_cast<int>(glm::floor(transformed_position.y)));

	return o()._widget_state;
}

void ok::ui::Model(ok::GameObject * model, glm::vec3 euler_angles, glm::vec3 scale, float x, float y)
{
	ok::Transform transform;
	glm::vec2 transformed_position = o()._transform_stack.back() * glm::vec3(x, y, 1.f);
	

	ok::Transform* model_parent = model->GetParent();

	transform.AddChild(model);

	transform.BeginTransform();
	transform.SetPosition(glm::vec3(transformed_position, 0.f));
	transform.SetRotation(euler_angles);
	transform.SetScale(scale);
	transform.EndTransform(true);
	
	ok::graphics::LayeredRenderer::instance().BeginImmediateRender();
	model->Update(0.f);
	ok::graphics::LayeredRenderer::instance().EndImmediateRender();

	transform.BeginTransform();
	transform.SetPosition(glm::vec3(0.f, 0.f, 0.f));
	transform.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	transform.SetScale(glm::vec3(1.f, 1.f, 1.f));
	transform.EndTransform(true);

	model->SetParent(model_parent);
}

void ok::ui::Model(ok::GameObject * model, glm::vec3 euler_angles_pre, glm::vec3 euler_angles_post, glm::vec3 scale, float x, float y)
{
	glm::vec2 transformed_position = o()._transform_stack.back() * glm::vec3(x, y, 1.f);
	ok::Transform* model_parent = model->GetParent();

	Transform pre_rotation;
	Transform post_rotation;

	pre_rotation.AddChild(model);

	pre_rotation.BeginTransform();
	pre_rotation.SetRotation(euler_angles_pre);
	pre_rotation.EndTransform(true);

	post_rotation.AddChild(&pre_rotation);

	post_rotation.BeginTransform();
	post_rotation.SetPosition(glm::vec3(transformed_position, 0.f));
	post_rotation.SetScale(scale);
	post_rotation.SetRotation(euler_angles_post);
	post_rotation.EndTransform(true);

	ok::graphics::LayeredRenderer::instance().BeginImmediateRender();
	{
		model->Update(0.f);
	}
	ok::graphics::LayeredRenderer::instance().EndImmediateRender();

	post_rotation.BeginTransform();
	post_rotation.SetPosition(glm::vec3(0.f, 0.f, 0.f));
	post_rotation.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	post_rotation.SetScale(glm::vec3(1.f, 1.f, 1.f));
	post_rotation.EndTransform(true);

	pre_rotation.SetParent(nullptr);
	pre_rotation.BeginTransform();
	pre_rotation.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	pre_rotation.EndTransform(true);

	model->SetParent(model_parent);
}

void ok::ui::Model(ok::GameObject * model, glm::vec3 euler_angles_pre, glm::vec3 euler_angles, glm::vec3 euler_angles_post, glm::vec3 scale, float x, float y)
{
	glm::vec2 transformed_position = o()._transform_stack.back() * glm::vec3(x, y, 1.f);
	ok::Transform* model_parent = model->GetParent();

	Transform pre_rotation;
	Transform rotation;
	Transform post_rotation;

	pre_rotation.AddChild(model);

	pre_rotation.BeginTransform();
	pre_rotation.SetRotation(euler_angles_pre);
	pre_rotation.EndTransform(true);

	rotation.AddChild(&pre_rotation);

	rotation.BeginTransform();
	rotation.SetRotation(euler_angles);
	rotation.EndTransform(true);

	post_rotation.AddChild(&rotation);

	post_rotation.BeginTransform();
	post_rotation.SetPosition(glm::vec3(transformed_position, 0.f));
	post_rotation.SetScale(scale);
	post_rotation.SetRotation(euler_angles_post);
	post_rotation.EndTransform(true);

	ok::graphics::LayeredRenderer::instance().BeginImmediateRender();
	{
		model->Update(0.f);
	}
	ok::graphics::LayeredRenderer::instance().EndImmediateRender();

	post_rotation.BeginTransform();
	post_rotation.SetPosition(glm::vec3(0.f, 0.f, 0.f));
	post_rotation.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	post_rotation.SetScale(glm::vec3(1.f, 1.f, 1.f));
	post_rotation.EndTransform(true);

	rotation.SetParent(nullptr);
	rotation.BeginTransform();
	rotation.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	rotation.EndTransform(true);

	pre_rotation.SetParent(nullptr);
	pre_rotation.BeginTransform();
	pre_rotation.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	pre_rotation.EndTransform(true);

	model->SetParent(model_parent);
}

ok::ui::widget_state & ok::ui::Dummy(ok::ui::widget_ptr widget, float x, float y, float width, float height)
{
	o()._fill_widget_state(widget, x, y, width, height);

	return o()._widget_state;
}

ok::ui::widget_state & ok::ui::ScrollHorizontal(
	ok::ui::widget_ptr widget, float x, float y, float width, float height,
	int items_visible_count, int items_total,
	int & out__items_visible_first_index,
	float & out__scroll_button_relative_position,
	float & out__scroll_button_relative_size)
{
	//scroll dolgen diskretno perekluchat, seichas ne diskretno i ne poluchitsa vse posmotret
	out__scroll_button_relative_size = glm::max(8.f, width * (static_cast<float>(items_visible_count) / static_cast<float>(items_total)));

	o()._fill_widget_state(widget, x + out__scroll_button_relative_position, y, out__scroll_button_relative_size, height);

	if (ws().on_activate)
	{
		o()._mem_xy = ws().mouse_relative_pos;
	}

	if (ws().mouse_down)
	{
		o()._fill_widget_state(widget, x, y, width, height);
		out__scroll_button_relative_position = glm::clamp(ws().mouse_relative_pos.x - o()._mem_xy.x, 0.f, width - out__scroll_button_relative_size);
	}

	if (width == out__scroll_button_relative_size) out__items_visible_first_index = 0;
	else
	out__items_visible_first_index = static_cast<int>(glm::floor((out__scroll_button_relative_position / (width - out__scroll_button_relative_size)) * static_cast<float>(items_total- items_visible_count)));

	//always keep scroll button inside of bounds
	if (out__scroll_button_relative_position + out__scroll_button_relative_size > x + width)
	{
		out__scroll_button_relative_position += (x + width) - (out__scroll_button_relative_position + out__scroll_button_relative_size);
	}

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

void ok::ui::PushEffect_Fade(float power)
{
	o()._batch->BatchEnd();
	o()._batch->BatchBegin();

	o()._effects_fade.push_back(power);
}

void ok::ui::PopEffect_Fade()
{
	o()._batch->BatchEnd();
	o()._batch->BatchBegin();

	o()._effects_fade.pop_back();
}

ok::ui::widget_state & ok::ui::ws()
{
	return o()._widget_state;
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
		else if (*callback_name_ptr == "effects_fade")
			return glm::clamp(1.0f - _effects_fade.back(), 0.f, 1.f);
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
	_widget_state.mouse_relative_pos = glm::clamp(local_mouse - glm::vec2(left, top), glm::vec2(0.f), glm::vec2(width, height));

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
