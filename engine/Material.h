#pragma once

#include "Shader.h"
#include "Texture.h"


namespace ok
{
	namespace graphics
	{

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
			//void LinkIdToSubroutine(GLenum shadertype, ok::String subroutine_name, int id);

			void BindSubroutines(ok::graphics::ShaderAliasDispatcher* dispatcher = nullptr);
			unsigned int GetSubroutineIndex(GLenum shadertype, const std::string& name);
			//void SetSubroutine(GLenum shadertype, int index, int id);
			//std::vector<GLint>& GetSubroutines(GLenum shadertype);
			//void ReloadSubroutines();
			ok::graphics::Shader* GetShader();
		protected:
		private:
			ok::graphics::Shader* _shader;
			std::vector<std::pair<int, ok::graphics::Texture*>> _texture_slots;

			//std::vector<GLint> _vx_subroutines;
			//std::vector<GLint> _px_subroutines;

			//std::vector<std::vector<GLint>> _vx_subroutines_uniforms;
			//std::vector<std::vector<GLint>> _px_subroutines_uniforms;
		};
	}
}