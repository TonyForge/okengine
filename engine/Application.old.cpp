#include "Application.h"
#include "Automator.h"

ok::Application::Application()
{
}

float sh1 = 0;
float sh2 = 0;
float sh3 = 0;
float sh4 = 0;
float sh5 = 0;
float camera_shift_x = 0;
float camera_shift_y = 0;
float camera_shift_z = -500;

float near_plane_z = 1.0f;
float far_plane_z = 1000.0f;


void ok::Application::Run()
{
	LoadSettings();

	sf::ContextSettings contextSettings;
	contextSettings.antialiasingLevel = antialiasing;
	contextSettings.depthBits = screen_depth_bits;
	contextSettings.stencilBits = screen_stencil_bits;
	//OpenGL 4.0 (was 3.3)
	contextSettings.majorVersion = 4;
	contextSettings.minorVersion = 0;
	contextSettings.attributeFlags = sf::ContextSettings::Core;
	contextSettings.sRgbCapable = false;

	sf::Uint32 style;
	if (fullscreen)
		style = sf::Style::Fullscreen;
	else
		style = sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close;

	window = new sf::Window(sf::VideoMode(screen_width, screen_height, screen_depth_bits), window_title, style, contextSettings);

	window->setVerticalSyncEnabled(vsync);
	window->setActive(true);
	glewInit();

	glViewport(0, 0, window_width, window_height);

	ok::graphics::Camera::_viewport_x = 0;
	ok::graphics::Camera::_viewport_y = 0;
	ok::graphics::Camera::_viewport_w = static_cast<int>(window_width);
	ok::graphics::Camera::_viewport_h = static_cast<int>(window_height);

	ok::graphics::SpriteBatch* fixed_resolution_batch = nullptr;

	if (fixed_resolution)
	{
		fixed_resolution_framebuffer = new ok::graphics::RenderTarget(screen_width, screen_height, true, true, true, true);
		fixed_resolution_batch = new ok::graphics::SpriteBatch(1);
	}
	else fixed_resolution_framebuffer = nullptr;


	/*******************/
	ok::graphics::Font* font = ok::Assets::instance().GetFont("font001");
	font->SetInternalFont(ok::Assets::instance().GetInternalFont("consola.font.xml"));

	font->SetBrushColor(ok::Color(.5f, 0.75f, 0.25f, 0.5f));

	ok::graphics::TextBatch2D text_batch(screen_width, screen_height);
	text_batch.SetBrushFont(font);
	//********************
	ok::graphics::Shader* shader = ok::Assets::instance().GetShader("shader.xml");

	bool running = true;
	
	ok::graphics::TextureRect texrect;
	texrect.texture = ok::Assets::instance().GetTexture("test.png");
	texrect.left = 0;
	texrect.top = 0;
	texrect.width = 100;
	texrect.height = 100;
	texrect.uv_rect = glm::vec4(0.f, 0.f, 1.f, 1.f);

	ok::graphics::SpriteBatch batch(10);
	ok::graphics::Camera cam(ok::graphics::CameraCoordinateSystem::ScreenCenter);
	//ok::graphics::Camera cam(ok::graphics::CameraCoordinateSystem::ScreenCenter);

	//cam.SetProjectionPersp((float)(screen_width), (float)(screen_height), 45.0f, near_plane_z, far_plane_z);
	//cam.SetProjectionOrtho((float)(screen_width), (float)(screen_height), near_plane_z, far_plane_z);

	ok::GameObject teapot_go;
	ok::graphics::MeshRenderer teapot_go_mr;
	teapot_go.AddComponent(&teapot_go_mr);

	teapot_go_mr.SetMesh("Teapot003.mesh", true);
	teapot_go_mr.SetMaterial("Cross.Material.xml");
	teapot_go_mr.SetLayer(0);
	teapot_go_mr.SetStage(ok::graphics::LayeredRendererStage::Stage);

	ok::graphics::VertexAnimation* teapot_morph_anim = ok::Assets::instance().GetVertexAnimation("Teapot003.anim0.animation", ok::graphics::VertexAnimationInfluence::Positions_Normals);
	ok::graphics::AnimationController teapot_morph_anim_controller;
	teapot_morph_anim_controller.SetAnimation(teapot_morph_anim);
	teapot_go_mr.GetMesh()->EnableMorphDeformer(2, ok::graphics::MorphingTargetType::Positions_Normals, false);


	ok::GameObject cross_go;
	ok::graphics::MeshRenderer cross_go_mr;
	cross_go.AddComponent(&cross_go_mr);

	cross_go_mr.SetMesh("Cross.mesh", true);
	cross_go_mr.SetMaterial("Cross.Material.xml");
	cross_go_mr.SetLayer(0);
	cross_go_mr.SetStage(ok::graphics::LayeredRendererStage::Stage);

	ok::GameObject go;
	ok::graphics::MeshRenderer go_mr;
	go.AddComponent(&go_mr);

	ok::GameObject* skeleton = ok::Assets::instance().GetSkeleton("Teapot001.skeleton", false);//("Teapot001.skeleton", false);

	ok::graphics::TransformAnimation* skeleton_anim = ok::Assets::instance().GetTransformAnimation("Teapot001.anim0.animation");//("Teapot001.anim0.animation");
	ok::graphics::VertexAnimation* morph_anim = ok::Assets::instance().GetVertexAnimation("Teapot002.anim0.animation", ok::graphics::VertexAnimationInfluence::Positions_Normals);

	ok::graphics::AnimationController skeleton_anim_controller;
	skeleton_anim_controller.SetAnimation(skeleton_anim);
	ok::graphics::AnimationController morph_anim_controller;
	morph_anim_controller.SetAnimation(morph_anim);

	std::vector<float> skeleton_bones_absolute_transforms;
	skeleton_bones_absolute_transforms.resize(16 * skeleton->GetComponent<ok::graphics::TransformAnimationSkeleton>()->GetBones().size());

	go_mr.SetMesh("Teapot001.mesh", true);//("Teapot001.mesh", false);
	go_mr.SetMaterial("Teapot001.Material.xml");
	go_mr.SetLayer(0);
	go_mr.SetStage(ok::graphics::LayeredRendererStage::Stage);
	go_mr.GetMesh()->EnableMorphDeformer(4, ok::graphics::MorphingTargetType::Positions_Normals, true);

	std::vector<int>* skin_mapping_table = ok::Assets::instance().GetMeshToSkeletonMappingTable("Teapot001.mesh", "Teapot001.skeleton");//("Teapot001.mesh", "Teapot001.skeleton");

	/*ok::GameObject go;
	ok::graphics::MeshRenderer go_mr;
	go.AddComponent(&go_mr);

	go.GetComponent<ok::graphics::MeshRenderer>()->SetMesh("Teapot001.mesh");
	go.GetComponent<ok::graphics::MeshRenderer>()->SetMaterial("Box001.Material.xml");
	go.GetComponent<ok::graphics::MeshRenderer>()->SetLayer(0);
	go.GetComponent<ok::graphics::MeshRenderer>()->SetStage(ok::graphics::LayeredRendererStage::Stage);

	ok::graphics::VertexAnimation* anim = ok::Assets::instance().GetVertexAnimation("Teapot001.anim0.animation", ok::graphics::VertexAnimationInfluence::Positions_Normals);
	ok::graphics::AnimationController controller;
	controller.SetAnimation(anim);
	go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->EnableMorphDeformer(4, ok::graphics::MorphingTargetType::Positions_Normals, true);

	//skin test
	ok::GameObject* skeleton = ok::Assets::instance().GetSkeleton("Object.skeleton", false);
	ok::graphics::TransformAnimation* skeleton_anim = ok::Assets::instance().GetTransformAnimation("Object.anim0.animation");
	ok::graphics::AnimationController skeleton_anim_controller;
	skeleton_anim_controller.SetAnimation(skeleton_anim);

	std::vector<float> skeleton_bones_absolute_transforms;
	skeleton_bones_absolute_transforms.resize(16 * skeleton->GetComponent<ok::TransformAnimationSkeleton>()->GetBones().size());

	ok::GameObject go_skin;
	ok::graphics::MeshRenderer go_skin_mr;
	go_skin.AddComponent(&go_skin_mr);

	go_skin.GetComponent<ok::graphics::MeshRenderer>()->SetMesh("Object.mesh", true);
	go_skin.GetComponent<ok::graphics::MeshRenderer>()->SetMaterial("Box001Skin.Material.xml");
	go_skin.GetComponent<ok::graphics::MeshRenderer>()->SetLayer(0);
	go_skin.GetComponent<ok::graphics::MeshRenderer>()->SetStage(ok::graphics::LayeredRendererStage::Stage);

	std::vector<int>* go_skin_mapping_table = ok::Assets::instance().GetMeshToSkeletonMappingTable("Object.mesh", "Object.skeleton");*/

	//go_skin.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->EnableSkinDeformer(false);
	//go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->

/*	ok::graphics::Mesh* mesh = ok::Assets::instance().GetMesh("Box001.mesh");
	ok::graphics::Shader* mesh_shader = ok::Assets::instance().GetShader("FlatShader.xml");
	ok::graphics::Texture* mesh_tex = ok::Assets::instance().GetTexture("test.png");*/
	/*cam.BeginTransform(ok::TransformSpace::LocalSpace);
	cam.SetPosition(glm::vec3(1989.f, 0.f, 0.f));
	cam.EndTransform();
	*/

	/*ok::Automator automator;
	ok::Automator* pAutomator = &automator;
	ok::graphics::Camera* pCamera = &cam;

	automator.AssignChannel(0, [pAutomator, pCamera](std::vector<int> & curves_indexes) mutable {
		pCamera->SetPosition(pAutomator->Curve(curves_indexes[0]));
	});

	glm::vec3 v1(1.f, 1.f, 1.f);
	ok::AutomatorCurve curve1(ok::AutomatorCurveFunction::EXTERNAL_VARIABLE);
	curve1.external_variable = &v1;
	automator.AssignCurve(0, &curve1);
	automator.Connect(0, std::vector<int>({ 0 }));

	automator.Update(0.1f);*/

	/*ok::AutomatorCurve curve;
	ok::AutomatorCurve* pCurve = &curve;
	curve._type = ok::AutomatorCurveType::Function;
	curve._variable = new float(0.5f);

	curve._function = [pCurve, cam]() mutable {
		cam.BeginTransform(ok::TransformSpace::LocalSpace);
		*((float*)pCurve->_variable) = cam.GetPosition().x;
		cam.EndTransform();
	};

	float tmp1 = curve;*/
	//batch.Draw()

	//ok::AutomatorCurve curve;
	//curve.
	//ok::AutomatorChannelType::
	//********************

	float viewport_x = 0.f, viewport_y = 0.f, viewport_w = (float)window_width, viewport_h = (float)window_height;

	while (running)
	{
		sf::Event e;

		while (window->pollEvent(e))
		{
			switch (e.type)
			{
				case sf::Event::Closed :
				{
					running = false;
				}
				break;
				case sf::Event::Resized :
				{
					window_width = e.size.width;
					window_height = e.size.height;

					if (keep_aspect_ratio)
					{
						float screen_to_window_scale = glm::min(static_cast<float>(window_width) / static_cast<float>(screen_width), static_cast<float>(window_height) / static_cast<float>(screen_height));

						viewport_w = static_cast<float>(screen_width) * screen_to_window_scale;
						viewport_h = static_cast<float>(screen_height) * screen_to_window_scale;

						viewport_x = (static_cast<float>(window_width) - viewport_w) * 0.5f;
						viewport_y = (static_cast<float>(window_height) - viewport_h) * 0.5f;

						glViewport((GLint)viewport_x, (GLint)viewport_y, (GLint)viewport_w, (GLint)viewport_h);

						ok::graphics::Camera::_viewport_x = static_cast<int>(viewport_x);
						ok::graphics::Camera::_viewport_y = static_cast<int>(viewport_y);
						ok::graphics::Camera::_viewport_w = static_cast<int>(viewport_w);
						ok::graphics::Camera::_viewport_h = static_cast<int>(viewport_h);
					}
					else
					{
						glViewport((GLint)0, (GLint)0, (GLint)window_width, (GLint)window_height);

						ok::graphics::Camera::_viewport_x = 0;
						ok::graphics::Camera::_viewport_y = 0;
						ok::graphics::Camera::_viewport_w = static_cast<int>(window_width);
						ok::graphics::Camera::_viewport_h = static_cast<int>(window_height);
					}
				}
				break;
				case sf::Event::KeyPressed :
				{
					if (e.key.code != sf::Keyboard::Unknown)
						ok::Input::instance().OnKeyDown(static_cast<int>(e.key.code));
				}
				break;
				case sf::Event::KeyReleased :
				{
					if (e.key.code != sf::Keyboard::Unknown)
						ok::Input::instance().OnKeyUp(static_cast<int>(e.key.code));
				}
				break;
				case sf::Event::MouseButtonPressed :
				{
					ok::Input::instance().OnMouseDown(static_cast<int>(e.mouseButton.button));
				}
				break;
				case sf::Event::MouseButtonReleased:
				{
					ok::Input::instance().OnMouseUp(static_cast<int>(e.mouseButton.button));
				}
				break;
				case sf::Event::MouseMoved :
				{
					ok::Input::instance().OnMouseMove((e.mouseMove.x - viewport_x) / viewport_w, (e.mouseMove.y - viewport_y) / viewport_h, (float)screen_width, (float)screen_height);
				}
				break;
			}
		}

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		if (fixed_resolution)
		{
			fixed_resolution_framebuffer->BindTarget();
			glViewport((GLint)0, (GLint)0, (GLint)screen_width, (GLint)screen_height);

			ok::graphics::Camera::_viewport_x = 0;
			ok::graphics::Camera::_viewport_y = 0;
			ok::graphics::Camera::_viewport_w = static_cast<int>(screen_width);
			ok::graphics::Camera::_viewport_h = static_cast<int>(screen_height);
		}

		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT);

		glClearDepth(0.f);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		float speed = 10.0f;
		if (ok::Input::instance().KeyDown(ok::KKey::W))
		{
			camera_shift_z += speed;
		}

		if (ok::Input::instance().KeyDown(ok::KKey::S))
		{
			camera_shift_z -= speed;
		}

		if (ok::Input::instance().KeyDown(ok::KKey::D))
		{
			camera_shift_x += speed;
		}

		if (ok::Input::instance().KeyDown(ok::KKey::A))
		{
			camera_shift_x -= speed;
		}

		if (ok::Input::instance().KeyDown(ok::KKey::Q))
		{
			camera_shift_y += speed;
		}

		if (ok::Input::instance().KeyDown(ok::KKey::Z))
		{
			camera_shift_y -= speed;
		}

		if (ok::Input::instance().KeyDown(ok::KKey::F))
		{
			if (ok::Input::instance().KeyDown(ok::KKey::G))
			{
				far_plane_z += 1.0f;
			}

			if (ok::Input::instance().KeyDown(ok::KKey::B))
			{
				far_plane_z -= 1.0f;
				
			}
		}

		if (ok::Input::instance().KeyDown(ok::KKey::N))
		{
			if (ok::Input::instance().KeyDown(ok::KKey::J))
			{
				near_plane_z += 1.0f;
				


			}

			if (ok::Input::instance().KeyDown(ok::KKey::M))
			{
				near_plane_z -= 1.0f;


			}
		}

		if (far_plane_z > 2000.0f) far_plane_z = 2000.0f;
		if (far_plane_z < 0.0) far_plane_z = 0.0f;
		if (near_plane_z > far_plane_z) near_plane_z = far_plane_z;
		if (near_plane_z < 0) near_plane_z = 0.0f;

		cam.SetProjectionPersp((float)(screen_width), (float)(screen_height), 45.0f, near_plane_z, far_plane_z);
		//cam.SetProjectionOrtho((float)(screen_width), (float)(screen_height), near_plane_z, far_plane_z);

		cam.BeginTransform(ok::TransformSpace::WorldSpace);
		cam.SetPosition(glm::vec3(camera_shift_x, camera_shift_y, camera_shift_z));
		//cam.SetRotation(glm::vec3(sh1*360.0f, 0.0f, 0.0f));
		//cam.SetForward(glm::vec3(1.0f,0.0f,1.0f));
		cam.SetForward((glm::vec3(0.0f,0.0f,0.0f) - cam.GetPosition()));
		cam.EndTransform();
		

		ok::graphics::Camera::PushCamera(&cam);
		//cam.SetCurrent();
		//********************

		if (ok::Input::o().KeyDown(ok::MKey::Left))
			sh1 += 0.001f;
		if (sh1 > 1) sh1 = 0;

		//if (ok::Input::o().KeyDown(ok::MKey::Left))
		//	sh1 += 0.001f;
		//if (sh1 > 1) sh1 = 0;
		batch.BatchBegin(-1.0f);

		batch.Draw(&texrect, glm::vec2(0.f, 0.f), sh1*360.0f, glm::vec2(1.0f, 1.0f)*(1.0f+sh1), true);
		batch.Draw(&texrect, glm::vec2(100.0f*sh1, 0.f), 0.f, glm::vec2(1.0f, 1.0f)*(1.0f + 1.0f*sh1), true);
		

		batch.BatchEnd();


		if (!ok::Input::o().KeyDown(ok::KKey::LShift) && ok::Input::o().KeyDown(ok::MKey::Left))
			sh4 += 0.01f;
		if (sh4 > 1) sh4 = 1;

		if (ok::Input::o().KeyDown(ok::KKey::LShift) && ok::Input::o().KeyDown(ok::MKey::Left))
			sh4 -= 0.01f;
		if (sh4 < 0) sh4 = 0;

		if (!ok::Input::o().KeyDown(ok::KKey::LShift) && ok::Input::o().KeyDown(ok::MKey::Right))
			sh5 += 0.01f;
		if (sh5 > 1) sh5 = 1;

		if (ok::Input::o().KeyDown(ok::KKey::LShift) && ok::Input::o().KeyDown(ok::MKey::Right))
			sh5 -= 0.01f;
		if (sh5 < 0) sh5 = 0;
		
		//text_batch.SetClipRectEnabled(true);
		//text_batch.SetClipRect(ok::Rect2Df(100, 100, 120, 120));
		//text_batch.SetLineSpacingScale(1.0f);
		text_batch.SetBrushSize(180);
		text_batch.SetBrushAlignVertical(ok::graphics::TextAlign::Bottom);
		text_batch.SetBrushPosition(glm::vec2(100.f, 50.f));

		//text_batch.SetBrushColor(ok::Color(1.f, 0, 0, 1.f));
		//text_batch.SetBrushBold(0.0);
		//text_batch.SetBrushGradient(ok::Color(0.f, 1.f, 0.f, 0.0f), ok::Color(0.f, 0.f, 1.f, 0.0f));
		//text_batch.SetBrushItalic(0.0f);
		//text_batch.SetBrushOuterGlow(ok::Color(1.f, 0.5f, 0.25f, 0.0f), sh4, sh5);
		//text_batch.SetBrushInnerGlow(ok::Color(1.f, 1.0f, 1.0f, 0.0f), 0.1f, 0.3f);
		//text_batch.SetBrushItalic(sh1);
		//text_batch.SetBrushInnerShadow(ok::Color(0.f, 0.f, 0.f, 0.f), 0.1f, 0.1f, sh4, sh5);
		//text_batch.SetBrushOuterShadow(ok::Color(0.f, 0.f, 0.f, 1.0f), sh4, sh5, 0, 0);
		/*text_batch.SetBrushItalic(sh1);
		text_batch.SetBrushOuterGlow(ok::Color(1.f, 1.f, 1.f, 1.f), sh4, sh5);
		text_batch.SetBrushGradient(ok::Color(0.f, 1.f, 0.f, 1.f), ok::Color(0.f, 0.f, 1.f, 1.f));*/

		text_batch.BatchBegin();
		//text_batch.Draw(ok::String("Hello.World!\n"));
		//text_batch.Draw(ok::String("Test\n"));
		//text_batch.Draw(ok::String("1 2 3"));
		text_batch.Draw(ok::String("Hello world.\n123\nTromkaster..."));
		text_batch.BatchEnd();

		if (ok::Input::o().KeyDown(ok::MKey::Left))
			sh1 += 0.001f;
		if (sh1 > 1) sh1 = 0;

		sh2 += 0.001f;
		if (sh2 > 1) sh2 = 0;

		if (ok::Input::o().KeyDown(ok::MKey::Middle))
		sh3 += 0.01f;
		if (sh3 > 1)
			sh3 = 0;

		teapot_morph_anim_controller.GoToNormalizedTime(sh3);
		morph_anim_controller.GoToNormalizedTime(sh3);
		skeleton_anim_controller.GoToNormalizedTime(sh3);

		//morphing

		teapot_go_mr.GetMesh()->AssignMorphTarget(0, teapot_morph_anim_controller.GetFramesBlend(), 1.f,
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(teapot_morph_anim_controller.GetLeftFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(teapot_morph_anim_controller.GetLeftFrame()))->normals),
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(teapot_morph_anim_controller.GetRightFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(teapot_morph_anim_controller.GetRightFrame()))->normals)
		);

		teapot_go.BeginTransform();
		teapot_go.SetPosition(glm::vec3(0.f,30.f,0.f));
		teapot_go.EndTransform();
		teapot_go_mr.GetMesh()->ApplyDeformers();
		//teapot_go.Update(0.f);

		go_mr.GetMesh()->AssignMorphTarget(0, morph_anim_controller.GetFramesBlend(), 1.f,
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetLeftFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetLeftFrame()))->normals),
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetRightFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetRightFrame()))->normals)
		);
		/*go_mr.GetMesh()->AssignMorphTarget(2, morph_anim_controller.GetFramesBlend(), 1.f,
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetLeftFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetLeftFrame()))->normals),
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetRightFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetRightFrame()))->normals)
		);*/




		/*go_mr.GetMesh()->AssignMorphTarget(
			0,
			(1.0f - morph_anim_controller.GetFramesBlend()) * 1.0f,
			((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetLeftFrame()))->positions,
			((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetLeftFrame()))->normals
		);
		go_mr.GetMesh()->AssignMorphTarget(
			1,
			morph_anim_controller.GetFramesBlend() * 1.0f,
			((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetRightFrame()))->positions,
			((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetRightFrame()))->normals
		);
		go_mr.GetMesh()->AssignMorphTarget(
			2,
			(1.0f - morph_anim_controller.GetFramesBlend()) * 1.0f,
			((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetLeftFrame()))->positions,
			((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetLeftFrame()))->normals
		);
		go_mr.GetMesh()->AssignMorphTarget(
			3,
			morph_anim_controller.GetFramesBlend() * 1.0f,
			((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetRightFrame()))->positions,
			((ok::graphics::VertexAnimationFrame*)(morph_anim_controller.GetRightFrame()))->normals
		);*/

		//skinning
		ok::graphics::TransformAnimationFrame* skeleton_left_frame = (ok::graphics::TransformAnimationFrame*)skeleton_anim_controller.GetLeftFrame();
		ok::graphics::TransformAnimationFrame* skeleton_right_frame = (ok::graphics::TransformAnimationFrame*)skeleton_anim_controller.GetRightFrame();
		float skeleton_anim_blend = skeleton_anim_controller.GetFramesBlend();

		ok::graphics::TransformAnimationSkeleton* skeleton_skin = skeleton->GetComponent<ok::graphics::TransformAnimationSkeleton>();

		skeleton_skin->AnimateBones(skeleton_left_frame, skeleton_right_frame, skeleton_anim_blend);
		skeleton_skin->GrabBones(skeleton_bones_absolute_transforms.data());

		go_mr.GetMesh()->AssignSkinBones(skeleton_bones_absolute_transforms.data(), skin_mapping_table->data());
		go_mr.GetMesh()->ApplyDeformers();

		go.transform().BeginTransform(ok::TransformSpace::WorldSpace);
		//go.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f * sh2));
		//go.SetPosition(glm::vec3(100.0f* sh1, 0.0f, 0.f));
		//go.SetRotation(glm::vec3(360.0f * sh2, 360.0f * sh2, 360.0f * sh2));
		//go.SetScale(glm::vec3(1.00f, 1.00f, 1.00f));
		go.transform().EndTransform();


		go.Update(0.0f);

		int bone_id = 3;//3;
		skeleton_skin->GetBones()[bone_id]->BeginTransform(ok::TransformSpace::WorldSpace);
			cross_go.BeginTransform(ok::TransformSpace::WorldSpace);
				glm::vec3 pos = skeleton_skin->GetBones()[bone_id]->GetPosition();
				//pos.x *= -1;
				//pos.y *= -1;
				cross_go.SetPosition(pos);
				glm::vec3 rot = skeleton_skin->GetBones()[bone_id]->GetRotation();
				cross_go.SetRotation(rot);
			cross_go.EndTransform();
			//cross_go.Update(0.0f);
		skeleton_skin->GetBones()[bone_id]->EndTransform();
		//controller.GoToNormalizedTime(sh3);
		/*go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->AssignMorphTarget(
			0,
			(1.0f-controller.GetFramesBlend()) * 1.0f,
			((ok::graphics::VertexAnimationFrame*)(controller.GetLeftFrame()))->positions,
			((ok::graphics::VertexAnimationFrame*)(controller.GetLeftFrame()))->normals
		);
		go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->AssignMorphTarget(
			1,
			controller.GetFramesBlend() * 1.0f,
			((ok::graphics::VertexAnimationFrame*)(controller.GetRightFrame()))->positions,
			((ok::graphics::VertexAnimationFrame*)(controller.GetRightFrame()))->normals
		);
		go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->AssignMorphTarget(
			2,
			(1.0f - controller.GetFramesBlend()) * 1.0f,
			((ok::graphics::VertexAnimationFrame*)(controller.GetLeftFrame()))->positions,
			((ok::graphics::VertexAnimationFrame*)(controller.GetLeftFrame()))->normals
		);
		go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->AssignMorphTarget(
			3,
			controller.GetFramesBlend() * 1.0f,
			((ok::graphics::VertexAnimationFrame*)(controller.GetRightFrame()))->positions,
			((ok::graphics::VertexAnimationFrame*)(controller.GetRightFrame()))->normals
		);*/

		/*go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->AssignMorphTarget(0, controller.GetFramesBlend(), 1.f,
			std::pair<float*,float*>(((ok::graphics::VertexAnimationFrame*)(controller.GetLeftFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(controller.GetLeftFrame()))->normals),
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(controller.GetRightFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(controller.GetRightFrame()))->normals)
			);
		go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->AssignMorphTarget(2, controller.GetFramesBlend(), 1.f,
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(controller.GetLeftFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(controller.GetLeftFrame()))->normals),
			std::pair<float*, float*>(((ok::graphics::VertexAnimationFrame*)(controller.GetRightFrame()))->positions, ((ok::graphics::VertexAnimationFrame*)(controller.GetRightFrame()))->normals)
		);

		go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->_ApplyMorphDeformer();
		//go.GetComponent<ok::graphics::MeshRenderer>()->GetMesh()->AssignMorphTarget(0, anim->)

		go.transform().BeginTransform(ok::TransformSpace::WorldSpace);
		//go.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f * sh2));
		//go.SetRotation(glm::vec3(360.0f * sh2, 360.0f * sh2, 360.0f * sh2));
		go.SetScale(glm::vec3(10.01f, 10.01f, 10.01f));
		go.transform().EndTransform();
		//go_mr.Render();
		go.Update(0.0f);

		//------------
		skeleton_anim_controller.GoToNormalizedTime(sh3);
		ok::graphics::TransformAnimationFrame* skeleton_left_frame = (ok::graphics::TransformAnimationFrame*)skeleton_anim_controller.GetLeftFrame();
		ok::graphics::TransformAnimationFrame* skeleton_right_frame = (ok::graphics::TransformAnimationFrame*)skeleton_anim_controller.GetRightFrame();
		float skeleton_anim_blend = skeleton_anim_controller.GetFramesBlend();

		ok::TransformAnimationSkeleton* skeleton_skin = skeleton->GetComponent<ok::TransformAnimationSkeleton>();
		std::vector<ok::GameObject*> skeleton_bones = skeleton_skin->GetBones();
		std::vector<float>& skeleton_bones_bind_pose_trs = skeleton_skin->GetBindPose();

		int skeleton_bone_index = 0;
		for (auto&& bone : skeleton_bones)
		{
			bone->BeginTransform();
			bone->SetPosition(
				glm::vec3(
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 0],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 1],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 2]
				) * (1.f - skeleton_anim_blend) +
				glm::vec3(
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 0],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 1],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 2]
				) * (skeleton_anim_blend) - 
				glm::vec3(
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 0],
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 1],
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 2]
				)
			);
			bone->SetRotation(
				glm::vec3(
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 3],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 4],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 5]
				) * (1.f - skeleton_anim_blend) +
				glm::vec3(
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 3],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 4],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 5]
				) * (skeleton_anim_blend) -
				glm::vec3(
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 3],
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 4],
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 5]
				)
			);
			bone->SetScale(
				(glm::vec3(
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 6],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 7],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 8]
				) * (1.f - skeleton_anim_blend) +
				glm::vec3(
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 6],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 7],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 8]
				) * (skeleton_anim_blend)) /
				glm::vec3(
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 6],
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 7],
					skeleton_bones_bind_pose_trs[skeleton_bone_index * 9 + 8]
				)
			);
			bone->EndTransform();

			skeleton_bone_index++;
		}

		skeleton_bone_index = 0;

		for (auto&& bone : skeleton_bones)
		{
			memcpy(&skeleton_bones_absolute_transforms[skeleton_bone_index * 16], glm::value_ptr(bone->GetAbsoluteTransformMatrix()), sizeof(float) * 16);
			skeleton_bone_index++;
		}

		skeleton_bone_index = 0;
		for (auto&& bone : skeleton_bones)
		{
			bone->BeginTransform();
			bone->SetPosition(
				glm::vec3(
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 0],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 1],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 2]
				) * (1.f - skeleton_anim_blend) +
				glm::vec3(
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 0],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 1],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 2]
				) * (skeleton_anim_blend)
			);
			bone->SetRotation(
				glm::vec3(
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 3],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 4],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 5]
				) * (1.f - skeleton_anim_blend) +
				glm::vec3(
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 3],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 4],
					skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 5]
				) * (skeleton_anim_blend)
			);
			bone->SetScale(
				(glm::vec3(
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 6],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 7],
					skeleton_left_frame->trs_frames[skeleton_bone_index * 9 + 8]
				) * (1.f - skeleton_anim_blend) +
					glm::vec3(
						skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 6],
						skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 7],
						skeleton_right_frame->trs_frames[skeleton_bone_index * 9 + 8]
					) * (skeleton_anim_blend))
			);
			bone->EndTransform();

			skeleton_bone_index++;
		}

		go_skin_mr.GetMesh()->AssignSkinBones(skeleton_bones_absolute_transforms.data(), go_skin_mapping_table->data());
		go_skin_mr.GetMesh()->_ApplySkinDeformer();

		go_skin.Update(0.0f);*/

		ok::graphics::LayeredRenderer::instance().Flush();

		
		/*mesh_shader->Bind(&batch);
		mesh_shader->BindTexture(mesh_tex, 0);
		mesh->Bind();

		glDrawElements(GL_TRIANGLES, mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, 0);
		mesh->Unbind();*/
		//********************

		if (fixed_resolution)
		{
			fixed_resolution_framebuffer->UnbindTarget();

			if (keep_aspect_ratio)
			{
				glViewport((GLint)viewport_x, (GLint)viewport_y, (GLint)viewport_w, (GLint)viewport_h);

				ok::graphics::Camera::_viewport_x = static_cast<int>(viewport_x);
				ok::graphics::Camera::_viewport_y = static_cast<int>(viewport_y);
				ok::graphics::Camera::_viewport_w = static_cast<int>(viewport_w);
				ok::graphics::Camera::_viewport_h = static_cast<int>(viewport_h);
			}
			else
			{
				glViewport((GLint)0, (GLint)0, (GLint)window_width, (GLint)window_height);

				ok::graphics::Camera::_viewport_x = 0;
				ok::graphics::Camera::_viewport_y = 0;
				ok::graphics::Camera::_viewport_w = static_cast<int>(window_width);
				ok::graphics::Camera::_viewport_h = static_cast<int>(window_height);
			}

			
			while (ok::graphics::Camera::GetCurrent() != nullptr)
			{
				ok::graphics::Camera::PopCamera();
			}

			fixed_resolution_batch->BatchBegin(0.f);
			ok::graphics::Texture fixed_resolution_framebuffer_texture = ok::graphics::Texture(fixed_resolution_framebuffer);
			fixed_resolution_batch->Draw(&fixed_resolution_framebuffer_texture, glm::vec2(0.f, 0.f), glm::vec2(2.f, 2.f), false);
			fixed_resolution_batch->BatchEnd();
		}
		else
		{
			while (ok::graphics::Camera::GetCurrent() != nullptr)
			{
				ok::graphics::Camera::PopCamera();
			}
		}
	
		window->display();

		ok::Input::instance().Update();
	}

	if (fixed_resolution)
	{
		delete fixed_resolution_batch;
		delete fixed_resolution_framebuffer;
	}

	window->close();
	delete window;
}

void ok::Application::Update(float dt)
{

}

void ok::Application::LoadSettings()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("settings.xml");

	tinyxml2::XMLElement* elem;
	elem = doc.FirstChildElement("settings")->FirstChildElement("application");

	window_title = elem->Attribute("title");
	
	ok::Assets::instance().assets_root_folder = elem->FirstChildElement("assets")->Attribute("root_folder");

	elem = elem->FirstChildElement("screen");

	screen_width = elem->IntAttribute("width", 640);
	screen_height = elem->IntAttribute("height", 480);
	window_width = screen_width;
	window_height = screen_height;
	screen_depth_bits = elem->IntAttribute("depth", 24);
	screen_stencil_bits = elem->IntAttribute("stencil", 8);
	antialiasing = elem->IntAttribute("antialiasing", 0);
	framerate = elem->FloatAttribute("framerate", 60);
	vsync = elem->BoolAttribute("vsync", true);
	keep_aspect_ratio = elem->BoolAttribute("keep_aspect_ratio", true);
	fixed_resolution = elem->BoolAttribute("fixed_resolution", true);
	ok::graphics::Camera::_fixed_resolution_enabled = fixed_resolution;
	ok::graphics::Camera::_keep_aspect_ratio_enabled = keep_aspect_ratio;

	fullscreen = elem->BoolAttribute("fullscreen", false);

	elem = elem->FirstChildElement("background");
	background_color = ok::Color((unsigned char)elem->IntAttribute("r"), (unsigned char)elem->IntAttribute("g"), (unsigned char)elem->IntAttribute("b"), 255);
}
