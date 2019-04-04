#include "Assets.h"

namespace ok
{
	ok::Assets & ok::Assets::instance()
	{
		static ok::Assets s;
		return s;
	}

	ok::graphics::Font * Assets::GetFont(ok::String path)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::Font*>::iterator it = fonts.find(path);

		ok::graphics::Font* font = nullptr;

		if (it == fonts.end())
		{
			font = new ok::graphics::Font();
			fonts[path] = font;
		}
		else
		{
			font = it->second;
		}

		return font;
	}

	ok::graphics::InternalFont * Assets::GetInternalFont(ok::String path)
	{
		ok::String folder;
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			folder = alias_name.get_folder();
			path = assets_root_folder + alias_name;
		}
		else
		{
			folder = path.get_folder();
			path = assets_root_folder + path;
		}

		if (folder.getSize() != 0) folder += "/";

		std::unordered_map<std::string, ok::graphics::InternalFont*>::iterator it = internal_fonts.find(path);

		ok::graphics::InternalFont* font = nullptr;

		if (it == internal_fonts.end())
		{
			font = new ok::graphics::InternalFont();
			std::vector<std::pair<unsigned int, ok::graphics::FontGlyph>> font_glyphs;

			std::string std_path = path;
			tinyxml2::XMLDocument doc;
			doc.LoadFile(std_path.c_str());

			tinyxml2::XMLElement* elem;

			elem = doc.FirstChildElement("font");
			ok::graphics::Texture* tex = GetTexture(folder + ok::String(elem->Attribute("texture")));
			tex->SetSmooth(true);

			font->SetTexture(tex);

			std::ifstream f;
			f.open((const std::string&)(assets_root_folder + folder + ok::String(elem->Attribute("layout"))), std::ifstream::in | std::ifstream::binary);

			int glyphs_count;
			f.read((char*)&glyphs_count, sizeof(int));

			font_glyphs.resize(glyphs_count);

			float font_scale;

			ok::Rect2Df& font_bounds = font->GetFontBounds();
			font_bounds.SetXYWH(0, 0, 0, 0);
			font_bounds.ResetMerge();

			int data4i[4];
			float data4f[4];
			for (auto& glyph : font_glyphs)
			{
				f.read((char*)(&glyph.first), sizeof(unsigned int));

				f.read((char*)(data4i), sizeof(int) * 4);
				glyph.second.texture_rect.SetLTRB(data4i[0], data4i[1], data4i[2], data4i[3]);
				f.read((char*)(data4i), sizeof(int) * 4);
				glyph.second.texture_inner_rect.SetLTRB(data4i[0], data4i[1], data4i[2], data4i[3]);
				f.read((char*)(data4f), sizeof(float) * 4);
				glyph.second.bounds.SetLTRB(data4f[0], data4f[1], data4f[2], data4f[3]);
				f.read((char*)(&glyph.second.advance), sizeof(float));

				glyph.second.unknown = false;

				font_scale = static_cast<float>(glyph.second.texture_inner_rect.GetWidth()) / glyph.second.bounds.GetWidth();
				glyph.second.bounds.ResizeFromCenter(static_cast<float>(glyph.second.texture_inner_rect.GetWidth()), static_cast<float>(glyph.second.texture_inner_rect.GetHeight()));
				glyph.second.advance *= font_scale;

				font_bounds.Merge(glyph.second.bounds);
			}

			std::sort(font_glyphs.begin(), font_glyphs.end(), [](const std::pair<unsigned int, ok::graphics::FontGlyph>& lhs, const std::pair<unsigned int, ok::graphics::FontGlyph>& rhs)
			{
				return lhs.first < rhs.first;
			});

			std::vector<ok::graphics::FontGlyph> & glyphs_data = font->GetGlyphs();
			std::vector<unsigned int> & glyphs_charcodes = font->GetGlyphsCharCodes();

			glyphs_data.reserve(font_glyphs.size());
			glyphs_charcodes.reserve(font_glyphs.size());

			for (auto&& glyph : font_glyphs)
			{
				glyphs_data.push_back(glyph.second);
				glyphs_charcodes.push_back(glyph.first);
			}

			f.close();

			internal_fonts[path] = font;
		}
		else
		{
			font = it->second;
		}

		return font;
	}

	ok::graphics::VertexAnimation * ok::Assets::GetVertexAnimation(ok::String path, ok::graphics::VertexAnimationInfluence influence, bool reload)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::VertexAnimation*>::iterator it = vertex_animations.find(path);

		ok::graphics::VertexAnimation* animation = nullptr;

		if (it == vertex_animations.end() || reload == true)
		{
			std::ifstream f;
			f.open((const std::string&)path, std::ifstream::in | std::ifstream::binary);

			int resource_header;
			f.read((char*)&resource_header, sizeof(int));

			int animation_name_length;
			std::vector<char> animation_name;

			f.read((char*)&animation_name_length, sizeof(int));
			animation_name.resize(animation_name_length);

			f.read((char*)animation_name.data(), sizeof(char)*animation_name_length);

			int animation_frames_count;
			f.read((char*)&animation_frames_count, sizeof(int));

			int animation_vertex_count;
			f.read((char*)&animation_vertex_count, sizeof(int));
			animation_vertex_count /= 3;

			animation = new ok::graphics::VertexAnimation(ok::String(animation_name.data()), animation_frames_count, animation_vertex_count, influence);

			ok::graphics::VertexAnimationFrame* frame;

			for (int i = 0; i < animation_frames_count; i++)
			{
				frame = (ok::graphics::VertexAnimationFrame*)animation->GetFrame(i);

				switch (influence)
				{
				case ok::graphics::VertexAnimationInfluence::Positions:
				{
					f.read((char*)frame->positions, sizeof(float)*animation_vertex_count * 3);
					f.ignore(sizeof(float)*animation_vertex_count * 3);
				}
				break;

				case ok::graphics::VertexAnimationInfluence::Positions_Normals:
				{
					f.read((char*)frame->positions, sizeof(float)*animation_vertex_count * 3);
					f.read((char*)frame->normals, sizeof(float)*animation_vertex_count * 3);
				}
				break;
				}

				if (f.eof())
				{
					int a;
					a = 0;
				}

			}

			f.close();

			vertex_animations[path] = animation;
		}
		else
		{
			animation = it->second;
		}

		return animation;
	}

	ok::graphics::TransformAnimation * ok::Assets::GetTransformAnimation(ok::String path)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::TransformAnimation*>::iterator it = transform_animations.find(path);

		ok::graphics::TransformAnimation* animation = nullptr;

		if (it == transform_animations.end())
		{
			std::ifstream f;
			f.open((const std::string&)path, std::ifstream::in | std::ifstream::binary);

			int descriptor;
			f.read((char*)&descriptor, sizeof(int));

			int animation_name_length;
			std::vector<char> animation_name;

			f.read((char*)&animation_name_length, sizeof(int));
			animation_name.resize(animation_name_length);

			f.read((char*)animation_name.data(), sizeof(char)*animation_name_length);

			int animation_frames_count;
			f.read((char*)&animation_frames_count, sizeof(int));

			int nodes_count;
			f.read((char*)&nodes_count, sizeof(int));

			animation = new ok::graphics::TransformAnimation(ok::String(animation_name.data()), animation_frames_count, nodes_count);

			f.read((char*)animation->GetFramesRawData().data(), sizeof(float) * 9 * animation_frames_count*nodes_count);

			f.close();

			transform_animations[path] = animation;
		}
		else
		{
			animation = it->second;
		}

		return animation;
	}

	ok::graphics::Texture* ok::Assets::GetTexture(ok::String path)
	{
		if (path.getSize() == 0) path = "undefined.png";

		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::Texture*>::iterator it = textures.find(path);

		sf::Texture* _pre_texture;
		ok::graphics::Texture* texture;

		if (it == textures.end())
		{
			_pre_texture = new sf::Texture();
			_pre_texture->loadFromFile(path);

			texture = new ok::graphics::Texture(_pre_texture);

			textures[path] = texture;
		}
		else
		{
			texture = it->second;
		}

		return texture;
	}

	ok::graphics::Shader * ok::Assets::GetShader(ok::String path, bool reload)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::Shader*>::iterator it = shaders.find(path);

		ok::graphics::Shader* shader = nullptr;

		auto F_LoadShader = [](ok::graphics::Shader* target, ok::String& path) {
			unsigned int vertex_shader = 0;
			unsigned int fragment_shader = 0;
			unsigned int geometry_shader = 0;

			std::string std_path = path;//path.toAnsiString();
			tinyxml2::XMLDocument doc;
			doc.LoadFile(std_path.c_str());

			tinyxml2::XMLElement* elem;
			tinyxml2::XMLElement* inner_elem;

			ok::graphics::ShaderOptions& shader_options = target->options;

			elem = doc.FirstChildElement("shader")->FirstChildElement("properties");
			inner_elem = elem->FirstChildElement("transparent");
			shader_options.isTransparentEnabled = inner_elem->BoolAttribute("enabled", false);

			inner_elem = elem->FirstChildElement("blend");
			shader_options.isBlendEnabled = inner_elem->BoolAttribute("enabled", false);

			if (shader_options.isBlendEnabled)
			{
				ok::String template_file = inner_elem->Attribute("template_file");
				if (template_file != "")
				{
					ok::graphics::ShaderOptions& template_options = *(ok::Assets::instance().GetBlendTemplate(template_file));
					bool mem;
					mem = shader_options.isTransparentEnabled;
					shader_options = template_options;
					shader_options.isTransparentEnabled = mem;
					shader_options.isBlendEnabled = true;
				}
				else
				{
					shader_options.blendSeparateEnabled = inner_elem->BoolAttribute("separate", false);

					if (shader_options.blendSeparateEnabled)
					{
						shader_options.blendEquation_Modes[0] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("modeRGB"));
						shader_options.blendEquation_Modes[1] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("modeA"));
						shader_options.blendFunc_Factors[0] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("srcRGB"));
						shader_options.blendFunc_Factors[1] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("dstRGB"));
						shader_options.blendFunc_Factors[2] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("srcA"));
						shader_options.blendFunc_Factors[3] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("dstA"));
					}
					else
					{
						shader_options.blendEquation_Modes[0] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("modeRGBA"));
						shader_options.blendFunc_Factors[0] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("srcRGBA"));
						shader_options.blendFunc_Factors[1] = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("dstRGBA"));
					}

					shader_options.blendColorConstant[0] = inner_elem->FloatAttribute("clrR", 0.f);
					shader_options.blendColorConstant[1] = inner_elem->FloatAttribute("clrG", 0.f);
					shader_options.blendColorConstant[2] = inner_elem->FloatAttribute("clrB", 0.f);
					shader_options.blendColorConstant[3] = inner_elem->FloatAttribute("clrA", 0.f);
				}
			}

			inner_elem = elem->FirstChildElement("cull");
			shader_options.isCullEnabled = inner_elem->BoolAttribute("enabled", false);
			if (shader_options.isCullEnabled)
			{
				shader_options.cullFace = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("face"));
			}

			inner_elem = elem->FirstChildElement("depth");
			shader_options.isDepthEnabled = inner_elem->BoolAttribute("enabled", false);
			if (shader_options.isDepthEnabled)
			{
				shader_options.depthMask = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("depth_mask"));
				shader_options.depthFunc = ok::Assets::instance().GetAliasGL(inner_elem->Attribute("depth_func"));
			}


			elem = doc.FirstChildElement("shader")->FirstChildElement("vertex");
			std::string vertex_shader_source = elem->GetText();

			elem = doc.FirstChildElement("shader")->FirstChildElement("fragment");
			std::string fragment_shader_source = elem->GetText();

			elem = doc.FirstChildElement("shader")->FirstChildElement("geometry");
			std::string geometry_shader_source = elem->GetText();

			const char* gl_source;
			int success;
			char infoLog[512];

			target->isValid = true;

			vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			gl_source = vertex_shader_source.c_str();
			glShaderSource(vertex_shader, 1, &gl_source, nullptr);
			glCompileShader(vertex_shader);
			glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
				LOG(ERROR) << infoLog;
				target->isValid = false;
				vertex_shader = 0;
			}

			fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			gl_source = fragment_shader_source.c_str();
			glShaderSource(fragment_shader, 1, &gl_source, nullptr);
			glCompileShader(fragment_shader);
			glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
				LOG(ERROR) << infoLog;
				target->isValid = false;
				fragment_shader = 0;
			}

			if (geometry_shader_source.length() > 0)
			{
				geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
				gl_source = geometry_shader_source.c_str();
				glShaderSource(geometry_shader, 1, &gl_source, nullptr);
				glCompileShader(geometry_shader);
				glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(geometry_shader, 512, nullptr, infoLog);
					LOG(ERROR) << infoLog;
					target->isValid = false;
					geometry_shader = 0;
				}
			}

			target->shader_program_id = glCreateProgram();

			if (vertex_shader != 0)
				glAttachShader(target->shader_program_id, vertex_shader);
			if (fragment_shader != 0)
				glAttachShader(target->shader_program_id, fragment_shader);
			if (geometry_shader != 0)
				glAttachShader(target->shader_program_id, geometry_shader);

			glLinkProgram(target->shader_program_id);
			glGetProgramiv(target->shader_program_id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(target->shader_program_id, 512, nullptr, infoLog);
				LOG(ERROR) << infoLog;
				target->isValid = false;
				target->shader_program_id = 0;
			}

			int uniforms_count;
			GLint bufSize; // maximum name length
			glGetProgramiv(target->shader_program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);

			GLchar* uniform_name = new GLchar[bufSize]; // variable name in GLSL
			GLsizei uniform_name_length; // name length
			GLint	uniform_size; // size of the variable
			GLenum	uniform_type; // type of the variable (float, vec3 or mat4, etc)

			glGetProgramiv(target->shader_program_id, GL_ACTIVE_UNIFORMS, &uniforms_count);
			for (int i = 0; i < uniforms_count; i++)
			{
				glGetActiveUniform(target->shader_program_id, (GLuint)i, bufSize, &uniform_name_length, &uniform_size, &uniform_type, uniform_name);

				if (uniform_type == GL_SAMPLER_2D)
					target->RegisterUniform_Sampler(uniform_name, glGetUniformLocation(target->shader_program_id, uniform_name));
				else
					target->RegisterUniform(uniform_name, glGetUniformLocation(target->shader_program_id, uniform_name));
			}

			delete[] uniform_name;

			//load vertex stage subroutines
			std::vector<GLenum> shader_stages = { GL_VERTEX_SHADER , GL_FRAGMENT_SHADER };

			for (auto&& stage : shader_stages)
			{
				glGetProgramStageiv(target->shader_program_id, stage, GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH, &bufSize);
				uniform_name = new GLchar[bufSize];

				glGetProgramStageiv(target->shader_program_id, stage, GL_ACTIVE_SUBROUTINE_UNIFORMS, &uniforms_count);
				for (int i = 0; i < uniforms_count; i++)
				{
					glGetActiveSubroutineUniformName(target->shader_program_id, stage, (GLuint)i, bufSize, &uniform_size, uniform_name);
					target->RegisterSubroutine_Uniform(stage, uniform_name, glGetSubroutineUniformLocation(target->shader_program_id, stage, uniform_name));
				}

				delete[] uniform_name;

				glGetProgramStageiv(target->shader_program_id, stage, GL_ACTIVE_SUBROUTINE_MAX_LENGTH, &bufSize);
				uniform_name = new GLchar[bufSize];

				glGetProgramStageiv(target->shader_program_id, stage, GL_ACTIVE_SUBROUTINES, &uniforms_count);
				for (int i = 0; i < uniforms_count; i++)
				{
					glGetActiveSubroutineName(target->shader_program_id, stage, (GLuint)i, bufSize, &uniform_size, uniform_name);

					target->RegisterSubroutine(stage, uniform_name, glGetSubroutineIndex(target->shader_program_id, stage, uniform_name));
				}

				delete[] uniform_name;
			}

			//load aliases subroutines
			elem = doc.FirstChildElement("shader")->FirstChildElement("aliases_subroutines");

			if (elem != nullptr)
			{
				for (tinyxml2::XMLElement* e = elem->FirstChildElement("alias"); e != nullptr; e = e->NextSiblingElement("alias"))
				{
					target->RegisterAliasSubroutine(e->Attribute("uniform"), e->Attribute("type"), e->Attribute("ref"));
				}
			}

			//load aliases
			elem = doc.FirstChildElement("shader")->FirstChildElement("aliases");

			if (elem != nullptr)
			{
				for (tinyxml2::XMLElement* e = elem->FirstChildElement("alias"); e != nullptr; e = e->NextSiblingElement("alias"))
				{
					target->RegisterAlias(e->Attribute("uniform"), e->Attribute("type"), e->Attribute("ref"));
				}
			}


			if (vertex_shader != 0)
				glDeleteShader(vertex_shader);

			if (fragment_shader != 0)
				glDeleteShader(fragment_shader);

			if (geometry_shader != 0)
				glDeleteShader(geometry_shader);
		};

		if (it == shaders.end())
		{
			shader = new ok::graphics::Shader();

			F_LoadShader(shader, path);
			ok::graphics::Shader::RebindCurrentShader();

			shaders[path] = shader;
		}
		else
		{
			shader = it->second;

			if (reload == true)
			{
				F_LoadShader(shader, path);
				ok::graphics::Shader::RebindCurrentShader();
			}
		}

		return shader;
	}

	ok::graphics::Mesh * ok::Assets::GetMesh(ok::String path, bool prefer_hardware_skin)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::Mesh*>::iterator it = meshes.find(path);

		ok::graphics::Mesh* mesh = nullptr;

		if (it == meshes.end())
		{
			std::ifstream f;
			f.open((const std::string&)path, std::ifstream::in | std::ifstream::binary);

			/*
			[0] - descriptor
			[1] - vertex_count * 3 (for vertices)
			[2] - texcoord_channels_count
			[3] - vertex_count * 2 (for texcoord)
			[4] - triangles_count * 3 (for indices)
			[5] - vertex_count * 3 (for normals)
			*/
			int header[6];

			f.read((char*)header, sizeof(int) * 6);

			mesh = new ok::graphics::Mesh(header[1] / 3, header[4] / 3, header[2]);
			mesh->EnableNormals();

			//positions
			f.read((char*)mesh->GetVertices().vertices, sizeof(float) * header[1]);

			//texcoords
			for (int i = 0; i < header[2]; i++)
			{
				f.read((char*)mesh->GetUV(i).vertices, sizeof(float) * header[3]);
			}

			//triangles (indices)
			f.read((char*)mesh->GetIndices().indices, sizeof(int) * header[4]);

			//normals
			f.read((char*)mesh->GetNormals().vertices, sizeof(float) * header[5]);

			//read skin data
			if (header[0] == static_cast<int>(ok::ResourceDescriptor::SkinnedMesh))
			{
				int bones_count;
				f.read((char*)&bones_count, sizeof(int));

				mesh->EnableSkinDeformer(bones_count, prefer_hardware_skin);

				std::vector<std::string>* _mesh_to_skeleton_bones_names = new std::vector<std::string>();

				int bone_name_length;
				std::vector<char> bone_name;

				_mesh_to_skeleton_bones_names->reserve(bones_count);

				for (int i = 0; i < bones_count; i++)
				{
					f.read((char*)&bone_name_length, sizeof(int));
					bone_name.resize(bone_name_length);
					f.read((char*)bone_name.data(), sizeof(char)*bone_name_length);

					_mesh_to_skeleton_bones_names->push_back(std::string(bone_name.data()));
				}

				mesh_to_skeleton_bones_names[path] = _mesh_to_skeleton_bones_names;

				int skin_vertex_count;
				f.read((char*)&skin_vertex_count, sizeof(int));

				int bone_index;
				float bone_weight;

				std::vector<int> indexes;
				std::vector<float> weights;

				indexes.reserve(skin_vertex_count * 4);
				weights.reserve(skin_vertex_count * 4);

				for (int i = 0; i < skin_vertex_count; i++)
				{
					f.read((char*)&bones_count, sizeof(int));

					for (int j = 0; j < 4; j++)
					{
						if (j < bones_count)
						{
							f.read((char*)&bone_index, sizeof(int));
							f.read((char*)&bone_weight, sizeof(float));
						}
						else
						{
							bone_index = -1;
							bone_weight = 0.f;
						}

						indexes.push_back(bone_index);
						weights.push_back(bone_weight);
					}
				}

				mesh->UploadSkinIndexes(indexes.data());
				mesh->UploadSkinWeights(weights.data());
			}

			mesh->UpdateVertices();
			mesh->UpdateUV();
			mesh->UpdateIndices();
			mesh->UpdateNormals();

			f.close();

			meshes[path] = mesh;
		}
		else
		{
			mesh = it->second;
		}

		return mesh;
	}

	ok::graphics::Material * ok::Assets::GetMaterial(ok::String path)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::Material*>::iterator it = materials.find(path);

		ok::graphics::Material* material = nullptr;
		ok::graphics::Shader* shader = nullptr;
		int texture_slot_index;

		if (it == materials.end())
		{
			std::string std_path = path;
			tinyxml2::XMLDocument doc;
			doc.LoadFile(std_path.c_str());

			tinyxml2::XMLElement* elem;
			tinyxml2::XMLElement* inner_elem_textures;

			elem = doc.FirstChildElement("material");
			inner_elem_textures = elem->FirstChildElement("textures");

			shader = GetShader(elem->Attribute("shader"));
			material = new ok::graphics::Material(shader, inner_elem_textures->IntAttribute("count"));

			ok::graphics::Texture* texture = nullptr;

			for (tinyxml2::XMLElement* e = inner_elem_textures->FirstChildElement("slot"); e != nullptr; e = e->NextSiblingElement("slot"))
			{
				texture_slot_index = e->IntAttribute("index");
				material->LinkSlotToSampler(e->Attribute("sampler"), texture_slot_index);
				texture = GetTexture(e->Attribute("asset"));

				material->SetTexture(texture_slot_index, texture);
				material->SetSlotProperties(texture_slot_index, e->BoolAttribute("smooth"), GetAliasGL(e->Attribute("wrapping")), ok::Color(static_cast<unsigned char>(e->IntAttribute("r", 0)), static_cast<unsigned char>(e->IntAttribute("g", 0)), static_cast<unsigned char>(e->IntAttribute("b", 0)), static_cast<unsigned char>(255)), e->BoolAttribute("fixed_asset", false));
			}

			materials[path] = material;
		}
		else
		{
			material = it->second;
		}

		return material;
	}

	ok::sound::SoundAsset * Assets::GetSoundSample(ok::String path)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::sound::SoundAsset*>::iterator it = sound_samples.find(path);

		ok::sound::SoundAsset* sound_asset = nullptr;

		if (it == sound_samples.end())
		{
			sound_asset = new ok::sound::SoundAsset();
			sound_asset->_sample_resource = new sf::SoundBuffer();
			sound_asset->_sample_resource->loadFromFile(path);

			sound_samples[path] = sound_asset;
		}
		else
		{
			sound_asset = it->second;
		}

		return sound_asset;
	}

	ok::sound::SoundAsset * Assets::GetSoundStream(ok::String path)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::sound::SoundAsset*>::iterator it = sound_streams.find(path);

		ok::sound::SoundAsset* sound_asset = nullptr;

		if (it == sound_streams.end())
		{
			sound_asset = new ok::sound::SoundAsset();
			sound_asset->_stream_resource = path;
			sound_streams[path] = sound_asset;
		}
		else
		{
			sound_asset = it->second;
		}

		/*if (sound_asset->_stream_resource == nullptr)
		{
			std::shared_ptr<sf::FileInputStream> stream = std::make_shared<sf::FileInputStream>();
			stream->open(path);
			sound_asset->_stream_resource = stream;
		}*/

		return sound_asset;
	}

	ok::graphics::ShaderOptions * ok::Assets::GetBlendTemplate(ok::String path)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::ShaderOptions*>::iterator it = blend_templates.find(path);

		ok::graphics::ShaderOptions* blend_template = nullptr;

		if (it == blend_templates.end())
		{
			blend_template = new ok::graphics::ShaderOptions();

			std::string std_path = path;
			tinyxml2::XMLDocument doc;
			doc.LoadFile(std_path.c_str());

			tinyxml2::XMLElement* elem;

			ok::graphics::ShaderOptions& shader_options = *blend_template;

			elem = doc.FirstChildElement("blend");

			shader_options.blendSeparateEnabled = elem->BoolAttribute("separate", false);

			if (shader_options.blendSeparateEnabled)
			{
				shader_options.blendEquation_Modes[0] = GetAliasGL(elem->Attribute("modeRGB"));
				shader_options.blendEquation_Modes[1] = GetAliasGL(elem->Attribute("modeA"));
				shader_options.blendFunc_Factors[0] = GetAliasGL(elem->Attribute("srcRGB"));
				shader_options.blendFunc_Factors[1] = GetAliasGL(elem->Attribute("dstRGB"));
				shader_options.blendFunc_Factors[2] = GetAliasGL(elem->Attribute("srcA"));
				shader_options.blendFunc_Factors[3] = GetAliasGL(elem->Attribute("dstA"));
			}
			else
			{
				shader_options.blendEquation_Modes[0] = GetAliasGL(elem->Attribute("modeRGBA"));
				shader_options.blendFunc_Factors[0] = GetAliasGL(elem->Attribute("srcRGBA"));
				shader_options.blendFunc_Factors[1] = GetAliasGL(elem->Attribute("dstRGBA"));
			}

			shader_options.blendColorConstant[0] = elem->FloatAttribute("clrR", 0.f);
			shader_options.blendColorConstant[1] = elem->FloatAttribute("clrG", 0.f);
			shader_options.blendColorConstant[2] = elem->FloatAttribute("clrB", 0.f);
			shader_options.blendColorConstant[3] = elem->FloatAttribute("clrA", 0.f);

			blend_templates[path] = blend_template;
		}
		else
		{
			blend_template = it->second;
		}

		return blend_template;
	}

	ok::graphics::SpriteAtlas * Assets::GetSpriteAtlas(ok::String path)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::graphics::SpriteAtlas*>::iterator it = sprite_atlases.find(path);

		ok::graphics::SpriteAtlas* atlas;

		if (it == sprite_atlases.end())
		{
			atlas = new ok::graphics::SpriteAtlas();

			std::string std_path = path;
			tinyxml2::XMLDocument doc;
			doc.LoadFile(std_path.c_str());

			tinyxml2::XMLElement* elem;
			tinyxml2::XMLElement* inner_elem;
			tinyxml2::XMLElement* sequence_inner_elem;

			elem = doc.FirstChildElement("atlas");

			ok::graphics::SpriteInfo sprite;
			ok::String item_name;

			for (inner_elem = elem->FirstChildElement("sprite"); inner_elem != nullptr; inner_elem = inner_elem->NextSiblingElement("sprite"))
			{
				sprite.rect = ok::graphics::TextureRect(
					GetTexture(inner_elem->Attribute("texture")),
					inner_elem->IntAttribute("left", 0),
					inner_elem->IntAttribute("top", 0),
					inner_elem->IntAttribute("width", 0),
					inner_elem->IntAttribute("height", 0));
				sprite.hotspot = glm::vec2(inner_elem->FloatAttribute("hotspot_x", 0.5f), inner_elem->FloatAttribute("hotspot_y", 0.5f));
				sprite.scale = glm::vec2(inner_elem->FloatAttribute("scale_x", 1.0f), inner_elem->FloatAttribute("scale_y", 1.0f));
				sprite.tint_color = ok::Color(
					static_cast<unsigned char>(inner_elem->IntAttribute("tint_r", 0)),
					static_cast<unsigned char>(inner_elem->IntAttribute("tint_g", 0)),
					static_cast<unsigned char>(inner_elem->IntAttribute("tint_b", 0)),
					static_cast<unsigned char>(inner_elem->IntAttribute("tint_a", 0))
				);
				sprite.tint_power = inner_elem->FloatAttribute("tint_a", 0.f) / 255.0f;
				sprite.flip_x = inner_elem->BoolAttribute("flip_x", false);
				sprite.flip_y = inner_elem->BoolAttribute("flip_y", false);

				item_name = inner_elem->Attribute("name");
				atlas->AddSprite(sprite, item_name);
			}

			for (inner_elem = elem->FirstChildElement("sequence"); inner_elem != nullptr; inner_elem = inner_elem->NextSiblingElement("sequence"))
			{
				atlas->BeginSequence();

					for (sequence_inner_elem = inner_elem->FirstChildElement("sprite"); sequence_inner_elem != nullptr; sequence_inner_elem = sequence_inner_elem->NextSiblingElement("sprite"))
					{
						sprite.rect = ok::graphics::TextureRect(
							GetTexture(sequence_inner_elem->Attribute("texture")),
							sequence_inner_elem->IntAttribute("left", 0),
							sequence_inner_elem->IntAttribute("top", 0),
							sequence_inner_elem->IntAttribute("width", 0),
							sequence_inner_elem->IntAttribute("height", 0));
						sprite.hotspot = glm::vec2(sequence_inner_elem->FloatAttribute("hotspot_x", 0.5f), sequence_inner_elem->FloatAttribute("hotspot_y", 0.5f));
						sprite.scale = glm::vec2(sequence_inner_elem->FloatAttribute("scale_x", 1.0f), sequence_inner_elem->FloatAttribute("scale_y", 1.0f));
						sprite.tint_color = ok::Color(
							static_cast<unsigned char>(sequence_inner_elem->IntAttribute("tint_r", 0)),
							static_cast<unsigned char>(sequence_inner_elem->IntAttribute("tint_g", 0)),
							static_cast<unsigned char>(sequence_inner_elem->IntAttribute("tint_b", 0)),
							static_cast<unsigned char>(sequence_inner_elem->IntAttribute("tint_a", 0))
						);
						sprite.tint_power = sequence_inner_elem->FloatAttribute("tint_a", 0.f) / 255.0f;
						sprite.flip_x = sequence_inner_elem->BoolAttribute("flip_x", false);
						sprite.flip_y = sequence_inner_elem->BoolAttribute("flip_y", false);

						item_name = sequence_inner_elem->Attribute("name");
						atlas->AddSprite(sprite, item_name);
					}

				atlas->EndSequence(ok::String(inner_elem->Attribute("name")));
			}

			sprite_atlases[path] = atlas;
		}
		else
		{
			atlas = it->second;
		}

		return atlas;

		return nullptr;
	}

	GLenum ok::Assets::GetAliasGL(ok::String alias)
	{
		if (_glname_alias.size() == 0)
		{
			_glname_alias["GL_ZERO"] = GL_ZERO;
			_glname_alias["GL_ONE"] = GL_ONE;
			_glname_alias["GL_SRC_COLOR "] = GL_SRC_COLOR;
			_glname_alias["GL_ONE_MINUS_SRC_COLOR"] = GL_ONE_MINUS_SRC_COLOR;
			_glname_alias["GL_DST_COLOR"] = GL_DST_COLOR;
			_glname_alias["GL_ONE_MINUS_DST_COLOR"] = GL_ONE_MINUS_DST_COLOR;
			_glname_alias["GL_SRC_ALPHA"] = GL_SRC_ALPHA;
			_glname_alias["GL_ONE_MINUS_SRC_ALPHA"] = GL_ONE_MINUS_SRC_ALPHA;
			_glname_alias["GL_DST_ALPHA"] = GL_DST_ALPHA;
			_glname_alias["GL_ONE_MINUS_DST_ALPHA"] = GL_ONE_MINUS_DST_ALPHA;
			_glname_alias["GL_CONSTANT_COLOR"] = GL_CONSTANT_COLOR;
			_glname_alias["GL_ONE_MINUS_CONSTANT_COLOR"] = GL_ONE_MINUS_CONSTANT_COLOR;
			_glname_alias["GL_CONSTANT_ALPHA"] = GL_CONSTANT_ALPHA;
			_glname_alias["GL_ONE_MINUS_CONSTANT_ALPHA"] = GL_ONE_MINUS_CONSTANT_ALPHA;
			_glname_alias["GL_SRC_ALPHA_SATURATE"] = GL_SRC_ALPHA_SATURATE;
			_glname_alias["GL_SRC1_COLOR"] = GL_SRC1_COLOR;
			_glname_alias["GL_ONE_MINUS_SRC1_COLOR"] = GL_ONE_MINUS_SRC1_COLOR;
			_glname_alias["GL_SRC1_ALPHA"] = GL_SRC1_ALPHA;
			_glname_alias["GL_ONE_MINUS_SRC1_ALPHA"] = GL_ONE_MINUS_SRC1_ALPHA;
			_glname_alias["GL_FUNC_ADD"] = GL_FUNC_ADD;
			_glname_alias["GL_FUNC_SUBTRACT"] = GL_FUNC_SUBTRACT;
			_glname_alias["GL_FUNC_REVERSE_SUBTRACT"] = GL_FUNC_REVERSE_SUBTRACT;
			_glname_alias["GL_MIN"] = GL_MIN;
			_glname_alias["GL_MAX"] = GL_MAX;
			_glname_alias["GL_FRONT"] = GL_FRONT;
			_glname_alias["GL_BACK"] = GL_BACK;
			_glname_alias["GL_FRONT_AND_BACK"] = GL_FRONT_AND_BACK;
			_glname_alias["GL_NEVER"] = GL_NEVER;
			_glname_alias["GL_LESS"] = GL_LESS;
			_glname_alias["GL_EQUAL"] = GL_EQUAL;
			_glname_alias["GL_LEQUAL"] = GL_LEQUAL;
			_glname_alias["GL_GREATER"] = GL_GREATER;
			_glname_alias["GL_NOTEQUAL"] = GL_NOTEQUAL;
			_glname_alias["GL_GEQUAL"] = GL_GEQUAL;
			_glname_alias["GL_ALWAYS"] = GL_ALWAYS;
			_glname_alias["GL_TRUE"] = GL_TRUE;
			_glname_alias["GL_FALSE"] = GL_FALSE;
			_glname_alias["GL_REPEAT"] = GL_REPEAT;
			_glname_alias["GL_MIRRORED_REPEAT"] = GL_MIRRORED_REPEAT;
			_glname_alias["GL_CLAMP_TO_EDGE"] = GL_CLAMP_TO_EDGE;
			_glname_alias["GL_CLAMP_TO_BORDER"] = GL_CLAMP_TO_BORDER;
			_glname_alias["GL_CLAMP"] = GL_CLAMP;
		}

		return _glname_alias[alias];
	}

	ok::String ok::Assets::GetAliasName(ok::String alias)
	{
		return _name_alias[alias];
	}

	ok::GameObject * ok::Assets::GetModel(ok::String path, bool get_initial_model)
	{
		ok::String folder;
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			folder = alias_name.get_folder() + ok::String("/");
			path = assets_root_folder + alias_name;
		}
		else
		{
			folder = path.get_folder() + ok::String("/");
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::GameObject*>::iterator it = models.find(path);

		ok::GameObject* model = nullptr;

		if (it == models.end())
		{
			model = new ok::GameObject();

			//load model
			std::string std_path = path;
			tinyxml2::XMLDocument doc;
			doc.LoadFile(std_path.c_str());

			tinyxml2::XMLElement* elem;
			tinyxml2::XMLElement* resources_elem;
			tinyxml2::XMLElement* nodes_elem;

			std::unordered_map<std::string, std::pair<ok::ResourceDescriptor, void*>> resources;
			std::unordered_map<std::string, std::pair<ok::ResourceDescriptor, void*>>* resources_ptr = &resources;

			elem = doc.FirstChildElement("Model");
			resources_elem = elem->FirstChildElement("Resources");
			nodes_elem = elem->FirstChildElement("Nodes");

			std::function<void(ok::GameObject*, tinyxml2::XMLElement*)> F_LoadNode = [&F_LoadNode, resources_ptr](ok::GameObject* parent, tinyxml2::XMLElement* node) {

				tinyxml2::XMLElement* chunk;
				ok::GameObject* game_object = new ok::GameObject();

				chunk = node->FirstChildElement("Position");
				game_object->SetPosition(glm::vec3(chunk->FloatAttribute("x", 0.f), chunk->FloatAttribute("y", 0.f), chunk->FloatAttribute("z", 0.f)));
				chunk = node->FirstChildElement("Rotation");
				game_object->SetRotation(glm::vec3(chunk->FloatAttribute("x", 0.f), chunk->FloatAttribute("y", 0.f), chunk->FloatAttribute("z", 0.f)));
				chunk = node->FirstChildElement("Scale");
				game_object->SetScale(glm::vec3(chunk->FloatAttribute("x", 0.f), chunk->FloatAttribute("y", 0.f), chunk->FloatAttribute("z", 0.f)));

				std::pair<ok::ResourceDescriptor, void*> resource_info;

				for (chunk = node->FirstChildElement("Resource"); chunk != nullptr; chunk = chunk->NextSiblingElement("Resource"))
				{
					resource_info = (*resources_ptr)[chunk->Attribute("Name")];

					switch (resource_info.first)
					{
					case ok::ResourceDescriptor::Mesh:
					{
						ok::graphics::MeshRenderer* mesh_renderer = new ok::graphics::MeshRenderer();
						game_object->AddComponent(mesh_renderer);

						mesh_renderer->SetMesh((ok::graphics::Mesh*)resource_info.second);
					}
					break;

					case ok::ResourceDescriptor::SkinnedMesh:
					{
						ok::graphics::MeshRenderer* mesh_renderer = new ok::graphics::MeshRenderer();
						game_object->AddComponent(mesh_renderer);

						mesh_renderer->SetMesh((ok::graphics::Mesh*)resource_info.second);
					}
					break;

					case ok::ResourceDescriptor::TransformAnimation:
					{
					}
					break;

					case ok::ResourceDescriptor::VertexAnimation:
					{
					}
					break;

					case ok::ResourceDescriptor::SkeletalAnimation:
					{
					}
					break;
					}
				}

				if (parent != nullptr)
				{
					parent->AddChild(game_object);
				}

				for (chunk = node->FirstChildElement("Node"); chunk != nullptr; chunk = chunk->NextSiblingElement("Node"))
				{
					F_LoadNode(game_object, chunk);
				}
			};

			//load resources
			ok::String resource_name;
			ok::String resource_file;
			ok::String resource_type;

			for (tinyxml2::XMLElement* e = resources_elem->FirstChildElement("Resource"); e != nullptr; e = e->NextSiblingElement("Resource"))
			{
				resource_name = ok::String(e->Attribute("Name"));
				resource_file = folder + ok::String(e->Attribute("File"));

				resource_type = e->Attribute("Type");

				if (resource_type == "Mesh")
				{
					resources[resource_name] = std::pair<ok::ResourceDescriptor, void*>(ok::ResourceDescriptor::Mesh, GetMesh(resource_file));
				}
				else if (resource_type == "SkinnedMesh")
				{
					resources[resource_name] = std::pair<ok::ResourceDescriptor, void*>(ok::ResourceDescriptor::SkinnedMesh, GetMesh(resource_file));
				}
				else if (resource_type == "TransformAnimation")
				{
					//GetAnimation(resource_name);
				}
				else if (resource_type == "VertexAnimation")
				{
					//GetAnimation(resource_name);
				}
				else if (resource_type == "SkeletalAnimation")
				{
					//GetAnimation(resource_name);
				}
			}

			//load nodes
			model->Rename(path.get_file_name_ext());

			for (tinyxml2::XMLElement* e = nodes_elem->FirstChildElement("Node"); e != nullptr; e = e->NextSiblingElement("Node"))
			{
				F_LoadNode(model, e);
			}

			models[path] = model;
		}
		else
		{
			model = it->second;
		}

		return (get_initial_model == true) ? model : model->Duplicate();
	}

	ok::GameObject * ok::Assets::GetSkeleton(ok::String path, bool get_initial_skeleton)
	{
		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, ok::GameObject*>::iterator it = skeletons.find(path);

		ok::GameObject* skeleton = nullptr;

		if (it == skeletons.end())
		{
			std::ifstream f;
			f.open((const std::string&)path, std::ifstream::in | std::ifstream::binary);

			std::vector<ok::GameObject*> _bones;
			std::vector<float> _bones_bind_pose_trs;
			std::vector<glm::mat4> _bones_bind_pose_inv_mat;

			int descriptor;
			f.read((char*)&descriptor, sizeof(int));

			int skeleton_id = skeletons.size();

			std::function<ok::GameObject*(std::ifstream&)> parse_node = [&parse_node, &_bones, &_bones_bind_pose_trs, skeleton_id](std::ifstream& f) -> ok::GameObject* {

				ok::GameObject* node = new ok::GameObject();

				node->AddComponent(new ok::graphics::TransformAnimationBone(skeleton_id, _bones.size()));

				_bones.push_back(node);

				int animation_name_length;
				std::vector<char> animation_name;

				f.read((char*)&animation_name_length, sizeof(int));
				animation_name.resize(animation_name_length);

				f.read((char*)animation_name.data(), sizeof(char)*animation_name_length);

				float trs[9];
				f.read((char*)trs, sizeof(float) * 9);

				_bones_bind_pose_trs.insert(_bones_bind_pose_trs.end(), &trs[0], &trs[0] + 9);


				node->Rename(ok::String(animation_name.data()));

				node->BeginTransform(ok::TransformSpace::LocalSpace);

				node->SetPosition(glm::vec3(trs[0], trs[1], trs[2]));
				node->SetRotation(glm::vec3(trs[3], trs[4], trs[5]));
				node->SetScale(glm::vec3(trs[6], trs[7], trs[8]));

				node->EndTransform(false);


				char bracket;
				int nodes_count;

				f.read((char*)&bracket, sizeof(char));
				f.read((char*)&nodes_count, sizeof(int));

				for (int i = 0; i < nodes_count; i++)
				{
					node->AddChild(parse_node(f), false);
				}

				f.read((char*)&bracket, sizeof(char));

				return node;
			};


			f.ignore(sizeof(char));
			f.ignore(sizeof(int));

			skeleton = parse_node(f);

			f.ignore(sizeof(char));

			f.close();


			_bones_bind_pose_inv_mat.reserve(_bones.size());

			/*int skeleton_bone_index = 0;

			for (auto&& _bone : _bones)
			{
				_bone->BeginTransform();
				_bone->SetPosition(
					glm::vec3(
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 0],
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 1],
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 2]
					)
				);
				_bone->SetRotation(
					glm::vec3(
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 3],
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 4],
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 5]
					)
				);
				_bone->SetScale(
					glm::vec3(
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 6],
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 7],
						_bones_bind_pose_trs[skeleton_bone_index * 9 + 8]
					)
				);
				_bone->EndTransform();

				skeleton_bone_index++;
			}*/

			for (auto&& _bone : _bones)
			{
				_bones_bind_pose_inv_mat.push_back(glm::inverse(_bone->GetAbsoluteTransformMatrix()));
			}

			ok::graphics::TransformAnimationSkeleton* skeleton_skin_asset = new ok::graphics::TransformAnimationSkeleton(skeleton_id);
			skeleton_skin_asset->GetBones() = _bones;
			skeleton_skin_asset->GetBindPose() = _bones_bind_pose_trs;
			skeleton_skin_asset->GetBindPoseInvMat() = _bones_bind_pose_inv_mat;
			skeleton->AddComponent(skeleton_skin_asset);

			skeletons[path] = skeleton;
		}
		else
		{
			skeleton = it->second;
		}

		return (get_initial_skeleton == true) ? skeleton : skeleton->Duplicate();
	}

	std::vector<int>* ok::Assets::GetMeshToSkeletonMappingTable(ok::String mesh_path, ok::String skeleton_path, bool rebuild)
	{
		ok::String path = mesh_path + skeleton_path;

		ok::String alias_name = GetAliasName(path);
		if (!alias_name.isEmpty())
		{
			path = assets_root_folder + alias_name;
		}
		else
		{
			path = assets_root_folder + path;
		}

		std::unordered_map<std::string, std::vector<int>*>::iterator it = mesh_to_skeleton_mapping_tables.find(path);

		std::vector<int>* mapping_table = nullptr;

		if (it == mesh_to_skeleton_mapping_tables.end() || rebuild == true)
		{
			ok::graphics::Mesh* mesh = GetMesh(mesh_path);
			ok::GameObject* skeleton = GetSkeleton(skeleton_path, true);
			std::vector<ok::GameObject*>& skeleton_bones = skeleton->GetComponent<ok::graphics::TransformAnimationSkeleton>()->GetBones();

			ok::String mesh_asset_path = mesh_path;
			ok::String mesh_alias_name = GetAliasName(mesh_asset_path);
			if (!mesh_alias_name.isEmpty())
			{
				mesh_asset_path = assets_root_folder + mesh_alias_name;
			}
			else
			{
				mesh_asset_path = assets_root_folder + mesh_asset_path;
			}

			std::vector<std::string>* _mesh_to_skeleton_bones_names = mesh_to_skeleton_bones_names[mesh_asset_path];

			if (it == mesh_to_skeleton_mapping_tables.end())
			{
				mapping_table = new std::vector<int>();
				mesh_to_skeleton_mapping_tables[path] = mapping_table;
			}
			else
			{
				mapping_table = it->second;
				mapping_table->resize(0);
			}

			mapping_table->reserve(_mesh_to_skeleton_bones_names->size());

			for (auto&& bone_name : *_mesh_to_skeleton_bones_names)
			{
				size_t bone_name_hash = std::hash<std::string>()(bone_name);

				auto ptr = std::find_if
				(
					skeleton_bones.begin(),
					skeleton_bones.end(),
					[&bone_name, &bone_name_hash](ok::GameObject*& item)
				{
					return ((item->name_hash == bone_name_hash) && (item->name == bone_name));
				}
				);

				if (ptr == skeleton_bones.end())
				{
					//no such bone in skeleton
					mapping_table->push_back(-1);
				}
				else
				{
					//bone found
					mapping_table->push_back(static_cast<int>(ptr - skeleton_bones.begin()/*std::distance(skeleton_bones.begin(), ptr)*/));
				}
			}
		}
		else
		{
			mapping_table = it->second;
		}

		return mapping_table;
	}

	void ok::Assets::DeclareAlias(ok::String alias, ok::String name)
	{
		_name_alias[alias] = name;
	}
}
