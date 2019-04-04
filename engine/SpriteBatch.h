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

			TextureRect();
			TextureRect(ok::graphics::Texture* _texture, int _left, int _top, int _width, int _height);
			TextureRect(ok::graphics::Texture* _texture);
		protected:
		private:
		};

		class SpriteInfo
		{
		public:
			SpriteInfo();

			ok::graphics::TextureRect rect;
			glm::vec2 hotspot; //normalized 0..1
			glm::vec2 scale;
			ok::Color tint_color;
			float tint_power;
			bool flip_x, flip_y;
		};

		enum class SpriteAtlasPickMode
		{
			Loop,
			Clamp,
			Pong
		};

		class SpriteAtlas
		{
		private:
			struct SpriteAtlasSequence
			{
				size_t begin;
				size_t end;
			};

			ok::graphics::SpriteAtlas::SpriteAtlasSequence _temp_sequence;

		public:
			void AddSprite(ok::graphics::SpriteInfo& sprite, ok::String& name);

			void BeginSequence();
			void EndSequence(ok::String& name);

			ok::graphics::SpriteInfo& Get(ok::String& name);
			ok::graphics::SpriteInfo* Search(ok::String& name);
			ok::graphics::SpriteInfo& Get(size_t index);

			ok::graphics::SpriteInfo& Pick(float pick, bool reverse = false);
			ok::graphics::SpriteInfo& Pick(float pick, ok::graphics::SpriteAtlasPickMode mode, bool reverse = false);
			ok::graphics::SpriteInfo& Pick(float pick, int first_index, int last_index, bool reverse = false);
			ok::graphics::SpriteInfo& Pick(float pick, int first_index, int last_index, ok::graphics::SpriteAtlasPickMode mode, bool reverse = false);
			
			ok::graphics::SpriteInfo& PickSequence(ok::String& name, float pick, bool reverse = false);
			ok::graphics::SpriteInfo& PickSequence(ok::String& name, float pick, ok::graphics::SpriteAtlasPickMode mode, bool reverse = false);

			int IndexOf(ok::String& name);
		protected:
		private:
			std::vector<ok::graphics::SpriteInfo> _items;
			std::unordered_map<std::string, size_t> _items_indexes;
			std::unordered_map<std::string, ok::graphics::SpriteAtlas::SpriteAtlasSequence> _sequences;
		};

		class SpriteBatch : public ok::graphics::ShaderAliasDispatcher
		{
		public:
			SpriteBatch(int size = 1000);
			~SpriteBatch();

			void Draw(ok::graphics::TextureRect* tex_rect, glm::vec2 position, float rotation_degrees, glm::vec2 scale, bool flip_y = false, glm::vec2 hotspot = glm::vec2(0.5f, 0.5f));
			void Draw(ok::graphics::SpriteInfo* sprite_info, glm::vec2 position, float rotation_degrees, glm::vec2 scale);
			void Draw(ok::graphics::Texture* tex, glm::vec2 position, glm::vec2 size, bool flip_y = false, glm::vec2 hotspot = glm::vec2(0.5f,0.5f));
			void Draw(ok::graphics::Texture* tex, glm::mat3& transform_matrix, bool flip_y = false, glm::vec2 hotspot = glm::vec2(0.5f, 0.5f));

			void Draw(ok::graphics::SpriteInfo* sprite_info, glm::mat3& transform_matrix);


			//Pixel perfect blitting functions (camera must be pixel aligned too!)
			void Blit(ok::graphics::Texture* tex, int x, int y, glm::vec2 hotspot, bool flip_y = false);
			void Blit(ok::graphics::SpriteInfo* sprite_info, int x, int y);

			//void Flush();

			void SetMaterial(ok::graphics::Material* material);
			void SetMaterial(ok::String material_asset);

			void BatchBegin(float default_z = 0.0f);
			void BatchEnd();

			glm::mat4 DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type);
			float DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type);

		protected:
		private:
			void PushQuad(bool zero_color_data = true);

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