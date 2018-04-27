#pragma once

#include "AssetsBasic.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "VertexAnimation.h"
#include "TransformAnimation.h"
#include "Sound.h"
#include "SpriteBatch.h"

namespace ok
{
	enum class ResourceDescriptor
	{
		Mesh = 1001,
		SkinnedMesh = 1002,
		TransformAnimation = 2001,
		VertexAnimation = 2002,
		SkeletalAnimation = 2003,
		Skeleton = 3001
	};

	class Assets : public ok::AssetsBasic
	{
	public:
		ok::String assets_root_folder;

		static ok::Assets& instance();

		ok::graphics::Font* GetFont(ok::String path);
		ok::graphics::InternalFont* GetInternalFont(ok::String path);

		ok::graphics::VertexAnimation* GetVertexAnimation(
			ok::String path,
			ok::graphics::VertexAnimationInfluence components = ok::graphics::VertexAnimationInfluence::Positions_Normals,
			bool reload = false);
		ok::graphics::TransformAnimation* GetTransformAnimation(ok::String path);


		ok::graphics::Texture* GetTexture(ok::String path);
		ok::graphics::Shader* GetShader(ok::String path, bool reload = false);
		ok::graphics::Mesh* GetMesh(ok::String path, bool prefer_hardware_skin = false);
		ok::graphics::Material* GetMaterial(ok::String path);

		ok::sound::SoundAsset* GetSoundSample(ok::String path);
		ok::sound::SoundAsset* GetSoundStream(ok::String path);

		ok::graphics::ShaderOptions* GetBlendTemplate(ok::String path);

		ok::graphics::SpriteAtlas* GetSpriteAtlas(ok::String path);

		GLenum GetAliasGL(ok::String alias);
		ok::String GetAliasName(ok::String alias);

		ok::GameObject* GetModel(ok::String path, bool get_initial_model = false);
		ok::GameObject* GetSkeleton(ok::String path, bool get_initial_skeleton = false);
		std::vector<int>* GetMeshToSkeletonMappingTable(ok::String mesh_path, ok::String skeleton_path, bool rebuild = false);

		void DeclareAlias(ok::String alias, ok::String name);

	private:
		Assets() { basic_instance = this; }
		~Assets() {}
		ok::Assets(ok::Assets const&) { basic_instance = this; }
		ok::Assets& operator= (ok::Assets const&) { basic_instance = this; }
	protected:

		std::unordered_map<std::string, ok::graphics::Font*> fonts;
		std::unordered_map<std::string, ok::graphics::InternalFont*> internal_fonts;

		std::unordered_map<std::string, ok::graphics::VertexAnimation*> vertex_animations;
		std::unordered_map<std::string, ok::graphics::TransformAnimation*> transform_animations;

		std::unordered_map<std::string, ok::graphics::Texture*> textures;
		std::unordered_map<std::string, ok::graphics::Shader*> shaders;
		std::unordered_map<std::string, ok::graphics::Mesh*> meshes;
		std::unordered_map<std::string, ok::graphics::Material*> materials;
		std::unordered_map<std::string, ok::graphics::ShaderOptions*> blend_templates;
		std::unordered_map<std::string, ok::GameObject*> models;
		std::unordered_map<std::string, ok::GameObject*> skeletons;
		std::unordered_map<std::string, std::vector<int>*> mesh_to_skeleton_mapping_tables;
		std::unordered_map<std::string, std::vector<std::string>*> mesh_to_skeleton_bones_names;

		std::unordered_map<std::string, ok::sound::SoundAsset*> sound_samples;
		std::unordered_map<std::string, ok::sound::SoundAsset*> sound_streams;

		std::unordered_map<std::string, ok::graphics::SpriteAtlas*> sprite_atlases;

		std::unordered_map<std::string, GLenum> _glname_alias;
		std::unordered_map<std::string, ok::String> _name_alias;
	};
}