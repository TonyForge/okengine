#pragma once

#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Font.h"

namespace ok
{
	class AssetsBasic
	{
	public:
		virtual ok::graphics::InternalFont* GetInternalFont(ok::String path) = 0;
		virtual ok::graphics::Font* GetFont(ok::String path) = 0;
		virtual ok::graphics::Texture* GetTexture(ok::String path) = 0;
		virtual ok::graphics::Shader* GetShader(ok::String path, bool reload = false) = 0;
		virtual ok::graphics::Mesh* GetMesh(ok::String path, bool prefer_hardware_skin = false) = 0;
		virtual ok::graphics::Material* GetMaterial(ok::String path) = 0;
		virtual GLenum GetAliasGL(ok::String alias) = 0;

		static ok::AssetsBasic& instance();
		static ok::AssetsBasic* basic_instance;
	protected:
	private:
	};
}