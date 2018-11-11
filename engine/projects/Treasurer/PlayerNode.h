#pragma once

#include "..\..\GameObject.h"
#include "..\..\MeshRenderer.h"
#include "..\..\Assets.h"
#include "..\..\Mesh.h"
#include "..\..\Material.h"

namespace ok
{
	namespace Treasurer
	{
		class PlayerNode : public ok::GameObject
		{
		public:
			PlayerNode();
			void Update(float dt);

			ok::graphics::Mesh* tank_tower;
			ok::graphics::Mesh* tank_body;
			ok::graphics::MeshRenderer* renderer;
			ok::graphics::Material* material;
		private:
			glm::vec3 pos_xyz = glm::vec3(0.f,0.f,0.f);

			ok::Transform root;
			ok::Transform tower_root;
		protected:
		};
	}
}