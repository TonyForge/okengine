#include "Camera.h"

ok::graphics::Camera* ok::graphics::Camera::_currentCamera = nullptr;
std::stack<ok::graphics::Camera*>  ok::graphics::Camera::_currentCamera_stack;
bool ok::graphics::Camera::_fixed_resolution_enabled;
float ok::graphics::Camera::_fixed_resolution_resample_scale = 1.f;
bool ok::graphics::Camera::_keep_aspect_ratio_enabled;
int ok::graphics::Camera::_viewport_x;
int ok::graphics::Camera::_viewport_y;
int ok::graphics::Camera::_viewport_w;
int ok::graphics::Camera::_viewport_h;
std::stack<glm::ivec4> ok::graphics::Camera::_scissorTest_stack;

int ok::graphics::Camera::_gl_viewport_x;
int ok::graphics::Camera::_gl_viewport_y;
int ok::graphics::Camera::_gl_viewport_w;
int ok::graphics::Camera::_gl_viewport_h;

bool ok::graphics::Camera::_current_triangle_order_ccw = false;
bool ok::graphics::Camera::_current_triangle_order_cw = false;

ok::graphics::Camera::Camera(ok::graphics::CameraCoordinateSystem coordinate_system) : mView(1.0f), mProj(1.0f), mVP(1.0f), _coordinate_system(coordinate_system)
{
	mP_dirty = false;
	mV_dirty = false;
	perspective_enabled = false;
}

void ok::graphics::Camera::SetProjectionOrtho(float width, float height, float clip_plane_near, float clip_plane_far)
{
	std::swap(clip_plane_near, clip_plane_far);

	float left = 0.f;
	float right = width;
	float bottom = 0.f;
	float top = height;

	if (_coordinate_system == ok::graphics::CameraCoordinateSystem::Cartesian)
	{
		mProj = glm::orthoRH<float>(left, right, bottom, top, clip_plane_near, clip_plane_far);
		mProj = glm::scale(mProj, glm::vec3(1.0f, -1.0f, 1.0f));
		_triangle_order_cw = true;
		_triangle_order_ccw = false;
		//glFrontFace(GL_CW);
	}
	else if (_coordinate_system == ok::graphics::CameraCoordinateSystem::CartesianCenter)
	{
		mProj = glm::orthoRH<float>(left - (right - left) * .5f, left + (right - left) * .5f, bottom - (top - bottom) * .5f, bottom + (top - bottom) * .5f, clip_plane_near, clip_plane_far);
		mProj = glm::scale(mProj, glm::vec3(1.0f, -1.0f, 1.0f));
		_triangle_order_cw = true;
		_triangle_order_ccw = false;
		//glFrontFace(GL_CW);
	}
	else if (_coordinate_system == ok::graphics::CameraCoordinateSystem::Screen)
	{
		mProj = glm::orthoRH<float>(left, right, bottom - (top - bottom), top - (top - bottom), clip_plane_near, clip_plane_far);
		_triangle_order_cw = false;
		_triangle_order_ccw = true;
		//glFrontFace(GL_CCW);
	}
	else if (_coordinate_system == ok::graphics::CameraCoordinateSystem::ScreenCenter)
	{
		mProj = glm::orthoRH<float>(left - (right - left) * .5f, left + (right - left) * .5f, bottom - (top - bottom) * .5f, bottom + (top - bottom) * .5f, clip_plane_near, clip_plane_far);
		_triangle_order_cw = false;
		_triangle_order_ccw = true;
		//glFrontFace(GL_CCW);
	}
	
	projection_width = width;
	projection_height = height;

	mP_dirty = true;
	perspective_enabled = false;
}

void ok::graphics::Camera::SetProjectionPersp(float width, float height, float fov_degrees, float clip_plane_near, float clip_plane_far)
{
	std::swap(clip_plane_near, clip_plane_far);

	mProj = glm::mat4(0.f);
	float const tanHalfFovy = glm::tan((glm::radians(fov_degrees)) / 2.0f);

	mProj[0][0] = 1.f / ((width / height) * tanHalfFovy);
	mProj[1][1] = 1.f / (tanHalfFovy);
	mProj[2][3] = -1.f;

	mProj[2][2] = -(clip_plane_far + clip_plane_near) / (clip_plane_far - clip_plane_near);
	mProj[3][2] = -(2.f * clip_plane_far * clip_plane_near) / (clip_plane_far - clip_plane_near);

	if (
		_coordinate_system == ok::graphics::CameraCoordinateSystem::Cartesian ||
		_coordinate_system == ok::graphics::CameraCoordinateSystem::CartesianCenter
		)
	{
		mProj = glm::scale(mProj, glm::vec3(1.0f, -1.0f, 1.0f));
		_triangle_order_cw = true;
		_triangle_order_ccw = false;
		//glFrontFace(GL_CW);
	}
	else
	{
		_triangle_order_cw = false;
		_triangle_order_ccw = true;
		//glFrontFace(GL_CCW);
	}

	projection_width = width;
	projection_height = height;

	mP_dirty = true;
	perspective_enabled = true;
}

void ok::graphics::Camera::BeginScissorTest(int px_x, int px_y, int px_w, int px_h)
{
	if (_scissorTest_stack.size() == 0)
	{
		glEnable(GL_SCISSOR_TEST);
	}

	float projection_to_viewport_scale_x = static_cast<float>(_viewport_w) / projection_width;
	float projection_to_viewport_scale_y = static_cast<float>(_viewport_h) / projection_height;

	if (_coordinate_system == ok::graphics::CameraCoordinateSystem::Cartesian)
	{
		px_x = _viewport_x + static_cast<int>(static_cast<float>(px_x)*projection_to_viewport_scale_x);
		px_y = _viewport_y + static_cast<int>(static_cast<float>(px_y)*projection_to_viewport_scale_y);
		px_w = static_cast<int>(static_cast<float>(px_w) * projection_to_viewport_scale_x);
		px_h = static_cast<int>(static_cast<float>(px_h) * projection_to_viewport_scale_y);
	}
	else if (_coordinate_system == ok::graphics::CameraCoordinateSystem::CartesianCenter)
	{
		px_x = _viewport_x + static_cast<int>(glm::floor(static_cast<float>(_viewport_w) * 0.5f)) + static_cast<int>(static_cast<float>(px_x)*projection_to_viewport_scale_x);
		px_y = _viewport_y + static_cast<int>(glm::floor(static_cast<float>(_viewport_h) * 0.5f)) + static_cast<int>(static_cast<float>(px_y)*projection_to_viewport_scale_y);
		px_w = static_cast<int>(static_cast<float>(px_w) * projection_to_viewport_scale_x);
		px_h = static_cast<int>(static_cast<float>(px_h) * projection_to_viewport_scale_y);
	}
	else if (_coordinate_system == ok::graphics::CameraCoordinateSystem::Screen)
	{
		px_x = _viewport_x + static_cast<int>(static_cast<float>(px_x)*projection_to_viewport_scale_x);
		px_y = _viewport_y + (_viewport_h - static_cast<int>(static_cast<float>(px_y + px_h) * projection_to_viewport_scale_y));
		px_w = static_cast<int>(static_cast<float>(px_w) * projection_to_viewport_scale_x);
		px_h = static_cast<int>(static_cast<float>(px_h) * projection_to_viewport_scale_y);
	}
	else if (_coordinate_system == ok::graphics::CameraCoordinateSystem::ScreenCenter)
	{
		px_x = _viewport_x + static_cast<int>(glm::floor(static_cast<float>(_viewport_w) * 0.5f)) + static_cast<int>(static_cast<float>(px_x)*projection_to_viewport_scale_x);
		px_y = _viewport_y + static_cast<int>(glm::floor(static_cast<float>(_viewport_h) * 0.5f)) - static_cast<int>(static_cast<float>(px_y + px_h) * projection_to_viewport_scale_y);
		px_w = static_cast<int>(static_cast<float>(px_w) * projection_to_viewport_scale_x);
		px_h = static_cast<int>(static_cast<float>(px_h) * projection_to_viewport_scale_y);
	}

	_scissorTest_stack.push(glm::ivec4(px_x, px_y, px_w, px_h));
	
	glScissor(px_x, px_y, px_w, px_h);
}

void ok::graphics::Camera::EndScissorTest()
{
	_scissorTest_stack.pop();

	if (_scissorTest_stack.size() == 0)
	{
		glDisable(GL_SCISSOR_TEST);
	}
	else
	{
		glm::ivec4& rect = _scissorTest_stack.top();
		glScissor(rect.x, rect.y, rect.z, rect.w);
	}
}

void ok::graphics::Camera::Update(float dt)
{
	GameObject::Update(dt);
}

void ok::graphics::Camera::_SetCurrent()
{
	_currentCamera = this;

	if (_currentCamera->_triangle_order_ccw && (_current_triangle_order_cw || !_current_triangle_order_ccw))
	{
		_current_triangle_order_cw = false;
		_current_triangle_order_ccw = true;
		glFrontFace(GL_CCW);
	}

	if (_currentCamera->_triangle_order_cw && (_current_triangle_order_ccw || !_current_triangle_order_cw))
	{
		_current_triangle_order_cw = true;
		_current_triangle_order_ccw = false;
		glFrontFace(GL_CW);
	}
}

ok::graphics::Camera * ok::graphics::Camera::GetCurrent()
{
	return _currentCamera;
}

void ok::graphics::Camera::_ResetCurrent()
{
	_currentCamera = nullptr;

	_current_triangle_order_cw = false;
	_current_triangle_order_ccw = true;
	glFrontFace(GL_CCW);
}

void ok::graphics::Camera::SetGLViewport(int x, int y, int w, int h)
{
	_gl_viewport_x = x;
	_gl_viewport_y = y;
	_gl_viewport_w = w;
	_gl_viewport_h = h;

	glViewport(
		static_cast<GLint>(x),
		static_cast<GLint>(y),
		static_cast<GLint>(w),
		static_cast<GLint>(h)
	);
}

void ok::graphics::Camera::SetGLViewport(float x, float y, float w, float h)
{
	SetGLViewport(
		static_cast<int>(x),
		static_cast<int>(y),
		static_cast<int>(w),
		static_cast<int>(h)
		);
}

void ok::graphics::Camera::SetGLViewport(glm::ivec4 & xywh)
{
	SetGLViewport(xywh.x, xywh.y, xywh.z, xywh.w);
}

void ok::graphics::Camera::GetGLViewport(glm::ivec4 & out)
{
	out.x = _gl_viewport_x;
	out.y = _gl_viewport_y;
	out.z = _gl_viewport_w;
	out.w = _gl_viewport_h;
}

void ok::graphics::Camera::PushCamera(ok::graphics::Camera * camera)
{
	_currentCamera_stack.push(camera);
	_currentCamera_stack.top()->_SetCurrent();
}

void ok::graphics::Camera::PopCamera()
{
	_currentCamera_stack.pop();
	if (_currentCamera_stack.size() > 0)
		_currentCamera_stack.top()->_SetCurrent();
	else
		ok::graphics::Camera::_ResetCurrent();
}

glm::mat4 ok::graphics::Camera::GetProjectionMatrix()
{
	return mProj;
}

glm::mat4 ok::graphics::Camera::GetViewMatrix()
{
	if (mV_dirty)
	{
		BeginTransform(ok::TransformSpace::WorldSpace);

		mView = glm::lookAtRH(GetPosition(), GetPosition() + GetForward(), -GetUp());

		EndTransform(false);

		mV_dirty = false;
	}

	return mView;
}

glm::mat4 ok::graphics::Camera::GetVPMatrix()
{
	if (mP_dirty || mV_dirty)
	{
		mVP = GetProjectionMatrix() * GetViewMatrix();
		
		mP_dirty = false;
		mV_dirty = false;
	}
	return mVP;
}

void ok::graphics::Camera::OnChange()
{
	mP_dirty = true;
	mV_dirty = true;
}

ok::GameObject * ok::graphics::Camera::Duplicate(ok::GameObject * _clone)
{
	_clone = new ok::graphics::Camera(_coordinate_system);
	ok::GameObject::Duplicate(_clone);

	return _clone;
}

glm::vec3 ok::graphics::Camera::ScreenToWorldPosition(glm::vec3 normalized_screen_position)
{
	normalized_screen_position.y = 1.0f - normalized_screen_position.y;

	normalized_screen_position = (normalized_screen_position - glm::vec3(0.5f, 0.5f, 0.5f))*2.f;

	glm::vec4 result = glm::inverse(GetVPMatrix()) * glm::vec4(normalized_screen_position, 1.0f);

	if (perspective_enabled)
	{
		result.w = 1.0f / result.w;
		result.x *= result.w;
		result.y *= result.w;
		result.z *= result.w;
	}

	return glm::vec3(result);
}

glm::vec3 ok::graphics::Camera::WorldToScreenPosition(glm::vec3 world_position)
{
	glm::vec4 result = GetVPMatrix() * glm::vec4(world_position, 1.0f);

	result.x = (result.x + 1.0f) * 0.5f;
	result.y = (result.y + 1.0f) * 0.5f;
	result.z = 0;

	return glm::vec3(result);
}

bool ok::graphics::Camera::IsInViewport(glm::vec3 world_position, float world_radius)
{
	glm::vec3 screen_position = WorldToScreenPosition(world_position);
	float screen_radius = (WorldToScreenPosition(world_position + GetRight()*world_radius) - screen_position).x;

	float delta_x = screen_position.x - glm::max(0.f, glm::min(screen_position.x, 1.f));
	float delta_y = screen_position.y - glm::max(0.f, glm::min(screen_position.y, 1.f));
	return (delta_x * delta_x + delta_y * delta_y) < (screen_radius * screen_radius);
}

float ok::graphics::Camera::GetViewportWidth()
{
	return projection_width;
}

float ok::graphics::Camera::GetViewportHeight()
{
	return projection_height;
}
