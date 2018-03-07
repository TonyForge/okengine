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
	_rotation_order(ok::RotationOrder::XYZ)
{

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
		child->_relativePosition = child->_absolutePosition - _absolutePosition;
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
			_relativePosition = _absolutePosition-parent->_absolutePosition;
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
		std::list<ok::Transform*>::iterator it = _childrens.begin();
		std::list<ok::Transform*>::iterator it_end = _childrens.end();

		while (it != it_end)
		{
			(*it)->UpdateAbsoluteTransform(updateChildrens);
			it++;
		}
	}
}

glm::mat4 ok::Transform::GetAbsoluteTransformMatrix()
{
	return _absoluteTransformMatrix;
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

		if (updateChildrens)
		{
			UpdateAbsoluteTransform(true);
		}
		else
		{
			_UpdateAbsoluteTransformMatrix();
		}

		OnChange();
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
	SetRotation(-_ConvertMatToEulerAnglesXYZ(orientationMatrix));
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
	SetRotation(-_ConvertMatToEulerAnglesXYZ(orientationMatrix));
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
	SetRotation(-_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::SetOrientation(glm::vec3 forward, glm::vec3 up)
{
	_forward = forward;
	_up = up;
	_right = glm::normalize(glm::cross(_up, _forward));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);
	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(-_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::LookAt(glm::vec3 target, glm::vec3 up)
{
	_forward = glm::normalize(target - _absolutePosition);
	_up = up;
	_right = glm::normalize(glm::cross(_up, _forward));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);
	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(-_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::LookAt(glm::vec3 target)
{
	_forward = glm::normalize(target - _absolutePosition);
	_right = glm::normalize(glm::cross(_up, _forward));

	glm::mat4 orientationMatrix = glm::lookAtLH(glm::vec3(0.f, 0.f, 0.f), _forward, _up);
	BeginTransform(ok::TransformSpace::WorldSpace);
	SetRotation(-_ConvertMatToEulerAnglesXYZ(orientationMatrix));
	EndTransform(true);
}

void ok::Transform::SetRotationOrder(ok::RotationOrder new_order)
{
	_rotation_order = new_order;

	BeginTransform(ok::TransformSpace::LocalSpace);

	switch (new_order)
	{
		case ok::RotationOrder::XYZ :
		{
			SetRotation(_ConvertMatToEulerAnglesXYZ(_relativeRotationMatrix));
		}
		break;
		case ok::RotationOrder::XZY:
		{
			SetRotation(_ConvertMatToEulerAnglesXZY(_relativeRotationMatrix));
		}
		break;
		case ok::RotationOrder::YXZ:
		{
			SetRotation(_ConvertMatToEulerAnglesYXZ(_relativeRotationMatrix));
		}
		break;
		case ok::RotationOrder::YZX:
		{
			SetRotation(_ConvertMatToEulerAnglesYZX(_relativeRotationMatrix));
		}
		break;
		case ok::RotationOrder::ZXY:
		{
			SetRotation(_ConvertMatToEulerAnglesZXY(_relativeRotationMatrix));
		}
		break;
		case ok::RotationOrder::ZYX:
		{
			SetRotation(_ConvertMatToEulerAnglesZYX(_relativeRotationMatrix));
		}
		break;
	}

	EndTransform(true);
}

void ok::Transform::OnChange()
{
}

void ok::Transform::CopyPaste(ok::Transform & copyFrom, ok::Transform & pasteTo, bool updateChildrens, ok::TransformSpace space)
{
	pasteTo.SetRotationOrder(copyFrom._rotation_order);

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
	switch (_rotation_order)
	{
	case ok::RotationOrder::XYZ:
	{
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.x), glm::vec3(1.f, 0.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.y), glm::vec3(0.f, 1.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.z), glm::vec3(0.f, 0.f, 1.f));
	}
	break;
	case ok::RotationOrder::XZY:
	{
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.x), glm::vec3(1.f, 0.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.z), glm::vec3(0.f, 0.f, 1.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.y), glm::vec3(0.f, 1.f, 0.f));	
	}
	break;
	case ok::RotationOrder::YXZ:
	{
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.y), glm::vec3(0.f, 1.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.x), glm::vec3(1.f, 0.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.z), glm::vec3(0.f, 0.f, 1.f));
	}
	break;
	case ok::RotationOrder::YZX:
	{
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.y), glm::vec3(0.f, 1.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.z), glm::vec3(0.f, 0.f, 1.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.x), glm::vec3(1.f, 0.f, 0.f));
	}
	break;
	case ok::RotationOrder::ZXY:
	{
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.z), glm::vec3(0.f, 0.f, 1.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.x), glm::vec3(1.f, 0.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.y), glm::vec3(0.f, 1.f, 0.f));
	}
	break;
	case ok::RotationOrder::ZYX:
	{
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.z), glm::vec3(0.f, 0.f, 1.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.y), glm::vec3(0.f, 1.f, 0.f));
		_relativeRotationMatrix = glm::rotate(_relativeRotationMatrix, glm::radians(_relativeRotationEuler.x), glm::vec3(1.f, 0.f, 0.f));
	}
	break;
	}

	_relativeTransformMatrix = glm::translate(_relativeTransformMatrix, _relativePosition) * _relativeRotationMatrix * glm::scale(_relativeTransformMatrix, _relativeScale);

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
}

void ok::Transform::_UpdateOrientationVectors()
{
	_forward =	_absoluteRotationMatrix * glm::vec4(0.f, 0.f, 1.f, 0.f);
	_up		 = _absoluteRotationMatrix * glm::vec4(0.f, 1.f, 0.f, 0.f);
	_right	 = _absoluteRotationMatrix * glm::vec4(1.f, 0.f, 0.f, 0.f);

	_forward = glm::normalize(_forward);
	_up		 = glm::normalize(_up);
	_right	 = glm::normalize(_right);

	_orientation_dirty = false;
}

/*glm::vec3 ok::Transform::_ConvertMatToEulerAnglesXYZ(const glm::mat3 & mat)
{
	const float xEuler = std::atan2(mat[1].z, mat[2].z);
	const float cosYangle = glm::sqrt(glm::pow(mat[0].x, 2) + glm::pow(mat[0].y, 2));
	const float yEuler = std::atan2(-mat[0].z, cosYangle);
	const float sinXangle = glm::sin(xEuler);
	const float cosXangle = glm::cos(xEuler);
	const float zEuler = std::atan2(
		sinXangle * mat[2].x - cosXangle * mat[1].x,
		cosXangle * mat[1].y - sinXangle * mat[2].y);

	return -glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}*/
/*
glm::vec3 ok::Transform::_ConvertMatToEulerAnglesXYZ(const glm::mat3 & mat)
{
	const float xEuler = std::atan2(-mat[1].z, mat[2].z);
	const float cosYangle = glm::sqrt(1.f - glm::pow(mat[0].z, 2));
	const float yEuler = std::atan2(mat[0].z, cosYangle);
	const float zEuler = std::atan2(-mat[0].y, mat[0].x);

	return glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}*/

glm::vec3 ok::Transform::_ConvertMatToEulerAnglesXYZ(const glm::mat3 & mat)
{
	const float xEuler = std::atan2(-mat[2].y, mat[2].z);
	const float cos_angle = glm::sqrt(1.f - glm::pow(mat[2].x, 2));
	const float yEuler = std::atan2(mat[2].x, cos_angle);
	const float zEuler = std::atan2(-mat[1].x, mat[0].x);

	return glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}

glm::vec3 ok::Transform::_ConvertMatToEulerAnglesXZY(const glm::mat3 & mat)
{
	const float xEuler = std::atan2(mat[1].z, mat[1].y);
	const float cos_angle = glm::sqrt(1.f - glm::pow(mat[1].x, 2));
	const float zEuler = std::atan2(-mat[1].x, cos_angle);
	const float yEuler = std::atan2(mat[2].x, mat[0].x);

	return glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}

glm::vec3 ok::Transform::_ConvertMatToEulerAnglesYXZ(const glm::mat3 & mat)
{
	const float yEuler = std::atan2(mat[0].z, mat[2].z);
	const float cos_angle = glm::sqrt(1.f - glm::pow(mat[2].y, 2));
	const float xEuler = std::atan2(-mat[2].y, cos_angle);
	const float zEuler = std::atan2(mat[0].x, mat[1].y);

	return glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}

glm::vec3 ok::Transform::_ConvertMatToEulerAnglesYZX(const glm::mat3 & mat)
{
	const float yEuler = std::atan2(-mat[0].z, mat[0].x);
	const float cos_angle = glm::sqrt(1.f - glm::pow(mat[0].y, 2));
	const float zEuler = std::atan2(mat[0].y, cos_angle);
	const float xEuler = std::atan2(-mat[2].y, mat[1].y);

	return glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}

glm::vec3 ok::Transform::_ConvertMatToEulerAnglesZXY(const glm::mat3 & mat)
{
	const float zEuler = std::atan2(-mat[1].x, mat[1].y);
	const float cos_angle = glm::sqrt(1.f - glm::pow(mat[1].z, 2));
	const float xEuler = std::atan2(mat[1].z, cos_angle);
	const float yEuler = std::atan2(-mat[0].z, mat[2].z);

	return glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}

glm::vec3 ok::Transform::_ConvertMatToEulerAnglesZYX(const glm::mat3 & mat)
{
	const float zEuler = std::atan2(mat[0].y, mat[0].x);
	const float cos_angle = glm::sqrt(1.f - glm::pow(mat[0].z, 2));
	const float yEuler = std::atan2(-mat[0].z, cos_angle);
	const float xEuler = std::atan2(mat[1].z, mat[2].z);

	return glm::degrees(glm::vec3(xEuler, yEuler, zEuler));
}

