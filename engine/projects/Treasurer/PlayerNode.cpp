#include "PlayerNode.h"

ok::Treasurer::PlayerNode::PlayerNode()
{
	renderer = new ok::graphics::MeshRenderer();
	AddComponent(renderer);


	tank_tower = ok::Assets::instance().GetMesh("tank_tower.mesh");
	tank_body = ok::Assets::instance().GetMesh("tank_body.mesh");

	root.keepWorldTransform = false;
	tower_root.keepWorldTransform = false;

	//AddChild(&root);
	root.AddChild(&tower_root);

	material = ok::Assets::instance().GetMaterial("BlueTeamMaterial.xml");
	renderer->SetMaterial(material);

	root.BeginTransform();
	root.SetScale(3.f*glm::vec3(1.f, 1.f, 1.f));
	root.EndTransform(true);

	tower_root.SetPosition(glm::vec3(0.f, 0.f, 3.988f));
}

void ok::Treasurer::PlayerNode::Update(float dt)
{
	pos_xyz += glm::vec3(20.f * dt, 0.f, 0.f);

	root.BeginTransform();
	root.SetPosition(pos_xyz);
	root.EndTransform(true);

	tower_root.BeginTransform();
	tower_root.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	tower_root.EndTransform(true);

	//render

	transform().CopyPaste(root, transform(), true, ok::TransformSpace::WorldSpace);

	renderer->SetMesh(tank_body);
	renderer->Render();

	transform().CopyPaste(tower_root, transform(), true, ok::TransformSpace::WorldSpace);

	renderer->SetMesh(tank_tower);
	renderer->Render();
}
