#include "JumpHole.h"

Zoner::JumpHole::JumpHole()
{
	this_type = Zoner::ShipType::ST_Jumphole;
}

Zoner::Collision::Point Zoner::JumpHole::Pick(glm::vec3 world_position)
{
	BeginTransform(ok::TransformSpace::WorldSpace);
	world_position = world_position - GetPosition();
	EndTransform(false);

	glm::vec3 point = glm::vec3(glm::inverse(GetAbsoluteTransformMatrix()) * glm::vec4(world_position, 0.f)); //w = 0.f but i dont know why, must be 1.f, but doesnt work
	point = (point - glm::vec3(0.f,0.f,0.f)) / glm::vec3(radius, 1.f);
	point *= point;

	float k = point.x + point.y + point.z;

	if (k <= 1.f)
	{
		//collision detected
		return Zoner::Collision::Point(point, true);
	}

	return Zoner::Collision::Point();
}

void Zoner::JumpHole::Update(float dt)
{
	ok::GameObject::Update(dt);

	debug_draw.BatchBegin();
	debug_draw.SetBrushThickness(2.f);
	debug_draw.SetBrushSoftness(0.0001f);
	debug_draw.SetBrushColor(ok::Color::Gray);

		BeginTransform(ok::TransformSpace::WorldSpace);
			debug_draw.Circle(GetPosition(), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), radius.x, 5.f);
			debug_draw.Circle(GetPosition(), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), radius.y, 5.f);
		EndTransform(false);
	debug_draw.BatchEnd();
}

void * Zoner::JumpHole::GetPtr(int id)
{
	void* result = Zoner::Ship::GetPtr(id);

	if (id == Zoner::Requests::JumpHole_Destination)
	{
		result = destination;
	}
	else if (id == Zoner::Requests::JumpHole_DestinationPosition)
	{
		result = &destination_position;
	}

	return result;
}

void Zoner::JumpHole::SaveTo(tinyxml2::XMLDocument & doc, tinyxml2::XMLElement & element)
{
	BeginTransform();
	element.SetAttribute("x", GetPosition().x);
	element.SetAttribute("y", GetPosition().y);
	EndTransform(false);

	element.SetAttribute("dest_x", destination_position.x);
	element.SetAttribute("dest_y", destination_position.y);
	element.SetAttribute("rx", radius.x);
	element.SetAttribute("ry", radius.y);

	BeginTransform();
	element.SetAttribute("rotation", GetRotation().z);
	EndTransform(false);

	element.SetAttribute("from", location->_gameengine_id.toAnsiString().c_str());
	element.SetAttribute("to", destination->_gameengine_id.toAnsiString().c_str());
}

void Zoner::JumpHole::LoadFrom(tinyxml2::XMLDocument & doc, tinyxml2::XMLElement & element)
{
	this_type = Zoner::ShipType::ST_Jumphole;

	destination_position = glm::vec2(element.FloatAttribute("dest_x"), element.FloatAttribute("dest_y"));

	BeginTransform();
	SetPosition(glm::vec3(element.FloatAttribute("x"), element.FloatAttribute("y"), 0.f));
	SetRotation(glm::vec3(0.f, 0.f, element.FloatAttribute("rotation")));
	EndTransform(true);

	radius = glm::vec2(
		element.FloatAttribute("rx"),
		element.FloatAttribute("ry")
	);

	isNPC = true;
}
