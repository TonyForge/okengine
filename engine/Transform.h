#pragma once

namespace ok
{
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
		void EndTransform();

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

		virtual void OnChange();

	protected:
		std::list<ok::Transform*> _childrens;
	private:
		ok::Transform* _parent;
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