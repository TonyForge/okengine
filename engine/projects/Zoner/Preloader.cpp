#include "Preloader.h"

void Zoner::Preloader::LoadItself()
{
	camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::Screen);
	camera->SetProjectionOrtho(static_cast<float>(Zoner::IGame::o().GetScreenWidth()), static_cast<float>(Zoner::IGame::o().GetScreenHeight()), 1.f, 100.f);

	camera->BeginTransform();
	camera->SetPosition(glm::vec3(0.f, 0.f, -10.f));
	camera->EndTransform(true);

	camera->LookAt(glm::vec3(0.f, 0.f, 0.f));

	progress_bar = new ok::graphics::LineBatch(1);
}

void Zoner::Preloader::UnloadItself()
{
	delete camera;
	delete progress_bar;
}

void Zoner::Preloader::Update(float dt)
{
	ok::graphics::Camera::PushCamera(camera);
	ok::GameObject::Update(dt);

	if (_task != nullptr)
	{
		if (task_step == -1)
		{
			_task->Begin();

			task_step = 0;
			task_step_max = _task->steps_total;
		}
		else
		{
			if (task_step == task_step_max)
			{
				_task->End();
				delete _task;
				_task = nullptr;
			}
			else
			{
				_task->Step();
				task_step++;
			}
		}

	}

	float progress = static_cast<float>(task_step) / static_cast<float>(task_step_max);

	progress_bar->SetBrushThickness(8.f);
	progress_bar->SetBrushSoftness(0.01f);
	progress_bar->SetBrushColor(ok::Color(0.f,1.f,0.f,1.f));

	progress_bar->BatchBegin();
	progress_bar->MoveTo(glm::vec3(0.f, Zoner::IGame::o().GetScreenHeight()*0.5f, 0.f));
	progress_bar->LineTo(glm::vec3(Zoner::IGame::o().GetScreenWidth()*progress, Zoner::IGame::o().GetScreenHeight()*0.5f, 0.f));
	progress_bar->BatchEnd();

	ok::graphics::Camera::PopCamera();
}


void Zoner::Preloader::Task_DefaultResources()
{
	task_step = -1;
	
	_task = new Zoner::Preloader::Task_DefaultResources_Object();
}

void Zoner::Preloader::Task_ShowProgress()
{
	task_step = 0;
	task_step_max = 1;
	_task = nullptr;
}

void Zoner::Preloader::Task_ShowProgress_Update(int _task_step, int _task_step_max)
{
	task_step = _task_step;
	task_step_max = _task_step_max;
}

void Zoner::Preloader::Task_DefaultResources_Object::Begin()
{
	ok::String& root_folder = ok::Assets::instance().assets_root_folder;
	std::string std_path = root_folder + "ships\\blueprints.xml";

	tinyxml2::XMLDocument doc;
	doc.LoadFile(std_path.c_str());

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* inner_elem;
	tinyxml2::XMLElement* blueprint;

	elem = doc.FirstChildElement("blueprints");

	steps_total = 0;

	for (blueprint = elem->FirstChildElement("blueprint"); blueprint != nullptr; blueprint = blueprint->NextSiblingElement("blueprint"))
	{
		_names.push_back(std::make_unique<std::string>(blueprint->Attribute("name")));
		_files.push_back(std::make_unique<std::string>(blueprint->Attribute("file")));
		_types.push_back(std::make_unique<std::string>(blueprint->Attribute("type")));
		steps_total++;
	}

	//Atlases
	_atlases_begin_step = steps_total;

	std_path = root_folder + "zoner.sprite_atlases.xml";
	doc.LoadFile(std_path.c_str());

	elem = doc.FirstChildElement("atlases");

	for (inner_elem = elem->FirstChildElement("atlas"); inner_elem != nullptr; inner_elem = inner_elem->NextSiblingElement("atlas"))
	{
		_names.push_back(std::make_unique<std::string>(inner_elem->Attribute("name")));
		_files.push_back(std::make_unique<std::string>(inner_elem->Attribute("file")));
		steps_total++;
	}

	//Fonts
	_fonts_begin_step = steps_total;

	_names.push_back(std::make_unique<std::string>("lt_steel_b_xl_01"));
	_files.push_back(std::make_unique<std::string>(""));
	steps_total++;

	_names.push_back(std::make_unique<std::string>("plat_b_xxl_01"));
	_files.push_back(std::make_unique<std::string>(""));
	steps_total++;


	steps_left = steps_total;
}

void Zoner::Preloader::Task_DefaultResources_Object::Step()
{
	//blueprints loading
	if (steps_total - steps_left < _atlases_begin_step)
	{
		std::string& _name = *_names[steps_total - steps_left];
		std::string& _file = *_files[steps_total - steps_left];
		std::string& _type = *_types[steps_total - steps_left];

		//load blueprint
		ok::GameObject* root = nullptr;

		ok::String& root_folder = ok::Assets::instance().assets_root_folder;
		std::string std_path = root_folder + "ships\\blueprints\\" + _file;

		tinyxml2::XMLDocument doc;
		doc.LoadFile(std_path.c_str());

		if (_type == "spaceship")
		{
			root = LoadPart_Spaceship(doc.FirstChildElement("Ship")->FirstChildElement("Part"), nullptr);
		}
		//...

		Zoner::ShipBlueprint* blueprint = new Zoner::ShipBlueprint();
		blueprint->Rename(_name);

		ok::GameObject* transformer = new ok::GameObject();

		transformer->AddChild(root);
		transformer->BeginTransform();
		transformer->SetRotation(glm::vec3(-90.0f, 0.f, 0.f));
		transformer->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		transformer->EndTransform(true);

		blueprint->AddChild(transformer);

		blueprint->CalculateBounder();

		Zoner::IGame::o().GetShipBlueprints()[_name] = blueprint;
	}
	else
	{
		if (steps_total - steps_left < _fonts_begin_step)
		{
			//atlases loading
			std::string& _name = *_names[steps_total - steps_left];
			std::string& _file = *_files[steps_total - steps_left];

			Zoner::IGame::o().GetSpriteAtlases()[_name] = ok::Assets::instance().GetSpriteAtlas(_file);
		}
		else
		{
			//fonts loading
			std::string& _name = *_names[steps_total - steps_left];

			if (_name == "lt_steel_b_xl_01")
			{
				ok::graphics::Font* fnt = ok::Assets::instance().GetFont("lt_steel_b_xl_01");
				fnt->SetInternalFont(ok::Assets::instance().GetInternalFont("zoner_bold.font.xml"));

				//fnt->
				fnt->SetBrushSizeOverride(true, 16.f);
				//fnt->SetBrushColor(ok::Color(187, 233, 250, 255));
				fnt->SetBrushGradient(
					ok::Color(187, 233, 250, 255),
					ok::Color(110, 189, 218, 255)
				);

				fnt->SetBrushOuterShadow(ok::Color(6, 54, 87, 255), 0.5f, 0.1f, 0.f, 0.1f);
			}
			else if(_name == "plat_b_xxl_01")
			{
				ok::graphics::Font* fnt = ok::Assets::instance().GetFont("plat_b_xxl_01");
				fnt->SetInternalFont(ok::Assets::instance().GetInternalFont("zoner_bold.font.xml"));
			}
		}
	}

	steps_left--;
}

void Zoner::Preloader::Task_DefaultResources_Object::End()
{
	Zoner::IGame::o().State(Zoner::GameStates::DefaultResourcesLoaded, true);
}

ok::GameObject * Zoner::Preloader::Task_DefaultResources_Object::LoadPart_Spaceship(tinyxml2::XMLElement * part_xml, ok::GameObject * parent)
{
	ok::GameObject* part = new ok::GameObject();

	part->Rename(part_xml->Attribute("name"));

	part->BeginTransform();
	part->SetPosition(glm::vec3(part_xml->FloatAttribute("tx"), part_xml->FloatAttribute("ty"), part_xml->FloatAttribute("tz")));

	part->SetOrientation(
		glm::normalize(glm::vec3(part_xml->FloatAttribute("fwd_x"), part_xml->FloatAttribute("fwd_y"), part_xml->FloatAttribute("fwd_z"))),
		glm::normalize(glm::vec3(part_xml->FloatAttribute("up_x"), part_xml->FloatAttribute("up_y"), part_xml->FloatAttribute("up_z")))
	);

	part->SetScale(glm::vec3(part_xml->FloatAttribute("sx"), part_xml->FloatAttribute("sy"), part_xml->FloatAttribute("sz")));
	part->EndTransform(false);


	std::string mesh_name = part_xml->Attribute("mesh");

	if (mesh_name != "")
	{
		ok::String mesh_file = "ships\\parts\\" + mesh_name + ".mesh";

		ok::graphics::MeshRenderer* mr = new ok::graphics::MeshRenderer();
		mr->SetMesh(mesh_file);
		mr->SetMaterial(ok::Assets::instance().GetMaterial("ships\\parts\\material0.xml"));

		part->AddComponent(mr);

		Zoner::ShipMaterial_SpacecraftFragment* mat = new Zoner::ShipMaterial_SpacecraftFragment();

		mat->_Normals = ok::Assets::instance().GetTexture("ships\\parts\\normals.png");
		mat->_Maps = ok::Assets::instance().GetTexture("ships\\parts\\maps.png");
		mat->_Facture = ok::Assets::instance().GetTexture("ships\\parts\\" + std::string(part_xml->Attribute("tex")) + ".png");

		mat->_FactureTransform = glm::vec4(part_xml->FloatAttribute("ft_x"), part_xml->FloatAttribute("ft_y"), part_xml->FloatAttribute("ft_z"), part_xml->FloatAttribute("ft_w"));
		mat->_ColorMain_0 = glm::vec4(part_xml->FloatAttribute("cm0_r"), part_xml->FloatAttribute("cm0_g"), part_xml->FloatAttribute("cm0_b"), part_xml->FloatAttribute("cm0_a"));
		mat->_ColorMain_1 = glm::vec4(part_xml->FloatAttribute("cm1_r"), part_xml->FloatAttribute("cm1_g"), part_xml->FloatAttribute("cm1_b"), part_xml->FloatAttribute("cm1_a"));
		mat->_ColorMain_DarkMultiplier = glm::vec4(part_xml->FloatAttribute("cmdm_r"), part_xml->FloatAttribute("cmdm_g"), part_xml->FloatAttribute("cmdm_b"), part_xml->FloatAttribute("cmdm_a"));
		mat->_ColorSec_0 = glm::vec4(part_xml->FloatAttribute("cs0_r"), part_xml->FloatAttribute("cs0_g"), part_xml->FloatAttribute("cs0_b"), part_xml->FloatAttribute("cs0_a"));
		mat->_ColorSec_1 = glm::vec4(part_xml->FloatAttribute("cs1_r"), part_xml->FloatAttribute("cs1_g"), part_xml->FloatAttribute("cs1_b"), part_xml->FloatAttribute("cs1_a"));
		mat->_ColorSet3_0 = glm::vec4(part_xml->FloatAttribute("cs30_r"), part_xml->FloatAttribute("cs30_g"), part_xml->FloatAttribute("cs30_b"), part_xml->FloatAttribute("cs30_a"));
		mat->_ColorSet3_1 = glm::vec4(part_xml->FloatAttribute("cs31_r"), part_xml->FloatAttribute("cs31_g"), part_xml->FloatAttribute("cs31_b"), part_xml->FloatAttribute("cs31_a"));
		
		mr->OnMaterialBind = [mat](ok::graphics::Material& value) { mat->OnMaterialBind(value); };

		part->AddComponent(mat);
	}

	for (part_xml = part_xml->FirstChildElement("Part"); part_xml != nullptr; part_xml = part_xml->NextSiblingElement("Part"))
	{
		LoadPart_Spaceship(part_xml, part);
	}

	if (parent != nullptr)
	{
		parent->AddChild(part, false);
	}

	return part;
}
