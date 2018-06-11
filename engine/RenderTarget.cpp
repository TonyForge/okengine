#include "RenderTarget.h"

std::vector<ok::graphics::RenderTarget*> ok::graphics::RenderTarget::_bind_stack;
unsigned int ok::graphics::RenderTarget::exchange_framebuffer_id = 0;

ok::graphics::RenderTarget::RenderTarget(int width, int height, bool smooth, bool depth_enabled, bool stencil_enabled, bool rbo_enabled)
{
	size.x = width;
	size.y = height;

	rt_depth_enabled = depth_enabled;
	rt_stencil_enabled = stencil_enabled;
	rt_rbo_enabled = rbo_enabled;

	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

	glGenTextures(1, &framebuffer_color_channel);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer_color_channel);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

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
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_color_channel, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (rbo_enabled)
	{
		glGenRenderbuffers(1, &render_buffer_object_id);
		glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object_id);
		if (depth_enabled && stencil_enabled)
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		else
		{
			if (depth_enabled)
			{
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			}

			if (stencil_enabled)
			{
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			}
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if (depth_enabled && stencil_enabled)
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object_id);
		else
		{
			if (depth_enabled)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object_id);
			}

			if (stencil_enabled)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object_id);
			}
		}

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	else
	{
		if (depth_enabled && stencil_enabled)
		{
			glGenTextures(1, &framebuffer_depth_stencil_channel);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, framebuffer_depth_stencil_channel);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffer_depth_stencil_channel, 0);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			if (depth_enabled)
			{
				glGenTextures(1, &framebuffer_depth_channel);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, framebuffer_depth_channel);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer_depth_channel, 0);

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			if (stencil_enabled)
			{
				glGenTextures(1, &framebuffer_depth_stencil_channel);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, framebuffer_depth_stencil_channel);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffer_depth_stencil_channel, 0);

				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG(ERROR) << "Framebuffer is not complete!";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ok::graphics::RenderTarget::~RenderTarget()
{
	glDeleteTextures(1, &framebuffer_color_channel);

	if (rt_rbo_enabled)
	{
		glDeleteRenderbuffers(1, &render_buffer_object_id);
	}

	if (rt_depth_enabled && !rt_stencil_enabled)
	{
		glDeleteTextures(1, &framebuffer_depth_channel);
	}
	else if (rt_stencil_enabled)
	{
		glDeleteTextures(1, &framebuffer_depth_stencil_channel);
	}

	glDeleteFramebuffers(1, &framebuffer_id);
}

void ok::graphics::RenderTarget::BindTarget()
{
	_previous_camera_viewport_settings.x = ok::graphics::Camera::_viewport_x;
	_previous_camera_viewport_settings.y = ok::graphics::Camera::_viewport_y;
	_previous_camera_viewport_settings.z = ok::graphics::Camera::_viewport_w;
	_previous_camera_viewport_settings.w = ok::graphics::Camera::_viewport_h;

	_bind_stack.push_back(this);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

	ok::graphics::Camera::_viewport_x = 0;
	ok::graphics::Camera::_viewport_y = 0;
	ok::graphics::Camera::_viewport_w = size.x;
	ok::graphics::Camera::_viewport_h = size.y;

	ok::graphics::Camera::GetGLViewport(_previous_gl_viewport_settings);
	ok::graphics::Camera::SetGLViewport(0, 0, size.x, size.y);
}

void ok::graphics::RenderTarget::UnbindTarget()
{
	_bind_stack.pop_back();

	if (_bind_stack.size() > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _bind_stack.back()->framebuffer_id);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	ok::graphics::Camera::_viewport_x = _previous_camera_viewport_settings.x;
	ok::graphics::Camera::_viewport_y = _previous_camera_viewport_settings.y;
	ok::graphics::Camera::_viewport_w = _previous_camera_viewport_settings.z;
	ok::graphics::Camera::_viewport_h = _previous_camera_viewport_settings.w;

	ok::graphics::Camera::SetGLViewport(_previous_gl_viewport_settings);
}

unsigned int ok::graphics::RenderTarget::GetTexture_Color()
{
	return framebuffer_color_channel;
}

glm::ivec2 ok::graphics::RenderTarget::GetSize()
{
	return size;
}

void ok::graphics::RenderTarget::CopyColorBetween(ok::graphics::RenderTarget & from, ok::graphics::RenderTarget & to, int from_x, int from_y, int to_x, int to_y, int width, int height)
{
	if (exchange_framebuffer_id == 0)
	{
		glGenFramebuffers(1, &exchange_framebuffer_id);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, exchange_framebuffer_id);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, from.framebuffer_color_channel, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, to.framebuffer_color_channel, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);

	glBlitFramebuffer(from_x, from_y, from_x + width, from_y + height, to_x, to_y, to_x + width, to_y + height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	if (_bind_stack.size() > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _bind_stack.back()->framebuffer_id);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void ok::graphics::RenderTarget::CopyColorBetweenSequenceBegin(ok::graphics::RenderTarget & from, ok::graphics::RenderTarget & to)
{
	if (exchange_framebuffer_id == 0)
	{
		glGenFramebuffers(1, &exchange_framebuffer_id);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, exchange_framebuffer_id);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, from.framebuffer_color_channel, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, to.framebuffer_color_channel, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
}

void ok::graphics::RenderTarget::CopyColorBetweenSequenceStep(int from_x, int from_y, int to_x, int to_y, int width, int height)
{
	glBlitFramebuffer(from_x, from_y, from_x+width, from_y+height, to_x, to_y, to_x+width, to_y+height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void ok::graphics::RenderTarget::CopyColorBetweenSequenceEnd()
{
	if (_bind_stack.size() > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _bind_stack.back()->framebuffer_id);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
