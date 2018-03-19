#include "Texture.h"

std::vector<unsigned int> ok::graphics::Texture::binded_textures;

ok::graphics::Texture::Texture(sf::Texture * object_to_own)
{
	_owned_sf_texture = object_to_own;
	_owned_rt = nullptr;
	SetSmooth(true);
	SetWrapping(GL_REPEAT);
	SetBackgroundColor(ok::Color(0.f, 0.f, 0.f, 1.f));

	GuaranteeBindedTextureStorage();
}

ok::graphics::Texture::Texture(ok::graphics::RenderTarget * object_to_own)
{
	_owned_sf_texture = nullptr;
	_owned_rt = object_to_own;
	SetSmooth(true);
	SetWrapping(GL_REPEAT);
	SetBackgroundColor(ok::Color(0.f, 0.f, 0.f, 1.f));

	GuaranteeBindedTextureStorage();
}

glm::ivec2 ok::graphics::Texture::GetSize()
{
	if (_owned_sf_texture != nullptr)
	{
		sf::Vector2u _size = _owned_sf_texture->getSize();
		return glm::ivec2(static_cast<int>(_size.x), static_cast<int>(_size.y));
	}
	if (_owned_rt != nullptr)
	{
		return _owned_rt->GetSize();
	}

	return glm::ivec2(0, 0);
}

unsigned int ok::graphics::Texture::getNativeHandle()
{
	if (_owned_sf_texture != nullptr) return _owned_sf_texture->getNativeHandle();
	if (_owned_rt != nullptr) return _owned_rt->GetTexture_Color();
	return 0;
}

void ok::graphics::Texture::SetSmooth(bool smooth)
{
	if (_smooth == smooth) return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getNativeHandle());

	if (smooth)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ok::graphics::Texture::SetWrapping(GLenum wrapping)
{
	if (_wrapping == wrapping) return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getNativeHandle());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ok::graphics::Texture::SetBackgroundColor(ok::Color background_color)
{
	if (_background_color == background_color) return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getNativeHandle());

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(static_cast<glm::vec4>(background_color)));

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ok::graphics::Texture::SetProperties(bool smooth, GLenum wrapping, ok::Color background_color)
{
	if (_smooth == smooth &&
		_wrapping == wrapping &&
		_background_color == background_color) 
	return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getNativeHandle());

	if (smooth)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(static_cast<glm::vec4>(background_color)));

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ok::graphics::Texture::BindTexture(ok::graphics::Texture * texture, int texture_channel_index)
{
	while (ok::graphics::Texture::binded_textures.size() < (size_t)(texture_channel_index + 1))
	{
		ok::graphics::Texture::binded_textures.push_back(0);
	}

	if (texture == nullptr)
	{
		glActiveTexture(GL_TEXTURE1 + texture_channel_index); //GL_TEXTURE0 is reserved by the engine, so use GL_TEXTURE1 instead
		glBindTexture(GL_TEXTURE_2D, 0);

		ok::graphics::Texture::binded_textures[texture_channel_index] = 0;
	}
	else
	{
		unsigned int texNativeHandle = texture->getNativeHandle();

		if (ok::graphics::Texture::binded_textures[texture_channel_index] != texNativeHandle)
		{
			glActiveTexture(GL_TEXTURE1 + texture_channel_index); //GL_TEXTURE0 is reserved by the engine, so use GL_TEXTURE1 instead
			glBindTexture(GL_TEXTURE_2D, texNativeHandle);

			ok::graphics::Texture::binded_textures[texture_channel_index] = texNativeHandle;
		}
	}
}

void ok::graphics::Texture::UnbindTexture(ok::graphics::Texture * texture)
{
	if (texture == nullptr) return;

	int texture_channel_index = 0;

	unsigned int texNativeHandle = texture->getNativeHandle();

	for (auto& tex : ok::graphics::Texture::binded_textures)
	{
		if (tex == texNativeHandle)
		{
			glActiveTexture(GL_TEXTURE1 + texture_channel_index); //GL_TEXTURE0 is reserved by the engine, so use GL_TEXTURE1 instead
			glBindTexture(GL_TEXTURE_2D, 0);

			tex = 0;

			break;
		}

		texture_channel_index++;
	}
}

void ok::graphics::Texture::GuaranteeBindedTextureStorage()
{
	if (ok::graphics::Texture::binded_textures.size() == 0)
	{
		ok::graphics::Texture::binded_textures.resize(32);
		for (auto& tex : ok::graphics::Texture::binded_textures)
		{
			tex = 0;
		}
	}
}
