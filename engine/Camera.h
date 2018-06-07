#pragma once

#include "Transform.h"
#include "GameObject.h"

namespace ok
{
	namespace graphics
	{
		enum class CameraCoordinateSystem
		{
			ScreenCenter,
			Screen,
			CartesianCenter,
			Cartesian
		};

		class Camera : public ok::GameObject
		{
		public:
			Camera(ok::graphics::CameraCoordinateSystem coordinate_system = ok::graphics::CameraCoordinateSystem::Screen);
			void SetProjectionOrtho(float width, float height, float clip_plane_near = -100.0f, float clip_plane_far = 100.0f);
			void SetProjectionPersp(float width, float height, float fov_degrees = 45.0f, float clip_plane_near = 0.1f, float clip_plane_far = 100.0f);
			void BeginScissorTest(int px_x, int px_y, int px_w, int px_h);
			void EndScissorTest();
			void Update(float dt);
			static ok::graphics::Camera* GetCurrent();
			static void PushCamera(ok::graphics::Camera* camera);
			static void PopCamera();
			glm::mat4 GetProjectionMatrix();
			glm::mat4 GetViewMatrix();
			glm::mat4 GetVPMatrix();
			void OnChange();
			virtual ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);

			glm::vec3 ScreenToWorldPosition(glm::vec3 normalized_screen_position);
			glm::vec3 WorldToScreenPosition(glm::vec3 world_position);
			bool IsInViewport(glm::vec3 world_position, float world_radius);

			float GetViewportWidth();
			float GetViewportHeight();
		protected:
		private:
			glm::mat4 mProj;
			glm::mat4 mView;
			glm::mat4 mVP;
			bool mV_dirty;
			bool mP_dirty;
			bool perspective_enabled;
			bool _triangle_order_ccw = false;
			bool _triangle_order_cw = false;
			static bool _current_triangle_order_ccw;
			static bool _current_triangle_order_cw;

			ok::graphics::CameraCoordinateSystem _coordinate_system;

			void _SetCurrent();
			static void _ResetCurrent();
			static ok::graphics::Camera* _currentCamera;
			static std::stack<ok::graphics::Camera*> _currentCamera_stack;
			static std::stack<glm::ivec4> _scissorTest_stack;
			
			float projection_width, projection_height;
		public:
			static float _fixed_resolution_resample_scale;
			static bool _fixed_resolution_enabled;
			static bool _keep_aspect_ratio_enabled;
			static int _viewport_x, _viewport_y, _viewport_w, _viewport_h;

			static void SetGLViewport(int x, int y, int w, int h);
			static void SetGLViewport(float x, float y, float w, float h);
			static void SetGLViewport(glm::ivec4& xywh);
			static void GetGLViewport(glm::ivec4& out);

			//this is to keep glViewport values (it is the same as _viewport above, but i dont want to mix gl states with states of okengine camera)
			static int _gl_viewport_x, _gl_viewport_y, _gl_viewport_w, _gl_viewport_h;
			
		};
	}
}