#include "Loading.h"

Starpom::Loading* Starpom::Loading::_instance = nullptr;

Starpom::Loading::Loading(Starpom::LoadingTask* task)
{
	_task = task;

	_task->Begin();
}

Starpom::Loading::~Loading()
{
	_task->End();
}

bool Starpom::Loading::Update(float dt)
{
	float progress = _task->steps_left / _task->steps_total;

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
	ok::String root_folder = ok::Assets::instance().assets_root_folder;
	std::string std_path = root_folder + "ships\\blueprints\\blueprints.xml";

	tinyxml2::XMLDocument doc;
	doc.LoadFile(std_path.c_str());

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* blueprint;

	elem = doc.FirstChildElement("blueprints");
	
	steps_total = 0;

	for (blueprint = elem->FirstChildElement("blueprint"); blueprint != nullptr; blueprint = blueprint->NextSiblingElement("blueprint"))
	{
		_names.push_back(std::make_unique<std::string>(blueprint->Attribute("name")));
		_files.push_back(std::make_unique<std::string>(blueprint->Attribute("name")));
		steps_total++;
	}

	steps_left = steps_total;
}

void Starpom::Task_SS_Blueprints::Step()
{
	std::string& _name = *_names[steps_total - steps_left];
	std::string& _file = *_files[steps_total - steps_left];

	//load blueprint
	Starpom::SS_ShipAgent::blueprints[_name] = new Starpom::SS_ShipAgent();

	steps_left--;
}

void Starpom::Task_SS_Blueprints::End()
{
}
