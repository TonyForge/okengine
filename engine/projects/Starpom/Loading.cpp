#include "Loading.h"

Starpom::Loading* Starpom::Loading::_instance = nullptr;

Starpom::Loading::Loading(Starpom::LoadingTask* task)
{
	_instance = this;
	_task = task;

	_task->Begin();
}

Starpom::Loading::~Loading()
{
	_task->End();
	_instance = nullptr;
}

bool Starpom::Loading::Update(float dt)
{
	float progress = static_cast<float>(_task->steps_left) / static_cast<float>(_task->steps_total);

	_task->Step();

	if (_task->steps_left == 0) return true;

	return false;
}

void Starpom::LoadingTask::Begin()
{
}

void Starpom::LoadingTask::Step()
{
}

void Starpom::LoadingTask::End()
{
}

void Starpom::Task_SS_Blueprints::Begin()
{
	ok::String& root_folder = ok::Assets::instance().assets_root_folder;
	std::string std_path = root_folder + "ships\\blueprints.xml";

	tinyxml2::XMLDocument doc;
	doc.LoadFile(std_path.c_str());

	tinyxml2::XMLElement* elem;
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

	steps_left = steps_total;
}

void Starpom::Task_SS_Blueprints::Step()
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
	

	Starpom::SS_ShipAgent* agent = new Starpom::SS_ShipAgent();
	agent->AddChild(root);

	Starpom::SS_ShipAgent::blueprints[_name] = agent;

	steps_left--;
}

void Starpom::Task_SS_Blueprints::End()
{
}

ok::GameObject * Starpom::Task_SS_Blueprints::LoadPart_Spaceship(tinyxml2::XMLElement * part_xml, ok::GameObject * parent)
{
	ok::GameObject* part = new ok::GameObject();

	part->Rename(part_xml->Attribute("name"));

	//glm::eu
	//part->SetRotationOrder(ok::RotationOrder::ZXY);
	part->BeginTransform();
	part->SetPosition(glm::vec3(part_xml->FloatAttribute("tx"), part_xml->FloatAttribute("ty"), part_xml->FloatAttribute("tz")));

	part->SetOrientation(
		glm::normalize(glm::vec3(part_xml->FloatAttribute("fwd_x"), part_xml->FloatAttribute("fwd_y"), part_xml->FloatAttribute("fwd_z"))),
		glm::normalize(glm::vec3(part_xml->FloatAttribute("up_x"), part_xml->FloatAttribute("up_y"), part_xml->FloatAttribute("up_z")))
	);

	//part->SetRotation(glm::vec3(part_xml->FloatAttribute("rx"), part_xml->FloatAttribute("ry"), part_xml->FloatAttribute("rz")));
	part->SetScale(glm::vec3(part_xml->FloatAttribute("sx"), part_xml->FloatAttribute("sy"), part_xml->FloatAttribute("sz")));
	part->EndTransform(false);
	//part->SetRotationOrder(ok::RotationOrder::XYZ);

	std::string mesh_name = part_xml->Attribute("mesh");

	if (mesh_name != "")
	{
		ok::String mesh_file = "ships\\parts\\" + mesh_name + ".mesh";

		ok::graphics::MeshRenderer* mr = new ok::graphics::MeshRenderer();
		mr->SetMesh(mesh_file);
		mr->SetMaterial(ok::Assets::instance().GetMaterial("ships\\parts\\material0.xml"));

		part->AddComponent(mr);

		Starpom::SS_ShipMaterial_Spaceship* mat = new Starpom::SS_ShipMaterial_Spaceship();

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
