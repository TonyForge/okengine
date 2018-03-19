#pragma once

#include "RenderTarget.h"
#include "Color.h"

namespace ok
{
	namespace graphics
	{
		class Texture
		{
		public:
			//Texture();
			Texture(sf::Texture* object_to_own);
			Texture(ok::graphics::RenderTarget* object_to_own);
			glm::ivec2 GetSize();
			unsigned int getNativeHandle();

			void SetSmooth(bool smooth);
			void SetWrapping(GLenum wrapping);
			void SetBackgroundColor(ok::Color background_color);
			void SetProperties(bool smooth, GLenum wrapping, ok::Color background_color);

			static void BindTexture(ok::graphics::Texture* texture, int texture_channel_index = 0);
			static void UnbindTexture(ok::graphics::Texture* texture);

		private:
			sf::Texture* _owned_sf_texture;
			ok::graphics::RenderTarget* _owned_rt;

			bool _smooth;
			GLenum _wrapping;
			ok::Color _background_color;

			static std::vector<unsigned int> binded_textures;
			static void GuaranteeBindedTextureStorage();
		protected:
		};
	}
}