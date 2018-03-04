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
			void SetBackgroundColor(ok::Color color);
		private:
			sf::Texture* _owned_sf_texture;
			ok::graphics::RenderTarget* _owned_rt;
		protected:
		};
	}
}