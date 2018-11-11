#pragma once

#include "..\..\AssetsBasic.h"
#include "..\..\Behaviour.h"
#include "..\..\LayeredRenderer.h"
#include "..\..\Shader.h"
#include "..\..\Mesh.h"
#include "..\..\Material.h"
#include "..\..\FOVMap.h"

namespace ok
{
	namespace Kripta
	{
		class KriptaTileMap3D;

		class KriptaTile3D
		{
		public:
			ok::graphics::Mesh* mesh = nullptr;
			glm::vec2 uv_shift;
		private:
			friend class ok::Kripta::KriptaTileMap3D;
			int _x = 0;
			int _y = 0;
		};

		class KriptaTileMap3D : public ok::Behaviour, public ok::graphics::LayeredRenderable, public ok::graphics::ShaderAliasDispatcher
		{
		public:
			KriptaTileMap3D();

			void Resize(int x_size, int y_size);
			void RegisterTile(int id, ok::graphics::Mesh* mesh, glm::vec2 uv_shift);
			void Rebuild(int x, int y, int radius, ok::FOVMap& fov_map);

			void Render();

			void SetMaterial(ok::graphics::Material* material);
			void SetMaterial(ok::String material_asset);
			ok::graphics::Material* GetMaterial();

			void Update(float dt);

			void SetStage(ok::graphics::LayeredRendererStage stage);
			ok::graphics::LayeredRendererStage GetStage();
			void SetLayer(int layer);
			int GetLayer();

			glm::mat4 DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type);

		private:
			bool _nothing_to_render = true;
			ok::graphics::Mesh* _merged_mesh = nullptr;
			int _map_size_x = 0;
			int _map_size_y = 0;
			std::vector<std::pair<int, bool>> _map;
			std::vector<ok::Kripta::KriptaTile3D> _tiles;
			std::vector<ok::Kripta::KriptaTile3D> _tiles_to_render;

			struct duo_float
			{
				float lpart;
				float rpart;
			};

			ok::graphics::Material* _material;
			ok::graphics::LayeredRendererRequest _renderer_request;

		protected:
		};
	}
}