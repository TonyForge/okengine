#pragma once

#include "Animation.h"
#include "Behaviour.h"
#include "GameObject.h"

namespace ok
{
	namespace graphics
	{
		class TransformAnimationFrame : public ok::graphics::AnimationFrame
		{
		public:
			float* trs_frames;
		};

		class TransformAnimation : public ok::graphics::Animation
		{
		public:
			TransformAnimation(ok::String& name, int frames_count, int transforms_count);
			std::vector<float>& GetFramesRawData();
		protected:
		private:
			std::vector<ok::graphics::TransformAnimationFrame> __frames;
			std::vector<float> _frames_raw_data;
		};

		class TransformAnimationBone : public ok::Behaviour
		{
		public:
			TransformAnimationBone(int skeleton_id, int bone_id);
			void Update(float dt);
			ok::Behaviour* Duplicate(ok::Behaviour * _clone = nullptr);
			bool IsCompatible(int skeleton_id);
			int GetBoneId();
		private:
			int _skeleton_id, _bone_id;
		};

		class TransformAnimationSkeleton : public ok::Behaviour
		{
		public:
			TransformAnimationSkeleton(int skeleton_id);
			void Update(float dt);
			ok::Behaviour* Duplicate(ok::Behaviour * _clone = nullptr);
			void RevisitBones();
			void OnGameObjectDuplicateEnd();

			void AnimateBones(
				ok::graphics::TransformAnimationFrame* left_frame,
				ok::graphics::TransformAnimationFrame* right_frame,
				float frames_blend);

			void GrabBones(float* out);

			std::vector<ok::GameObject*>& GetBones();
			std::vector<float>& GetBindPose();
			std::vector<glm::mat4>& GetBindPoseInvMat();
		private:
			std::vector<ok::GameObject*> _bones;
			std::vector<float> _bones_bind_pose_trs;
			std::vector<glm::mat4> _bones_bind_pose_inv_mat;
			int _skeleton_id;
		};

		class TransformAnimationMixer : public ok::graphics::AnimationMixer
		{
		public:
			void MixBegin(int bytes_per_frame);
			void MixAdd(ok::graphics::AnimationController* state, ok::graphics::AnimationMixFunction fnc, float blend);
			void MixEnd();
			ok::graphics::AnimationFrame* GetMixResult();
		private:
			ok::graphics::TransformAnimationFrame _mix_result;
			std::vector<float> _mix_result_data;
		};
	}
}