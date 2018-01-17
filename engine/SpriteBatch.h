#pragma once

#include "AssetsBasic.h"
#include "Quad.h"
#include "GLBufferObject.h"
#include "Camera.h"

namespace ok
{
	namespace graphics
	{
		class TextureRect
		{
		public:
			ok::graphics::Texture* texture;
			int left, top, width, height;
			glm::vec4 uv_rect;
		protected:
		private:

		};

		class SpriteInfo
		{

		};

		class SpriteBatch : public ok::graphics::ShaderAliasDispatcher
		{
		public:
			SpriteBatch(int size = 1000);
			~SpriteBatch();

			void Draw(ok::graphics::TextureRect* tex_rect, glm::vec2 position, float rotation_degrees, glm::vec2 scale, bool flip_y = false, glm::vec2 hotspot = glm::vec2(0.5f, 0.5f));
			void Draw(ok::graphics::SpriteInfo* sprite_info, glm::vec2 position, float rotation_deg, glm::vec2 scale);
			void Draw(ok::graphics::Texture* tex, glm::vec2 position, glm::vec2 size, bool flip_y = false);
			//void Flush();

			void SetMaterial(ok::graphics::Material* material);
			void SetMaterial(ok::String material_asset);

			void BatchBegin(float default_z = 0.0f);
			void BatchEnd();

			glm::mat4 DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type);
			float DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type);

		protected:
		private:
			void PushQuad();

			static ok::Quad quad;
			static const unsigned int quad_position_data_size = 4 * 2; //4 vertices, 2 floats XY per vertex
			static const unsigned int quad_texcoord_data_size = 4 * 2; //4 vertices, 2 floats UV per vertex
			static const unsigned int quad_color_data_size = 4 * 4; //4 vertices, 4 floats RGBA per vertex
			static const unsigned int quad_index_data_size = 3 * 2; //2 triangles, 3 vertices per triangle, 1 ints INDEX per vertex

			int batch_quads_in_use;
			int batch_size;

			GLBufferObject* glBO;
			float* positions;
			float* texcoords;
			float* colors;
			int* indices;

			ok::graphics::Texture* batch_texture;
			ok::graphics::Material* default_material;
			ok::graphics::Material* custom_material;

			float _default_z;
		};
	}
}