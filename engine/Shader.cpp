#include "Shader.h"

ok::graphics::Shader* ok::graphics::Shader::currentlyBoundShader = nullptr;
bool ok::graphics::ShaderAliasDispatcher::unresolved_alias = false;

ok::graphics::Shader::Shader()
{
	isValid = false;
	shader_program_id = 0;
	samplersUsed = 0;
}

void ok::graphics::Shader::RegisterUniform(const std::string& name, GLint location)
{
	uniforms[name] = location;
	uniforms_table.push_back(location);
}

void ok::graphics::Shader::RegisterUniform_Sampler(const std::string& name, GLint location)
{
	uniforms[name] = location;
	uniforms_table.push_back(location);

	_LinkSamplerToTextureChannelIndex(location, samplersUsed); 
	samplers[name] = location;
	samplersUsed++;
}

void ok::graphics::Shader::RegisterSubroutine(GLenum shadertype, const std::string& name, GLuint location)
{
	if (shadertype == GL_VERTEX_SHADER)
	{
		vx_subroutines[name] = location;
		vx_subroutines_table.push_back(location);
	}

	if (shadertype == GL_FRAGMENT_SHADER)
	{
		px_subroutines[name] = location;
		px_subroutines_table.push_back(location);
	}
}

void ok::graphics::Shader::RegisterSubroutine_Uniform(GLenum shadertype, const std::string& name, GLint location)
{
	if (shadertype == GL_VERTEX_SHADER)
	{
		vx_subroutines_uniforms[name] = location;
		vx_subroutines_uniforms_table.push_back(location);
	}

	if (shadertype == GL_FRAGMENT_SHADER)
	{
		px_subroutines_uniforms[name] = location;
		px_subroutines_uniforms_table.push_back(location);
	}
}

int ok::graphics::Shader::GetUniformDirectAccessIndex(const std::string& name)
{
	GLint location = uniforms[name];
	return std::find(uniforms_table.begin(), uniforms_table.end(), location) - uniforms_table.begin();
}

int ok::graphics::Shader::GetSubroutineDirectAccessIndex(GLenum shadertype, const std::string& name)
{
	GLuint location;
	int result = 0;

	if (shadertype == GL_VERTEX_SHADER)
	{
		location = vx_subroutines[name];
		result = std::find(vx_subroutines_table.begin(), vx_subroutines_table.end(), location) - vx_subroutines_table.begin();
	}

	if (shadertype == GL_FRAGMENT_SHADER)
	{
		location = px_subroutines[name];
		result = std::find(px_subroutines_table.begin(), px_subroutines_table.end(), location) - px_subroutines_table.begin();
	}

	return result;
}

int ok::graphics::Shader::GetSubroutineUniformDirectAccessIndex(GLenum shadertype, const std::string& name)
{
	GLint location;
	int result = 0;

	if (shadertype == GL_VERTEX_SHADER)
	{
		location = vx_subroutines_uniforms[name];
		result = std::find(vx_subroutines_uniforms_table.begin(), vx_subroutines_uniforms_table.end(), location) - vx_subroutines_uniforms_table.begin();
	}

	if (shadertype == GL_FRAGMENT_SHADER)
	{
		location = px_subroutines_uniforms[name];
		result = std::find(px_subroutines_uniforms_table.begin(), px_subroutines_uniforms_table.end(), location) - px_subroutines_uniforms_table.begin();
	}

	return result;
}

void ok::graphics::Shader::RegisterAlias(const std::string& uniform_name, const std::string& uniform_type, const std::string& alias_reference_name)
{
	int index = GetUniformDirectAccessIndex(uniform_name);

	ok::graphics::ShaderAliasType type;
	if (uniform_type == "float") type = ok::graphics::ShaderAliasType::alias_float;
	else
	if (uniform_type == "mat4") type = ok::graphics::ShaderAliasType::alias_mat4;
	else
	if (uniform_type == "mat4_transpose") type = ok::graphics::ShaderAliasType::alias_mat4_transpose;
	else
	if (uniform_type == "vec4") type = ok::graphics::ShaderAliasType::alias_vec4;
	else
	if (uniform_type == "vec3") type = ok::graphics::ShaderAliasType::alias_vec3;
	else
	if (uniform_type == "vec2") type = ok::graphics::ShaderAliasType::alias_vec2;
	else
	if (uniform_type == "float_array") type = ok::graphics::ShaderAliasType::alias_float_array;
	else
	if (uniform_type == "mat4_array") type = ok::graphics::ShaderAliasType::alias_mat4_array;
	else
	if (uniform_type == "int") type = ok::graphics::ShaderAliasType::alias_int;
	else
	if (uniform_type == "vx_subroutine_array") type = ok::graphics::ShaderAliasType::alias_vx_subroutine_array;
	else
	if (uniform_type == "px_subroutine_array") type = ok::graphics::ShaderAliasType::alias_px_subroutine_array;

	ok::graphics::ShaderAliasReference reference;
	if (alias_reference_name == "ModelViewProjectionMatrix") reference = ok::graphics::ShaderAliasReference::ModelViewProjectionMatrix;
	else
	if (alias_reference_name == "ViewProjectionMatrix") reference = ok::graphics::ShaderAliasReference::ViewProjectionMatrix;
	else
	if (alias_reference_name == "ModelMatrix") reference = ok::graphics::ShaderAliasReference::ModelMatrix;
	else
	if (alias_reference_name == "Bones") reference = ok::graphics::ShaderAliasReference::Bones;
	else
	if (alias_reference_name == "Callback") reference = ok::graphics::ShaderAliasReference::Callback;
		
	aliases.push_back(std::tuple<int, ok::graphics::ShaderAliasType, ok::graphics::ShaderAliasReference, std::string>(index, type, reference, uniform_name));
}

void ok::graphics::Shader::RegisterAliasSubroutine(const std::string& uniform_name, const std::string& uniform_type, const std::string& alias_reference_name)
{
	GLenum shadertype;
	
	ok::graphics::ShaderAliasType type;
	if (uniform_type == "vx_subroutine_array") { type = ok::graphics::ShaderAliasType::alias_vx_subroutine_array; shadertype = GL_VERTEX_SHADER; }
	else
	if (uniform_type == "px_subroutine_array") { type = ok::graphics::ShaderAliasType::alias_px_subroutine_array;  shadertype = GL_FRAGMENT_SHADER;}

	ok::graphics::ShaderAliasReference reference;

	if (alias_reference_name == "Callback") reference = ok::graphics::ShaderAliasReference::Callback;

	int index = GetSubroutineUniformDirectAccessIndex(shadertype, uniform_name);

	aliases_subroutines.push_back(std::tuple<int, ok::graphics::ShaderAliasType, ok::graphics::ShaderAliasReference, std::string>(index, type, reference, uniform_name));
}

void ok::graphics::Shader::Bind(ok::graphics::ShaderAliasDispatcher* _dispatcher)
{
	if (IsBound() == true)
	{
		//do nothing
	}
	else
	{
		//bind shader

		glUseProgram(shader_program_id);
		currentlyBoundShader = this;

		if (options.isBlendEnabled)
		{
			glEnable(GL_BLEND);
			if (options.blendSeparateEnabled)
			{
				glBlendEquationSeparate(options.blendEquation_Modes[0], options.blendEquation_Modes[1]);
				glBlendFuncSeparate(options.blendFunc_Factors[0], options.blendFunc_Factors[1], options.blendFunc_Factors[2], options.blendFunc_Factors[3]);
			}
			else
			{
				glBlendEquation(options.blendEquation_Modes[0]);
				glBlendFunc(options.blendFunc_Factors[0], options.blendFunc_Factors[1]);
			}

			glBlendColor(options.blendColorConstant[0], options.blendColorConstant[1], options.blendColorConstant[2], options.blendColorConstant[3]);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		if (options.isDepthEnabled)
		{
			glEnable(GL_DEPTH_TEST);

			glDepthMask(options.depthMask);
			glDepthFunc(options.depthFunc);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if (options.isCullEnabled)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(options.cullFace);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	
	ok::graphics::ShaderAliasType alias_type;
	ok::graphics::ShaderAliasReference alias_reference;


	for (auto&& alias : aliases)
	{
		alias_type = std::get<1>(alias);
		alias_reference = std::get<2>(alias);

		for (auto&& dispatcher : _dispatcher->shader_alias_dispatcher_mirrors)
		{
			ok::graphics::ShaderAliasDispatcher::unresolved_alias = false;

			if (alias_reference == ok::graphics::ShaderAliasReference::Callback)
			{
				dispatcher->callback_name_ptr = &std::get<3>(alias);
			}

			switch (alias_type)
			{
			case ok::graphics::ShaderAliasType::alias_mat4:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasMat4(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_mat4_transpose:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasMat4(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output, true);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_float:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasFloat(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_int:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasInt(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_vec4:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasVec4(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_vec3:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasVec3(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_vec2:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasVec2(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_float_array:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasFloatArray(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output.first, dispatcher_output.second);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_mat4_array:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasMat4Array(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetUniform(std::get<0>(alias), dispatcher_output.first, dispatcher_output.second);
				}
			}
			break;
			}


			dispatcher->callback_name_ptr = nullptr;

			if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
			{
				//continue
			}
			else
			{
				//assert here
				break;
			}
		}
	}
}

void ok::graphics::Shader::BindSubroutines(ok::graphics::ShaderAliasDispatcher * _dispatcher)
{

	ok::graphics::ShaderAliasType alias_type;
	ok::graphics::ShaderAliasReference alias_reference;

	for (auto&& alias : aliases_subroutines)
	{
		alias_type = std::get<1>(alias);
		alias_reference = std::get<2>(alias);

		for (auto&& dispatcher : _dispatcher->shader_alias_dispatcher_mirrors)
		{
			ok::graphics::ShaderAliasDispatcher::unresolved_alias = false;

			if (alias_reference == ok::graphics::ShaderAliasReference::Callback)
			{
				dispatcher->callback_name_ptr = &std::get<3>(alias);
			}

			switch (alias_type)
			{
			case ok::graphics::ShaderAliasType::alias_vx_subroutine_array:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasSubroutineArray(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetSubroutineUniform(std::get<0>(alias), GL_VERTEX_SHADER, dispatcher_output.first, dispatcher_output.second);
				}
			}
			break;
			case ok::graphics::ShaderAliasType::alias_px_subroutine_array:
			{
				const auto& dispatcher_output = dispatcher->DispatchAliasSubroutineArray(alias_reference);

				if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
				{
					//do nothing
				}
				else
				{
					SetSubroutineUniform(std::get<0>(alias), GL_FRAGMENT_SHADER, dispatcher_output.first, dispatcher_output.second);
				}
			}
			break;
			}


			dispatcher->callback_name_ptr = nullptr;

			if (ok::graphics::ShaderAliasDispatcher::unresolved_alias == true)
			{
				//continue
			}
			else
			{
				//assert here
				break;
			}
		}
		
	}
}

void ok::graphics::Shader::Unbind()
{
	glUseProgram(0);
	if (currentlyBoundShader == this) currentlyBoundShader = nullptr;
}

bool ok::graphics::Shader::IsBound()
{
	return (currentlyBoundShader == this);
}

GLint ok::graphics::Shader::GetSampler(const std::string& sampler_name)
{
	return samplers[sampler_name];
}

GLint ok::graphics::Shader::GetSampler(const std::string& sampler_name, int & texture_channel_index)
{
	GLint result = samplers[sampler_name];
	texture_channel_index = static_cast<int>(std::find(sampler_to_channel_linkage_table.begin(), sampler_to_channel_linkage_table.end(), result) - sampler_to_channel_linkage_table.begin());//std::distance(sampler_to_channel_linkage_table.begin(), std::find(sampler_to_channel_linkage_table.begin(), sampler_to_channel_linkage_table.end(), result));
	return result;
}

GLint ok::graphics::Shader::GetSampler(int texture_channel_index)
{
	return sampler_to_channel_linkage_table[texture_channel_index];
}

int ok::graphics::Shader::GetSamplersUsed()
{
	return samplersUsed;
}

GLuint ok::graphics::Shader::GetSubroutine(GLenum shadertype, const std::string & name)
{
	GLuint location = 0;

	if (shadertype == GL_VERTEX_SHADER)
	{
		location = vx_subroutines[name];
	}
	else
	if (shadertype == GL_FRAGMENT_SHADER)
	{
		location = px_subroutines[name];
	}

	return location;
}

GLuint ok::graphics::Shader::GetSubroutine(GLenum shadertype, int index)
{
	GLuint location = 0;

	if (shadertype == GL_VERTEX_SHADER)
	{
		location = vx_subroutines_table[index];
	}
	else
	if (shadertype == GL_FRAGMENT_SHADER)
	{
		location = px_subroutines_table[index];
	}

	return location;
}

void ok::graphics::Shader::SetUniform(std::string name, glm::vec4 value)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform4fv(location, 1, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, glm::vec3 value)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform3fv(location, 1, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, glm::vec2 value)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform2fv(location, 1, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, float value)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform1f(location, value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, int value)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform1i(location, value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, unsigned int value)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform1ui(location, value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, bool value)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform1i(location, (int)value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, glm::mat4 value, bool transpose)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, glm::mat3 value, bool transpose)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformMatrix3fv(location, 1, transpose, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, glm::mat2 value, bool transpose)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformMatrix2fv(location, 1, transpose, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, glm::vec4 value)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform4fv(location, 1, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, glm::vec3 value)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform3fv(location, 1, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, glm::vec2 value)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform2fv(location, 1, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, float value)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform1f(location, value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, int value)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform1i(location, value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, unsigned int value)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform1ui(location, value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, bool value)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniform1i(location, (int)value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, glm::mat4 value, bool transpose)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, glm::mat3 value, bool transpose)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformMatrix3fv(location, 1, transpose, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, glm::mat2 value, bool transpose)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformMatrix2fv(location, 1, transpose, glm::value_ptr(value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, glm::mat4 * value, int count, bool transpose)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformMatrix4fv(location, count, transpose, glm::value_ptr(*value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, glm::mat4 * value, int count, bool transpose)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformMatrix4fv(location, count, transpose, glm::value_ptr(*value));

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, float * value, int count, int stride_size)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	switch (stride_size)
	{
		case 1 :
		{
			glUniform1fv(location, count, (GLfloat*)(value));
		}
		break;
		case 2:
		{
			glUniform2fv(location, count / 2, (GLfloat*)(value));
		}
		break;
		case 3:
		{
			glUniform3fv(location, count / 3, (GLfloat*)(value));
		}
		break;
		case 4:
		{
			glUniform4fv(location, count / 4, (GLfloat*)(value));
		}
		break;
	}

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, float * value, int count, int stride_size)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	switch (stride_size)
	{
		case 1:
		{
			glUniform1fv(location, count, (GLfloat*)(value));
		}
		break;
		case 2:
		{
			glUniform2fv(location, count / 2, (GLfloat*)(value));
		}
		break;
		case 3:
		{
			glUniform3fv(location, count / 3, (GLfloat*)(value));
		}
		break;
		case 4:
		{
			glUniform4fv(location, count / 4, (GLfloat*)(value));
		}
		break;
	}

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(std::string name, int * value, int count, int stride_size)
{
	GLint location = uniforms[name];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	switch (stride_size)
	{
	case 1:
	{
		glUniform1iv(location, count, (GLint*)(value));
	}
	break;
	case 2:
	{
		glUniform2iv(location, count / 2, (GLint*)(value));
	}
	break;
	case 3:
	{
		glUniform3iv(location, count / 3, (GLint*)(value));
	}
	break;
	case 4:
	{
		glUniform4iv(location, count / 4, (GLint*)(value));
	}
	break;
	}

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetUniform(int index, int * value, int count, int stride_size)
{
	GLint location = uniforms_table[index];

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	switch (stride_size)
	{
	case 1:
	{
		glUniform1iv(location, count, (GLint*)(value));
	}
	break;
	case 2:
	{
		glUniform2iv(location, count / 2, (GLint*)(value));
	}
	break;
	case 3:
	{
		glUniform3iv(location, count / 3, (GLint*)(value));
	}
	break;
	case 4:
	{
		glUniform4iv(location, count / 4, (GLint*)(value));
	}
	break;
	}

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetSubroutineUniform(std::string name, GLenum shadertype, unsigned int* value, int count)
{
	GLint location;

	if (shadertype == GL_VERTEX_SHADER)
	{
		location = vx_subroutines_uniforms[name];
	}

	if (shadertype == GL_FRAGMENT_SHADER)
	{
		location = px_subroutines_uniforms[name];
	}

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformSubroutinesuiv(shadertype, (GLsizei)count, (const GLuint*)value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::SetSubroutineUniform(int index, GLenum shadertype, unsigned int* value, int count)
{
	GLint location;

	if (shadertype == GL_VERTEX_SHADER)
	{
		location = vx_subroutines_uniforms_table[index];
	}

	if (shadertype == GL_FRAGMENT_SHADER)
	{
		location = px_subroutines_uniforms_table[index];
	}

	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	glUniformSubroutinesuiv(shadertype, (GLsizei)count, (const GLuint*)value);

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

void ok::graphics::Shader::RebindCurrentShader()
{
	if (currentlyBoundShader != nullptr)
	{
		glUseProgram(currentlyBoundShader->shader_program_id);
	}
}

void ok::graphics::Shader::_LinkSamplerToTextureChannelIndex(GLint sampler_location, int texture_channel_index)
{
	if (!IsBound())
	{
		glUseProgram(shader_program_id);
	}

	sampler_to_channel_linkage_table.resize(texture_channel_index + 1);
	sampler_to_channel_linkage_table[texture_channel_index] = sampler_location;

	glUniform1i(sampler_location, 1+texture_channel_index); //GL_TEXTURE0 is reserved by the engine, so use GL_TEXTURE1 instead

	if (!IsBound())
	{
		glUseProgram(0);
	}
}

ok::graphics::ShaderAliasDispatcher::ShaderAliasDispatcher()
{
	shader_alias_dispatcher_mirrors.push_back(this);
}

glm::mat4 ok::graphics::ShaderAliasDispatcher::DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return glm::mat4(1.f);
}

glm::vec4 ok::graphics::ShaderAliasDispatcher::DispatchAliasVec4(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return glm::vec4();
}

glm::vec3 ok::graphics::ShaderAliasDispatcher::DispatchAliasVec3(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return glm::vec3();
}

glm::vec2 ok::graphics::ShaderAliasDispatcher::DispatchAliasVec2(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return glm::vec2();
}

float ok::graphics::ShaderAliasDispatcher::DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return 0.0f;
}

int ok::graphics::ShaderAliasDispatcher::DispatchAliasInt(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return 0;
}

std::pair<float*, int> ok::graphics::ShaderAliasDispatcher::DispatchAliasFloatArray(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return std::pair<float*, int>(nullptr, 0);
}

std::pair<glm::mat4*, int> ok::graphics::ShaderAliasDispatcher::DispatchAliasMat4Array(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return std::pair<glm::mat4*, int>(nullptr, 0);
}

std::pair<unsigned int*, int> ok::graphics::ShaderAliasDispatcher::DispatchAliasSubroutineArray(ok::graphics::ShaderAliasReference alias_type)
{
	unresolved_alias = true;
	return std::pair<unsigned int*, int>();
}
