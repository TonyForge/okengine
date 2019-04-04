#pragma once

#include "Texture.h"

namespace ok
{
	namespace graphics
	{
		enum class ShaderAliasReference
		{
			ModelViewProjectionMatrix = 0,
			ModelMatrix,
			ViewProjectionMatrix,
			Bones,
			Callback
		};

		enum class ShaderAliasType
		{
			alias_mat4 = 0,
			alias_mat4_transpose,
			alias_float,
			alias_int,
			alias_vec2,
			alias_vec3,
			alias_vec4,
			alias_float_array,
			alias_mat4_array,
			alias_mat4_array_transpose,
			alias_vx_subroutine_array,
			alias_px_subroutine_array
		};

		enum class ShaderUniformType
		{
			uniform_vec4 = 0,
			uniform_vec3,
			uniform_vec2,
			uniform_float,
			uniform_sampler2D
		};

		class ShaderAliasDispatcher
		{
		public:
			ShaderAliasDispatcher();

			virtual glm::mat4 DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type);
			virtual glm::vec4 DispatchAliasVec4(ok::graphics::ShaderAliasReference alias_type);
			virtual glm::vec3 DispatchAliasVec3(ok::graphics::ShaderAliasReference alias_type);
			virtual glm::vec2 DispatchAliasVec2(ok::graphics::ShaderAliasReference alias_type);
			virtual float DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type);
			virtual int DispatchAliasInt(ok::graphics::ShaderAliasReference alias_type);
			virtual std::pair<float*, int> DispatchAliasFloatArray(ok::graphics::ShaderAliasReference alias_type);
			virtual std::pair<glm::mat4*, int> DispatchAliasMat4Array(ok::graphics::ShaderAliasReference alias_type);
			virtual std::pair<unsigned int*, int> DispatchAliasSubroutineArray(ok::graphics::ShaderAliasReference alias_type);

			const std::string* callback_name_ptr;

			static bool unresolved_alias;
			std::vector<ok::graphics::ShaderAliasDispatcher*> shader_alias_dispatcher_mirrors;
		};

		class ShaderOptions
		{
		public:
			bool isTransparentEnabled;
			bool isBlendEnabled;
			bool isCullEnabled;
			bool isDepthEnabled;

			bool blendSeparateEnabled;
			GLenum blendFunc_Factors[4];
			GLenum blendEquation_Modes[2];
			GLclampf blendColorConstant[4];

			GLenum cullFace;
			GLenum depthFunc;
			GLenum depthMask;
		};

		class Shader
		{
		public:
			bool isValid;
			unsigned int shader_program_id;
			ok::graphics::ShaderOptions options;

			Shader();

			void RegisterUniform(const std::string& name, GLint location);
			void RegisterUniform_Sampler(const std::string& name, GLint location);
			void RegisterSubroutine(GLenum shadertype, const std::string& name, GLuint location);
			void RegisterSubroutine_Uniform(GLenum shadertype, const std::string& name, GLint location);
			int GetUniformDirectAccessIndex(const std::string& name);
			int GetSubroutineDirectAccessIndex(GLenum shadertype, const std::string& name);
			int GetSubroutineUniformDirectAccessIndex(GLenum shadertype, const std::string& name);

			void RegisterAlias(const std::string& uniform_name, const std::string& uniform_type, const std::string& alias_reference_name);
			void RegisterAliasSubroutine(const std::string& uniform_name, const std::string& uniform_type, const std::string& alias_reference_name);

			void Bind(ok::graphics::ShaderAliasDispatcher* _dispatcher = nullptr);
			void BindSubroutines(ok::graphics::ShaderAliasDispatcher* _dispatcher = nullptr);
			void Unbind();
			bool IsBound();

			GLint GetSampler(const std::string& sampler_name);
			GLint GetSampler(const std::string& sampler_name, int& texture_channel_index);
			GLint GetSampler(int texture_channel_index);
			int GetSamplersUsed();

			GLuint GetSubroutine(GLenum shadertype, const std::string& name);
			GLuint GetSubroutine(GLenum shadertype, int index);

			void SetUniform(std::string name, glm::vec4 value);
			void SetUniform(std::string name, glm::vec3 value);
			void SetUniform(std::string name, glm::vec2 value);
			void SetUniform(std::string name, float value);
			void SetUniform(std::string name, int value);
			void SetUniform(std::string name, unsigned int value);
			void SetUniform(std::string name, bool value);
			void SetUniform(std::string name, glm::mat4 value, bool transpose = false);
			void SetUniform(std::string name, glm::mat3 value, bool transpose = false);
			void SetUniform(std::string name, glm::mat2 value, bool transpose = false);

			void SetUniform(int index, glm::vec4 value);
			void SetUniform(int index, glm::vec3 value);
			void SetUniform(int index, glm::vec2 value);
			void SetUniform(int index, float value);
			void SetUniform(int index, int value);
			void SetUniform(int index, unsigned int value);
			void SetUniform(int index, bool value);
			void SetUniform(int index, glm::mat4 value, bool transpose = false);
			void SetUniform(int index, glm::mat3 value, bool transpose = false);
			void SetUniform(int index, glm::mat2 value, bool transpose = false);

			void SetUniform(std::string name, glm::mat4* value, int count, bool transpose = false);
			void SetUniform(int index, glm::mat4* value, int count, bool transpose = false);

			void SetUniform(std::string name, float* value, int count, int stride_size = 1);
			void SetUniform(int index, float* value, int count, int stride_size = 1);

			void SetUniform(std::string name, int* value, int count, int stride_size = 1);
			void SetUniform(int index, int* value, int count, int stride_size = 1);

			void SetSubroutineUniform(std::string name, GLenum shadertype, unsigned int* value, int count);
			void SetSubroutineUniform(int index, GLenum shadertype, unsigned int* value, int count);

			static void RebindCurrentShader();
		protected:
		private:
			static ok::graphics::Shader* currentlyBoundShader;
			int samplersUsed;

			std::unordered_map<std::string, GLint> uniforms;

			std::unordered_map<std::string, GLuint> vx_subroutines;
			std::unordered_map<std::string, GLint> vx_subroutines_uniforms;
			std::unordered_map<std::string, GLuint> px_subroutines;
			std::unordered_map<std::string, GLint> px_subroutines_uniforms;

			std::unordered_map<std::string, GLint> samplers;
			std::vector<GLint> sampler_to_channel_linkage_table;

			std::vector<GLint> uniforms_table;
			std::vector<GLuint> px_subroutines_table;
			std::vector<GLuint> vx_subroutines_table;
			std::vector<GLint> px_subroutines_uniforms_table;
			std::vector<GLint> vx_subroutines_uniforms_table;

			std::vector
				<
				std::tuple
				<
				int,
				ShaderAliasType,
				ShaderAliasReference,
				std::string
				>
				> aliases;

			std::vector
				<
				std::tuple
				<
				int,
				ShaderAliasType,
				ShaderAliasReference,
				std::string
				>
				> aliases_subroutines;

			void _LinkSamplerToTextureChannelIndex(GLint sampler_location, int texture_channel_index);
		};
	}
}