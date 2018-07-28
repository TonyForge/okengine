#pragma once

#include "Camera.h"

namespace ok
{
	namespace graphics
	{
		class RenderTarget
		{
		public:
			RenderTarget(int width, int height, bool smooth, bool depth_enabled, bool stencil_enabled, bool rbo_enabled);
			~RenderTarget();
			void BindTarget();
			void UnbindTarget();
			unsigned int GetTexture_Color();
			glm::ivec2 GetSize();

			static void CopyColorBetween(
				ok::graphics::RenderTarget& from,
				ok::graphics::RenderTarget& to,
				int from_x, int from_y, int to_x, int to_y,
				int width, int height);

			static void CopyColorBetweenSequenceBegin(ok::graphics::RenderTarget& from, ok::graphics::RenderTarget& to);
			static void CopyColorBetweenSequenceStep(int from_x, int from_y, int to_x, int to_y, int width, int height);
			static void CopyColorBetweenSequenceEnd();
		protected:
		private:
			bool rt_depth_enabled, rt_stencil_enabled, rt_rbo_enabled;
			unsigned int framebuffer_color_channel;
			unsigned int framebuffer_depth_channel;
			unsigned int framebuffer_depth_stencil_channel;
			unsigned int framebuffer_id;
			unsigned int render_buffer_object_id;
			glm::ivec2 size;

			glm::ivec4 _previous_camera_viewport_settings;
			glm::ivec4 _previous_gl_viewport_settings;
			static std::vector<ok::graphics::RenderTarget*> _bind_stack;

			static unsigned int exchange_framebuffer_id;
		};
	}
}