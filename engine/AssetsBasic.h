#pragma once

#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Font.h"

namespace ok
{
	class Assets;

	namespace sound
	{
		typedef sf::Sound Sample;
		typedef sf::Music Stream;

		class Sound;

		class SoundAsset
		{
		public:
			SoundAsset();
		private:
			friend class ok::sound::Sound;
			friend class ok::Assets;

			void Apply(ok::sound::Sample& sample);
			void Apply(ok::sound::Stream& stream);
			bool IsSample();
			bool IsStream();
			sf::SoundBuffer* _sample_resource;
			std::string _stream_resource; //just file name :)
		};
	}
}

namespace ok
{
	namespace graphics
	{
		class SpriteAtlas;
	}

	class AssetsBasic
	{
	public:
		virtual ok::graphics::InternalFont* GetInternalFont(ok::String path) = 0;
		virtual ok::graphics::Font* GetFont(ok::String path) = 0;
		virtual ok::graphics::Texture* GetTexture(ok::String path) = 0;
		virtual ok::graphics::Shader* GetShader(ok::String path, bool reload = false) = 0;
		virtual ok::graphics::Mesh* GetMesh(ok::String path, bool prefer_hardware_skin = false) = 0;
		virtual ok::graphics::Material* GetMaterial(ok::String path) = 0;
		virtual ok::sound::SoundAsset* GetSoundSample(ok::String path) = 0;
		virtual ok::sound::SoundAsset* GetSoundStream(ok::String path) = 0;
		virtual ok::graphics::SpriteAtlas* GetSpriteAtlas(ok::String path) = 0;
		virtual GLenum GetAliasGL(ok::String alias) = 0;

		static ok::AssetsBasic& instance();
		static ok::AssetsBasic* basic_instance;
	protected:
	private:
	};
}