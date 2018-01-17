#include "RenderTarget.h"

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
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
}

void ok::graphics::RenderTarget::UnbindTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ok::graphics::RenderTarget::BindTexture_Color()
{
	glBindTexture(GL_TEXTURE_2D, framebuffer_color_channel);
}

void ok::graphics::RenderTarget::UnbindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int ok::graphics::RenderTarget::GetTexture_Color()
{
	return framebuffer_color_channel;
}

glm::ivec2 ok::graphics::RenderTarget::GetSize()
{
	return size;
}
