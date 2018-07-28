#include "Transform.h"

ok::Transform::Transform() :
	keepWorldTransform(true),
	_orientation_dirty(false),
	_transformSequenceNoChanges(true),
	_transformSequenceEnabled(false),
	_forward(0.f, 0.f, 1.f),
	_up(0.f, 1.f, 0.f),
	_right(1.f, 0.f, 0.f),
	_relativePosition(0.f, 0.f, 0.f),
	_relativeRotationEuler(0.f, 0.f, 0.f),
	_relativeScale(1.f, 1.f, 1.f),
	_relativeTransformMatrix(1.f),
	_absoluteTransformMatrix(1.f),
	_absolutePosition(0.f, 0.f, 0.f),
	_absoluteRotationEuler(0.f, 0.f, 0.f),
	_absoluteScale(1.f, 1.f, 1.f),
	_relativeRotationMatrix(1.0f),
	_absoluteRotationMatrix(1.0f),
	_parent(nullptr),
	_rotation_direction(ok::RotationDirection::CCW),
	_transform_combine_order(ok::TransformCombineOrder::SRT),
	_gameObject(nullptr),
	_childrens_absolute_transform_dirty(false)
{

}

ok::GameObject & ok::Transform::gameObject()
{
	return *_gameObject;
}

void ok::Transform::Rename(ok::String name)
{
	this->name = name;
	this->name_hash = std::hash<std::string>()(name);
}

ok::Transform * ok::Transform::FindChild(ok::String& name, bool nameIsPath)
{
	if (nameIsPath)
	{
		std::vector <ok::String> path_parts = name.split("\\/");
		ok::Transform* _current_child = this;

		for (auto&& name : path_parts)
		{
			_current_child = _current_child->FindChild(name, false);
			if (_current_child == nullptr)
				break;
		}

		return _current_child;
	}
	else
	{
		std::list<ok::Transform*>::iterator it = _childrens.begin();
		std::list<ok::Transform*>::iterator it_end = _childrens.end();
		std::hash<std::string> hasher;
		size_t name_hash = hasher(name);

		while (it != it_end)
		{
			ok::Transform* child = (*it);
			if ((child->name_hash == name_hash) && (child->name == name))
			{
				return child;
			}
			it++;
		}
	}

	return nullptr;
}

std::list<ok::Transform*>& ok::Transform::GetChildrens()
{
	return _childrens;
}

void ok::Transform::AddChild(ok::Transform * child)
{
	_AddChild(child, child->keepWorldTransform);
}

void ok::Transform::SetParent(ok::Transform * parent)
{
	_SetParent(parent, keepWorldTransform);
}

void ok::Transform::RemoveChild(ok::Transform * child)
{
	_RemoveChild(child, child->keepWorldTransform);
}

void ok::Transform::AddChild(ok::Transform * child, bool forcedKeepWorldTransformValue)
{
	_AddChild(child, forcedKeepWorldTransformValue);
}

void ok::Transform::SetParent(ok::Transform * parent, bool forcedKeepWorldTransformValue)
{
	_SetParent(parent, forcedKeepWorldTransformValue);
}

void ok::Transform::RemoveChild(ok::Transform * child, bool forcedKeepWorldTransformValue)
{
	_RemoveChild(child, forcedKeepWorldTransformValue);
}

ok::Transform * ok::Transform::GetParent()
{
	return _parent;
}

void ok::Transform::_AddChild(ok::Transform * child, bool _keepWorldTransform)
{
	if (child == nullptr || child->_parent == this) return;

	if (child->_parent != nullptr)
	{
		child->_parent->_RemoveChild(child, _keepWorldTransform);
	}

	_childrens.push_back(child);
	child->_parent = this;

	if (_keepWorldTransform)
	{
		child->_relativePosition = glm::vec3(glm::transpose(_absoluteRotationMatrix) * glm::vec4(child->_absolutePosition - _absolutePosition, 1.f));
		child->_relativeRotationEuler = child->_absoluteRotationEuler - _absoluteRotationEuler;

		glm::vec3 parentScale = _absoluteScale;
		child->_relativeScale = child->_absoluteScale;
		if (glm::abs(parentScale.x) == 0.f) child->_relativeScale.x = std::numeric_limits<float>().max(); else child->_relativeScale.x /= parentScale.x;
		if (glm::abs(parentScale.y) == 0.f) child->_relativeScale.y = std::numeric_limits<float>().max(); else child->_relativeScale.y /= parentScale.y;
		if (glm::abs(parentScale.z) == 0.f) child->_relativeScale.z = std::numeric_limits<float>().max(); else child->_relativeScale.z /= parentScale.z;

		child->_UpdateRelativeTransformMatrix();
	}

	if (false == _keepWorldTransform)
		child->UpdateAbsoluteTransform(true);
}

void ok::Transform::_SetParent(ok::Transform * parent, bool _keepWorldTransform)
{
	if (_parent != nullptr)
	{
		_parent->_RemoveChild(this, _keepWorldTransform);
		_parent = nullptr;
	}

	if (parent != nullptr)
	{
		parent->_childrens.push_back(this);

		if (_keepWorldTransform)
		{
			//_relativePosition = _absolutePosition-parent->_absolutePosition;
			_relativePosition = glm::vec3(glm::transpose(parent->_absoluteRotationMatrix) * glm::vec4(_absolutePosition - parent->_absolutePosition, 1.f));
			_relativeRotationEuler = _absoluteRotationEuler - parent->_absoluteRotationEuler;

			glm::vec3 parentScale = parent->_absoluteScale;
			_relativeScale = _absoluteScale;
			if (glm::abs(parentScale.x) == 0.f) _relativeScale.x = std::numeric_limits<float>().max(); else _relativeScale.x /= parentScale.x;
			if (glm::abs(parentScale.y) == 0.f) _relativeScale.y = std::numeric_limits<float>().max(); else _relativeScale.y /= parentScale.y;
			if (glm::abs(parentScale.z) == 0.f) _relativeScale.z = std::numeric_limits<float>().max(); else _relativeScale.z /= parentScale.z;

			_UpdateRelativeTransformMatrix();
		}
	}
	
	_parent = parent;

	if (false == _keepWorldTransform)
	UpdateAbsoluteTransform(true);
}

void ok::Transform::_RemoveChild(ok::Transform * child, bool _keepWorldTransform)
{
	if (child->_parent == this)
	{
		if (_keepWorldTransform)
		{
			child->_relativePosition = child->_absolutePosition;
			child->_relativeRotationEuler = child->_absoluteRotationEuler;
			child->_relativeScale = child->_absoluteScale;

			child->_UpdateRelativeTransformMatrix();
		}

		_childrens.remove(child);
		child->_parent = nullptr;

		if (false == _keepWorldTransform)
			child->UpdateAbsoluteTransform(true);
	}
}

void ok::Transform::UpdateAbsoluteTransform(bool updateChildrens)
{
	_UpdateAbsoluteTransformMatrix();

	if (updateChildrens)
	{
		UpdateChildrensTransform();
	}
}

glm::mat4 ok::Transform::GetAbsoluteTransformMatrix()
{
	return _absoluteTransformMatrix;
}

void ok::Transform::UpdateChildrensTransform(bool force)
{
	if (_parent != nullptr && _childrens_absolute_transform_dirty == false)
	{
		_childrens_absolute_transform_dirty = _parent->_childrens_absolute_transform_dirty;
	}

	if (_childrens_absolute_transform_dirty || force)
	{
		std::list<ok::Transform*>::iterator it = _childrens.begin();
		std::list<ok::Transform*>::iterator it_end = _childrens.end();

		while (it != it_end)
		{
			(*it)->UpdateAbsoluteTransform(true);
			it++;
		}

		_childrens_absolute_transform_dirty = false;
	}
}

bool ok::Transform::IsUpdateChildrensTransformNeeded()
{
	return _childrens_absolute_transform_dirty;
}

void ok::Transform::BeginTransform(ok::TransformSpace space)
{
	_transformSequenceSpace = space;
	_transformSequenceEnabled = true;
	_transformSequenceNoChanges = true;
}

void ok::Transform::EndTransform(bool updateChildrens)
{
	if (false == _transformSequenceNoChanges)
	{
		_UpdateRelativeTransformMatrix();
		_childrens_absolute_transform_dirty = true;

		if (updateChildrens)
		{
			UpdateAbsoluteTransform(true);
		}
		else
		{
			_UpdateAbsoluteTransformMatrix();
		}
	}


	_transformSequenceEnabled = false;
}

void ok::Transform::SetPosition(glm::vec3 position)
{
	if (_transformSequenceEnabled)
	{
		_transformSequenceNoChanges = false;

		if (_transformSequenceSpace == ok::TransformSpace::LocalSpace)
		{
			_relativePosition = position;
		}
		else
		{
			if (_parent)
			{
				_relativePosition = position - _parent->_absolutePosition;
			}
			else
			{
				_relativePosition = position;
			}
		}
	}
	else
	{
		_relativePosition = position;
		_UpdateRelativeTransformMatrix();
		_UpdateAbsoluteTransformMatrix();
	}
}

glm::vec3 ok::Transform::GetPosition()
{
	if (_transformSequenceEnabled)
	{
		if (_transformSequenceSpace == ok::TransformSpace::LocalSpace)
		{
			return _relativePosition;
		}
		else
		{
			if (_parent)
			{
				return _absolutePosition;
			}
			else
			{
				return _relativePosition;
			}
		}
	}
	else
	{
		return _relativePosition;
	}

	return glm::vec3();
}

void ok::Transform::SetScale(glm::vec3 scale)
{
	if (_transformSequenceEnabled)
	{
		_transformSequenceNoChanges = false;

		if (_transformSequenceSpace == ok::TransformSpace::LocalSpace)
		{
			_relativeScale = scale;
		}
		else
		{
			if (_parent)
			{
				glm::vec3 parentScale = _parent->_absoluteScale;

				_relativeScale = scale;

				if (glm::abs(parentScale.x) == 0.f) _relativeScale.x = std::numeric_limits<float>().max(); else _relativeScale.x /= parentScale.x;
				if (glm::abs(parentScale.y) == 0.f) _relativeScale.y = std::numeric_limits<float>().max(); else _relativeScale.y /= parentScale.y;
				if (glm::abs(parentScale.z) == 0.f) _relativeScale.z = std::numeric_limits<float>().max(); else _relativeScale.z /= parentScale.z;
			}
			else
			{
				_relativeScale = scale;
			}
		}
	}
	else
	{
		_relativeScale = scale;
		_UpdateRelativeTransformMatrix();
		_UpdateAbsoluteTransformMatrix();
	}
}

glm::vec3 ok::Transform::GetScale()
{
	if (_transformSequenceEnabled)
	{
		if (_transformSequenceSpace == ok::TransformSpace::LocalSpace)
		{
			return _relativeScale;
		}
		else
		{
			if (_parent)
			{
				return _absoluteScale;
			}
			else
			{
				return _relativeScale;
			}
		}
	}
	else
	{
		return _relativeScale;
	}

	return glm::vec3();
}

void ok::Transform::SetRotation(glm::vec3 eulerAnglesDegrees)
{
	_orientation_dirty = true;

	if (_transformSequenceEnabled)
	{
		_transformSequenceNoChanges = false;

		if (_transformSequenceSpace == ok::TransformSpace::LocalSpace)
		{
			_relativeRotationEuler = eulerAnglesDegrees;
		}
		else
		{
			if (_parent)
			{
				_relativeRotationEuler = eulerAnglesDegrees - _parent->_absoluteRotationEuler;
			}
			else
			{
				_relativeRotationEuler = eulerAnglesDegrees;
			}
		}
	}
	else
	{
		_relativeRotationEuler = eulerAnglesDegrees;
		_UpdateRelativeTransformMatrix();
		_UpdateAbsoluteTransformMatrix();
	}
}

glm::vec3 ok::Transform::GetRotation()
{
	if (_transformSequenceEnabled)
	{
		if (_transformSequenceSpace == ok::TransformSpace::LocalSpace)
		{
			return _relativeRotationEuler;
		}
		else
		{
			if (_parent)
			{
				return _absoluteRotationEuler;
			}
			else
			{
				return _relativeRotationEuler;
			}
		}
	}
	else
	{
		return _relativeRotationEuler;
	}

	return glm::vec3();
}

glm::vec3 ok::Transform::GetForward()
{
	if (_orientation_dirty)
	{
		_UpdateOrientationVectors();
	}
	return _forward;
}

void ok::Transform::SetForward(glm::vec3 forward)
{
	_forward = glm::normalize(forward);
	_up = glm::vec3(0.f, 1.f, 0.f);
	_right = glm::normalize(glm::cross(_up, _forward));
	_up = glm::normalize(glm::cross(_forward, _right));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);

	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

glm::vec3 ok::Transform::GetUp()
{
	if (_orientation_dirty)
	{
		_UpdateOrientationVectors();
	}
	return _up;
}

void ok::Transform::SetUp(glm::vec3 up)
{
	_up = glm::normalize(up);
	_forward = glm::vec3(0.f, 0.f, 1.f);
	_right = glm::normalize(glm::cross(_up, _forward));
	_forward = glm::normalize(glm::cross(_right, _up));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);

	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

glm::vec3 ok::Transform::GetRight()
{
	if (_orientation_dirty)
	{
		_UpdateOrientationVectors();
	}
	return _right;
}

void ok::Transform::SetRight(glm::vec3 right)
{
	_right = glm::normalize(right);
	_up = glm::vec3(0.f, 1.f, 0.f);
	_forward = glm::normalize(glm::cross(_right, _up));
	_up = glm::normalize(glm::cross(_forward, _right));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);

	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::SetOrientation(glm::vec3 forward, glm::vec3 up)
{
	_forward = forward;
	_up = up;
	_right = glm::normalize(glm::cross(_up, _forward));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);

	BeginTransform(ok::TransformSpace::WorldSpace);
		SetRotation(_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::LookAt(glm::vec3 target, glm::vec3 up)
{
	_forward = glm::normalize(target - _absolutePosition);
	_up = up;
	_right = glm::normalize(glm::cross(_up, _forward));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);

	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::LookAt(glm::vec3 target)
{
	_forward = glm::normalize(target - _absolutePosition);
	_right = glm::normalize(glm::cross(_up, _forward));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);

	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::LookAt(glm::vec3 v1, glm::vec3 v2, ok::LookAtAxis v1_axis, ok::LookAtAxis v2_axis)
{
	if (v1_axis == v2_axis) return;

	float axis_x_sign = 1.f;
	float axis_y_sign = 1.f;

	glm::vec3* axis_x;
	glm::vec3* axis_y;
	glm::vec3* axis_z;
		
	switch (v1_axis)
	{
		case ok::LookAtAxis::Forward : axis_x = &_forward; break;
		case ok::LookAtAxis::InvForward: axis_x = &_forward; axis_x_sign = -1.f; break;
		case ok::LookAtAxis::Right: axis_x = &_right; break;
		case ok::LookAtAxis::InvRight: axis_x = &_right; axis_x_sign = -1.f; break;
		case ok::LookAtAxis::Up: axis_x = &_up; break;
		case ok::LookAtAxis::InvUp: axis_x = &_up; axis_x_sign = -1.f; break;
	}

	switch (v2_axis)
	{
		case ok::LookAtAxis::Forward: axis_y = &_forward; break;
		case ok::LookAtAxis::InvForward: axis_y = &_forward; axis_y_sign = -1.f; break;
		case ok::LookAtAxis::Right: axis_y = &_right; break;
		case ok::LookAtAxis::InvRight: axis_y = &_right; axis_y_sign = -1.f; break;
		case ok::LookAtAxis::Up: axis_y = &_up; break;
		case ok::LookAtAxis::InvUp: axis_y = &_up; axis_y_sign = -1.f; break;
	}

	if (axis_x == &_forward && axis_y == &_right) axis_z = &_up; else
	if (axis_y == &_forward && axis_x == &_right) axis_z = &_up; else
	if (axis_x == &_forward && axis_y == &_up) axis_z = &_right; else
	if (axis_y == &_forward && axis_x == &_up) axis_z = &_right; else axis_z = &_forward;

	*axis_x = v1 * axis_x_sign;
	*axis_y = v2 * axis_y_sign;
	*axis_z = glm::normalize(glm::cross(*axis_y, *axis_x));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);

	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::SetRotationDirection(ok::RotationDirection new_direction)
{
	_rotation_direction = new_direction;

	BeginTransform(ok::TransformSpace::LocalSpace);
	SetRotation(_relativeRotationEuler);
	EndTransform(true);
}

void ok::Transform::SetTransformCombineOrder(ok::TransformCombineOrder new_order)
{
	_transform_combine_order = new_order;

	_UpdateRelativeTransformMatrix();
	UpdateAbsoluteTransform(true);
}

void ok::Transform::OnChange()
{
}

void ok::Transform::OnShow()
{
}

void ok::Transform::OnHide()
{
}

bool ok::Transform::IsVisible()
{
	return _visible;
}

void ok::Transform::Show()
{
	if (_visible == false) OnShow();
	_visible = true;
}

void ok::Transform::Hide()
{
	if (_visible == true) OnHide();
	_visible = false;
}

void ok::Transform::EnableOverrideChildrenVisibility()
{
	_override_children_visibility = true;
}

void ok::Transform::DisableOverrideChildrenVisibility()
{
	_override_children_visibility = false;
}

bool ok::Transform::IsChildrenVisibilityOverriden()
{
	return _override_children_visibility;
}

void ok::Transform::CopyPaste(ok::Transform & copyFrom, ok::Transform & pasteTo, bool updateChildrens, ok::TransformSpace space)
{
	copyFrom.BeginTransform(space);
	pasteTo.BeginTransform(space);

	pasteTo.SetPosition(copyFrom.GetPosition());
	pasteTo.SetRotation(copyFrom.GetRotation());
	pasteTo.SetScale(copyFrom.GetScale());

	copyFrom.EndTransform(false);
	pasteTo.EndTransform(updateChildrens);
}

void ok::Transform::_UpdateRelativeTransformMatrix()
{
	_relativeTransformMatrix = glm::mat4(1.0f);
	_relativeRotationMatrix = glm::mat4(1.0f);

	//order is critically important here!
	if (_rotation_direction == ok::RotationDirection::CCW)
	{
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(-_relativeRotationEuler.z), glm::vec3(0.f, 0.f, 1.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(-_relativeRotationEuler.y), glm::vec3(0.f, 1.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(-_relativeRotationEuler.x), glm::vec3(1.f, 0.f, 0.f));
	}
	else
	{
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.z), glm::vec3(0.f, 0.f, 1.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.y), glm::vec3(0.f, 1.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.x), glm::vec3(1.f, 0.f, 0.f));
	}

	switch (_transform_combine_order)
	{
		case ok::TransformCombineOrder::SRT :
		{
			_relativeTransformMatrix = glm::translate(_relativeTransformMatrix, _relativePosition) * (_relativeRotationMatrix * glm::scale(_relativeTransformMatrix, _relativeScale));
		}
		break;
		case ok::TransformCombineOrder::RST:
		{
			_relativeTransformMatrix = glm::translate(_relativeTransformMatrix, _relativePosition) * (glm::scale(_relativeTransformMatrix, _relativeScale) * _relativeRotationMatrix);
		}
		break;
		case ok::TransformCombineOrder::RTS:
		{
			_relativeTransformMatrix =  glm::scale(_relativeTransformMatrix, _relativeScale) * (glm::translate(_relativeTransformMatrix, _relativePosition) * _relativeRotationMatrix);
		}
		break;
	}
}

void ok::Transform::_UpdateAbsoluteTransformMatrix()
{
	if (_parent != nullptr)
	{
		_absoluteTransformMatrix = _parent->_absoluteTransformMatrix * _relativeTransformMatrix;
		_absoluteRotationMatrix = _parent->_absoluteRotationMatrix * _relativeRotationMatrix;
		_absolutePosition = glm::vec3(_parent->_absoluteTransformMatrix * glm::vec4(_relativePosition, 1.f));
		_absoluteRotationEuler = _parent->_absoluteRotationEuler + _relativeRotationEuler;
		_absoluteScale = _parent->_absoluteScale * _relativeScale;
	}
	else
	{
		_absoluteTransformMatrix = _relativeTransformMatrix;
		_absoluteRotationMatrix = _relativeRotationMatrix;
		_absolutePosition = _relativePosition;
		_absoluteRotationEuler = _relativeRotationEuler;
		_absoluteScale = _relativeScale;
	}
		
	_UpdateOrientationVectors();

	OnChange();
}

void ok::Transform::_UpdateOrientationVectors()
{
	_forward =	_absoluteRotationMatrix * glm::vec4(0.f, 0.f, 1.f, 0.f); //w = 0.f because there is no translation in orientation vectors
	_up		 = _absoluteRotationMatrix * glm::vec4(0.f, 1.f, 0.f, 0.f); //... same reason
	_right	 = _absoluteRotationMatrix * glm::vec4(1.f, 0.f, 0.f, 0.f); //... same reason

	_forward = glm::normalize(_forward);
	_up		 = glm::normalize(_up);
	_right	 = glm::normalize(_right);

	_orientation_dirty = false;
}

glm::vec3 ok::Transform::_ConvertMatToEulerAnglesXYZ(const glm::mat3 & mat)
{
	const float xEuler = std::atan2(-mat[2].y, mat[2].z);
	const float cos_angle = glm::sqrt(1.f - glm::pow(mat[2].x, 2));
	const float yEuler = std::atan2(mat[2].x, cos_angle);
	const float zEuler = std::atan2(-mat[1].x, mat[0].x);

	return glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}

ok::Transform_GameObject_Assignee::Transform_GameObject_Assignee(ok::GameObject * _gameObject, ok::Transform * _transform)
{
	_transform->_gameObject = _gameObject;
}
