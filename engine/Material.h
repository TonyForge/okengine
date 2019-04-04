#pragma once

#include "Shader.h"
#include "Texture.h"


namespace ok
{
	namespace graphics
	{
		class MaterialTextureSlot
		{
		public:
			ok::graphics::Texture* texture;
			GLenum wrapping;
			bool smooth;
			ok::Color background_color;
			bool fixed_asset = false;
		protected:
		private:
		};

		class Material
		{
		public:
			Material(ok::graphics::Shader* shader, int texture_slots_count);
			void SetTexture(int texture_slot_index, ok::graphics::Texture* texture);
			void SetTexture(const std::string& sampler_name, ok::graphics::Texture* texture);
			//void SetTexture(ok::String sampler_name, ok::String texture_asset);
			void Update(float dt);
			void Bind(ok::graphics::ShaderAliasDispatcher* dispatcher = nullptr);
			bool GetTransparent();
			void LinkSlotToSampler(const std::string & sampler_name, int texture_slot_index);
			void SetSlotProperties(int texture_slot_index, bool smooth, GLenum wrapping, ok::Color background_color, bool fixed_asset = false);
			void BindSubroutines(ok::graphics::ShaderAliasDispatcher* dispatcher = nullptr);
			unsigned int GetSubroutineIndex(GLenum shadertype, const std::string& name);
			ok::graphics::Shader* GetShader();
			void AllowTextureLinearFilter();
			void ForbidTextureLinearFilter();
		protected:
		private:
			bool _texture_linear_filter_allowed = true;
			ok::graphics::Shader* _shader;
			std::vector<std::pair<int, ok::graphics::MaterialTextureSlot>> _texture_slots;
		};
	}
}