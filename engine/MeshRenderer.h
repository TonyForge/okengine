#pragma once

#include "AssetsBasic.h"
#include "Behaviour.h"
#include "LayeredRenderer.h"

namespace ok
{
	namespace graphics
	{
		class MeshRenderer : public ok::Behaviour, public ok::graphics::LayeredRenderable, public ok::graphics::ShaderAliasDispatcher
		{
		public:
			MeshRenderer();

			ok::graphics::Mesh* GetMesh();
			void SetMesh(ok::graphics::Mesh* mesh);
			void SetMesh(ok::String mesh_asset, bool prefer_hardware_skin = false);
			void SetMaterial(ok::graphics::Material* material);
			void SetMaterial(ok::String material_asset);
			ok::graphics::Material* GetMaterial();

			void Update(float dt);
			void Render();
			void SetStage(ok::graphics::LayeredRendererStage stage);
			ok::graphics::LayeredRendererStage GetStage();
			void SetLayer(int layer);
			int GetLayer();

			ok::Behaviour* Duplicate(ok::Behaviour * _clone = nullptr);

			glm::mat4 DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type);
			glm::vec2 DispatchAliasVec2(ok::graphics::ShaderAliasReference alias_type);
			float DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type);
			std::pair<glm::mat4*, int> DispatchAliasMat4Array(ok::graphics::ShaderAliasReference alias_type);

			std::function<void(ok::graphics::Material& material)> OnMaterialBind;
		protected:
			ok::graphics::Mesh* _mesh;
			ok::graphics::Material* _material;
			ok::graphics::LayeredRendererRequest _renderer_request;
		private:
		};
	}
}