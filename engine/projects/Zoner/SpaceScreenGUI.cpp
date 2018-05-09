#include "SpaceScreenGUI.h"

void Zoner::SpaceScreenGUI::Update(float dt)
{
	if (o()._icons_cache_64px == nullptr)
	{
		o()._icons_cache_64px = new ok::graphics::RenderTarget(512, 512, true, true, false, true);
		o()._icons_cache_64px_tex = new ok::graphics::Texture(o()._icons_cache_64px);
	}

	Update_Inventory(dt);
}

void Zoner::SpaceScreenGUI::Update_Inventory(float dt)
{
	ok::Input::o().SetCurrentLayer(1);
	

	static ok::ui::widget inventory_panel_top;
	static ok::ui::widget inventory_panel_top_button_close;
	static ok::ui::widget inventory_panel_top_button_eject;


	struct inventory_panel_top_scroll_struct
	{
		ok::ui::widget widget;
		int items_visible_first_index;
		float scroll_button_relative_position;
		float scroll_button_relative_size;
	} static inventory_panel_top_scroll;


	static ok::graphics::SpriteAtlas* atlas_ui = nullptr;
	static ok::graphics::SpriteInfo spr_inventory_top_body_shadow;
	static ok::graphics::SpriteInfo spr_inventory_top_body;
	static ok::graphics::SpriteInfo spr_inventory_top_big_light;
	static ok::graphics::SpriteInfo spr_inventory_top_small_light;
	static ok::graphics::SpriteInfo spr_inventory_top_btn_close;
	static ok::graphics::SpriteInfo spr_inventory_top_btn_eject;

	static ok::graphics::SpriteInfo spr_inventory_top_scroll_back;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_l;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_m;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_r;

	static ok::graphics::SpriteInfo spr_inventory_top_knob_glow_l;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_glow_m;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_glow_r;

	if (atlas_ui == nullptr)
	{
		atlas_ui = Zoner::IGame::o().GetSpriteAtlases()["ui"];
		spr_inventory_top_body_shadow = atlas_ui->Get(ok::String("inventory_top_body_shadow"));
		spr_inventory_top_body = atlas_ui->Get(ok::String("inventory_top_body"));

		spr_inventory_top_body_shadow.scale.x = 2.f;
		spr_inventory_top_body_shadow.scale.y = 2.f;

		spr_inventory_top_big_light = atlas_ui->Get(ok::String("inventory_top_big_light"));
		spr_inventory_top_big_light.tint_color = ok::Color(255,162,0,255);
		spr_inventory_top_big_light.tint_power = 1.f;

		spr_inventory_top_small_light = atlas_ui->Get(ok::String("inventory_top_small_light"));

		spr_inventory_top_btn_close = atlas_ui->Get(ok::String("inventory_top_btn_close"));
		spr_inventory_top_btn_eject = atlas_ui->Get(ok::String("inventory_top_btn_eject"));

		spr_inventory_top_btn_close.hotspot = glm::vec2(0.f, 0.f);
		spr_inventory_top_btn_eject.hotspot = glm::vec2(0.f, 0.f);

		spr_inventory_top_scroll_back = atlas_ui->Get(ok::String("inventory_top_scroll_back"));
		spr_inventory_top_knob_l = atlas_ui->Get(ok::String("inventory_top_knob_l"));
		spr_inventory_top_knob_m = atlas_ui->Get(ok::String("inventory_top_knob_m"));
		spr_inventory_top_knob_r = atlas_ui->Get(ok::String("inventory_top_knob_r"));
		spr_inventory_top_knob_glow_l = atlas_ui->Get(ok::String("inventory_top_knob_glow_l"));
		spr_inventory_top_knob_glow_m = atlas_ui->Get(ok::String("inventory_top_knob_glow_m"));
		spr_inventory_top_knob_glow_r = atlas_ui->Get(ok::String("inventory_top_knob_glow_r"));

		spr_inventory_top_scroll_back.hotspot = glm::vec2(0.f, 0.f);
		spr_inventory_top_knob_l.hotspot = glm::vec2(0.f, 0.f);
		spr_inventory_top_knob_m.hotspot = glm::vec2(0.f, 0.f);
		spr_inventory_top_knob_r.hotspot = glm::vec2(0.f, 0.f);

		spr_inventory_top_knob_glow_l.hotspot = glm::vec2(1.f, 0.5f);
		spr_inventory_top_knob_glow_m.hotspot = glm::vec2(0.f, 0.5f);
		spr_inventory_top_knob_glow_r.hotspot = glm::vec2(0.f, 0.5f);

		inventory_panel_top_scroll.items_visible_first_index = 0;
		inventory_panel_top_scroll.scroll_button_relative_position = 0.f;
		inventory_panel_top_scroll.scroll_button_relative_size = 0.f;

		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(114, 38, 107, 92));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(202, 26, 40, 40));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(22, 172, 40, 40));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(54, 119, 232, 88));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(35, 129, 266, 49));
	}

	ok::ui::BeginUI(Zoner::IGame::o().GetScreenWidth(), Zoner::IGame::o().GetScreenHeight());

	ok::ui::PushTranslate(0.f, 0.f);
	{
		ok::ui::PushTranslate(177.f, 121.f);
		{
			//shadow
			ok::ui::Image(inventory_panel_top.ptr(), &spr_inventory_top_body_shadow);

			ok::ui::DisableSmooth();
			{
				//top main
				ok::ui::Blit(inventory_panel_top.ptr(), &spr_inventory_top_body);

				//small lights
				ok::ui::Blit(inventory_panel_top.ptr(), &spr_inventory_top_small_light, -24, -72);
				ok::ui::Blit(inventory_panel_top.ptr(), &spr_inventory_top_small_light, -24 + 14, -72);
				ok::ui::Blit(inventory_panel_top.ptr(), &spr_inventory_top_small_light, -24 + 14 + 15, -72);
			}
			ok::ui::EnableSmooth();

			//big lights
			ok::ui::Image(inventory_panel_top.ptr(), &spr_inventory_top_big_light,-108,-15);
			ok::ui::Image(inventory_panel_top.ptr(), &spr_inventory_top_big_light, 56, -29);
		}
		ok::ui::PopTranslate();

		
		ok::ui::DisableSmooth();
		{
			//inventory close button
			ok::ui::PushTranslate(202, 27);
			if (ok::ui::Dummy(inventory_panel_top_button_close.ptr(), 0, 0, 39, 38).mouse_down)
			{
				if (ok::ui::ws().on_activate)
				{
					//close inventory panel action
				}
			}
			else
			{
				ok::ui::Blit(inventory_panel_top_button_close.ptr(), &spr_inventory_top_btn_close);
			}
			ok::ui::PopTranslate();

			//inventory eject button
			ok::ui::PushTranslate(23, 172);
			if (ok::ui::Dummy(inventory_panel_top_button_eject.ptr(), 0, 0, 39, 39).mouse_down)
			{
				if (ok::ui::ws().on_activate)
				{
					//close inventory panel action
				}
			}
			else
			{
				ok::ui::Blit(inventory_panel_top_button_eject.ptr(), &spr_inventory_top_btn_eject);
			}
			ok::ui::PopTranslate();

			//scroll
			ok::ui::PushTranslate(67, 184);
			{
				ok::ui::PushTranslate(1, 1);
				{
					ok::ui::ScrollHorizontal(
						inventory_panel_top_scroll.widget.ptr(),
						0, 0, 197, 10,
						6, 12,
						inventory_panel_top_scroll.items_visible_first_index,
						inventory_panel_top_scroll.scroll_button_relative_position,
						inventory_panel_top_scroll.scroll_button_relative_size
					);

					ok::ui::PushNonActivable(true);
					{
						//knob glow begin
						ok::ui::Blit(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_glow_l,
							inventory_panel_top_scroll.scroll_button_relative_position +
								spr_inventory_top_knob_l.rect.width,
							5
						);

						ok::ui::Blit(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_glow_r,
							inventory_panel_top_scroll.scroll_button_relative_position + 
								glm::floor(inventory_panel_top_scroll.scroll_button_relative_size) - 
								spr_inventory_top_knob_r.rect.width,
							5
						);

						ok::ui::Image(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_glow_m,
							glm::floor(inventory_panel_top_scroll.scroll_button_relative_position) + spr_inventory_top_knob_l.rect.width,
							5,
							inventory_panel_top_scroll.scroll_button_relative_size - spr_inventory_top_knob_l.rect.width - spr_inventory_top_knob_r.rect.width - 0.5f
						);
						//knob glow end

						//scroll background
						ok::ui::Blit(inventory_panel_top_scroll.widget.ptr(), &spr_inventory_top_scroll_back, -1, -1);

						//knob begin
						ok::ui::Blit(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_l,
							inventory_panel_top_scroll.scroll_button_relative_position,
							1
						);

						ok::ui::Blit(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_r,
							inventory_panel_top_scroll.scroll_button_relative_position + glm::floor(inventory_panel_top_scroll.scroll_button_relative_size) - spr_inventory_top_knob_r.rect.width,
							1
						);

						ok::ui::Image(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_m,
							glm::floor(inventory_panel_top_scroll.scroll_button_relative_position) + spr_inventory_top_knob_l.rect.width - 1,
							1,
							inventory_panel_top_scroll.scroll_button_relative_size - spr_inventory_top_knob_l.rect.width - spr_inventory_top_knob_r.rect.width + 2
						);
						//knob end
					}
					ok::ui::PopNonActivable();
				}
				
				ok::ui::PopTranslate();
			}
			ok::ui::PopTranslate();
		}
		ok::ui::EnableSmooth();
		
	}
	ok::ui::PopTranslate();

	//test
	Zoner::ShipBlueprint* blueprint = Zoner::IGame::o().GetShipBlueprints()["nomad"];
	o()._CacheIcon(blueprint, 0, 0);

	ok::ui::widget w;
	ok::ui::PushTranslate(0, 0);
	ok::ui::Image(w.ptr(), &o()._GetIconCache(0, 0));
	//ok::ui::Image(w.ptr(), o()._icons_cache_64px_tex);
	ok::ui::PopTranslate();

	ok::ui::EndUI();

	ok::Input::o().SetCurrentLayer(0);
}

Zoner::SpaceScreenGUI & Zoner::SpaceScreenGUI::instance()
{
	static Zoner::SpaceScreenGUI s;
	return s;
}

Zoner::SpaceScreenGUI & Zoner::SpaceScreenGUI::o()
{
	return instance();
}

void Zoner::SpaceScreenGUI::_CacheIcon(ok::GameObject * blueprint, int slot_x, int slot_y)
{
	const glm::vec3 blueprint_rotation(90.f - 30.f, -41.5f, -20.18f);

	_icons_cache_64px->BindTarget();

	ok::graphics::Camera camera(ok::graphics::CameraCoordinateSystem::Cartesian);
	camera.SetProjectionOrtho(512.f, 512.f, 1.f, 1000.f);

	camera.BeginTransform();
	camera.SetPosition(glm::vec3(0.f, 0.f, -500.0f));
	camera.EndTransform(false);

	ok::Transform _container;

	_container.AddChild(blueprint);

	_container.BeginTransform();
	_container.SetRotation(blueprint_rotation);
	_container.EndTransform(true);

	glm::vec4 bounds;
	_CalculateBlueprintBounds(blueprint, bounds);

	bounds.x -= 2.f;
	bounds.y -= 2.f;
	bounds.z += 2.f;
	bounds.w += 2.f;

	glm::vec3 bounder_center((bounds.x + bounds.z)*0.5f, (bounds.y + bounds.w)*0.5f, 0.f);

	float scale = 64.f / glm::max(bounds.z - bounds.x, bounds.w - bounds.y);


	_container.BeginTransform();
		_container.SetScale(glm::vec3(1.f, 1.f, 1.f) * scale);
		_container.SetPosition(-bounder_center*scale+glm::vec3(static_cast<float>(slot_x) * 64.f + 32.f, static_cast<float>(slot_y) * 64.f + 32.f, 0.f));
	_container.EndTransform(true);

	ok::graphics::Camera::PushCamera(&camera);
	camera.BeginScissorTest(slot_x * 64, slot_y * 64, 64, 64);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glDepthMask(GL_TRUE);
	glClearDepth(0.f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ok::graphics::LayeredRenderer::instance().BeginImmediateRender();
	blueprint->Update(0.f);
	ok::graphics::LayeredRenderer::instance().EndImmediateRender();

	camera.EndScissorTest();
	ok::graphics::Camera::PopCamera();

	_container.AddChild(blueprint);

	_container.BeginTransform();
	_container.SetScale(glm::vec3(1.f, 1.f, 1.f));
	_container.SetPosition(glm::vec3(0.f, 0.f, 0.f));
	_container.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	_container.EndTransform(true);

	_container.RemoveChild(blueprint);

	_icons_cache_64px->UnbindTarget();
}

void Zoner::SpaceScreenGUI::_CalculateBlueprintBounds(ok::Transform * part, glm::vec4 & bounds)
{
	ok::graphics::MeshRenderer* mr = part->gameObject().GetComponent<ok::graphics::MeshRenderer>();

	if (mr != nullptr)
	{
		GLBufferInfo buf_info = mr->GetMesh()->GetVertices();

		struct f3
		{
			float x, y, z;
		};

		f3* positions = (f3*)(buf_info.vertices);

		glm::vec4 part_axis4;
		glm::vec3 part_axis;

		part_axis4.w = 1.f; //if = 0 then translation wont work, so we use 1.f because we need translation

		for (int i = 0; i < buf_info.vertex_count; i++)
		{
			part_axis4.x = positions->x;
			part_axis4.y = positions->y;
			part_axis4.z = positions->z;

			part_axis = glm::vec3(part->GetAbsoluteTransformMatrix() * part_axis4);

			bounds.x = glm::min(bounds.x, glm::dot(part_axis, glm::vec3(1.f, 0.f, 0.f)));
			bounds.y = glm::min(bounds.y, glm::dot(part_axis, glm::vec3(0.f, 1.f, 0.f)));

			bounds.z = glm::max(bounds.z, glm::dot(part_axis, glm::vec3(1.f, 0.f, 0.f)));
			bounds.w = glm::max(bounds.w, glm::dot(part_axis, glm::vec3(0.f, 1.f, 0.f)));

			positions++;
		}
	}
	else
	{
		glm::vec3 part_axis;

		part->BeginTransform(ok::TransformSpace::WorldSpace);
		part_axis = part->GetPosition();
		part->EndTransform(false);

		bounds.x = glm::min(bounds.x, glm::dot(part_axis, glm::vec3(1.f, 0.f, 0.f)));
		bounds.y = glm::min(bounds.y, glm::dot(part_axis, glm::vec3(0.f, 1.f, 0.f)));

		bounds.z = glm::max(bounds.z, glm::dot(part_axis, glm::vec3(1.f, 0.f, 0.f)));
		bounds.w = glm::max(bounds.w, glm::dot(part_axis, glm::vec3(0.f, 1.f, 0.f)));
	}

	std::list<ok::Transform*>& childrens = part->GetChildrens();
	for (auto& child : childrens)
	{
		_CalculateBlueprintBounds(child, bounds);
	}
}

ok::graphics::SpriteInfo Zoner::SpaceScreenGUI::_GetIconCache(int slot_x, int slot_y)
{
	ok::graphics::SpriteInfo sprite;

	sprite.rect = ok::graphics::TextureRect(_icons_cache_64px_tex, slot_x * 64, slot_y * 64, 64, 64);
	sprite.hotspot.x = 0.f;
	sprite.hotspot.y = 0.f;
	
	sprite.scale.x = 0.5f;
	sprite.scale.y = 0.5f;

	return sprite;
}
