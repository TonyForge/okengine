#pragma once

namespace ok
{
	enum class RotationDirection
	{
		CW,
		CCW
	};

	/*
	Left To Right
	TRS = First Translate Then Rotate Then Scale
	*/
	enum class TransformCombineOrder
	{
		SRT, //(Default)Usual for non-uniform scaled objects in 3d world (non uniform scale will work in natural manner)
		RST, //Usual for uniform scaled object in 3d world (non uniform scale will skew and distort rotated objects)
		RTS //Usual for world transformation relative to camera (translations shifted by center done by horizontal/vertical axis of zoomed screen space regardless of rotation around center)
	};

	enum class TransformSpace
	{
		LocalSpace = 0,
		WorldSpace
	};

	class Transform
	{
	public:
		bool keepWorldTransform;
		ok::String name;
		size_t name_hash;

		Transform();

		void Rename(ok::String name);

		ok::Transform* FindChild(ok::String& name, bool nameIsPath = false);
		std::list<ok::Transform*>& GetChildrens();

		void AddChild(ok::Transform* child);
		void SetParent(ok::Transform* parent);
		void RemoveChild(ok::Transform* child);

		void AddChild(ok::Transform* child, bool forcedKeepWorldTransformValue);
		void SetParent(ok::Transform* parent, bool forcedKeepWorldTransformValue);
		void RemoveChild(ok::Transform* child, bool forcedKeepWorldTransformValue);

		void UpdateAbsoluteTransform(bool updateChildrens = true);
		glm::mat4 GetAbsoluteTransformMatrix();

		void BeginTransform(ok::TransformSpace space = ok::TransformSpace::LocalSpace);
		void EndTransform(bool updateChildrens);

		void SetPosition(glm::vec3 position);
		glm::vec3 GetPosition();

		void SetScale(glm::vec3 scale);
		glm::vec3 GetScale();

		void SetRotation(glm::vec3 eulerAnglesDegrees);
		glm::vec3 GetRotation();

		glm::vec3 GetForward();
		void SetForward(glm::vec3 forward);

		glm::vec3 GetUp();
		void SetUp(glm::vec3 up);

		glm::vec3 GetRight();
		void SetRight(glm::vec3 right);

		void SetOrientation(glm::vec3 forward, glm::vec3 up);
		void LookAt(glm::vec3 target, glm::vec3 up);
		void LookAt(glm::vec3 target);

		void SetRotationDirection(ok::RotationDirection new_direction);
		void SetTransformCombineOrder(ok::TransformCombineOrder new_order);

		virtual void OnChange();

		static void CopyPaste(ok::Transform& copyFrom, ok::Transform& pasteTo, bool updateChildrens, ok::TransformSpace space = ok::TransformSpace::LocalSpace);
	protected:
		std::list<ok::Transform*> _childrens;
	private:
		ok::Transform* _parent;
		ok::RotationDirection _rotation_direction;
		ok::TransformCombineOrder _transform_combine_order;

		glm::mat4 _relativeTransformMatrix;
		glm::mat4 _relativeRotationMatrix;
		glm::mat4 _absoluteTransformMatrix;
		glm::mat4 _absoluteRotationMatrix;

		glm::vec3 _relativePosition;
		glm::vec3 _relativeRotationEuler;
		glm::quat _relativeRotationQuat;
		glm::vec3 _relativeScale;

		glm::vec3 _forward;
		glm::vec3 _up;
		glm::vec3 _right;
		bool _orientation_dirty;

		bool _transformSequenceEnabled;
		bool _transformSequenceNoChanges;
		ok::TransformSpace _transformSequenceSpace;

		void _UpdateRelativeTransformMatrix();
		void _UpdateAbsoluteTransformMatrix();
		void _UpdateOrientationVectors();

		glm::vec3 _ConvertMatToEulerAnglesXYZ(const glm::mat3& mat);

		glm::vec3 _absolutePosition;
		glm::vec3 _absoluteRotationEuler;
		glm::vec3 _absoluteScale;

		void _AddChild(ok::Transform* child, bool _keepWorldTransform);
		void _SetParent(ok::Transform* parent, bool _keepWorldTransform);
		void _RemoveChild(ok::Transform* child, bool _keepWorldTransform);
	};
}