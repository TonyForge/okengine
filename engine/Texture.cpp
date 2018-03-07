#include "Texture.h"

ok::graphics::Texture::Texture(sf::Texture * object_to_own)
{
	_owned_sf_texture = object_to_own;
	_owned_rt = nullptr;
	SetSmooth(true);
	SetWrapping(GL_REPEAT);
}

ok::graphics::Texture::Texture(ok::graphics::RenderTarget * object_to_own)
{
	_owned_sf_texture = nullptr;
	_owned_rt = object_to_own;
	SetSmooth(true);
	SetWrapping(GL_REPEAT);
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
	glBindTexture(GL_TEXTURE_2D, getNativeHandle());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ok::graphics::Texture::SetBackgroundColor(ok::Color color)
{
	glBindTexture(GL_TEXTURE_2D, getNativeHandle());

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(static_cast<glm::vec4>(color)));

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ok::graphics::Texture::SetProperties(bool smooth, GLenum wrapping, ok::Color background_color, bool already_binded)
{
	if (false == already_binded)
	{
		glBindTexture(GL_TEXTURE_2D, getNativeHandle());
	}

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

	if (false == already_binded)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
