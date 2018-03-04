#include "TransformAnimation.h"

ok::graphics::TransformAnimation::TransformAnimation(ok::String & name, int frames_count, int transforms_count) : ok::graphics::Animation(name, frames_count, ok::graphics::AnimationType::Transform)
{
	_frames.resize(frames_count);
	__frames.resize(frames_count);
	_frames_raw_data.resize(frames_count*transforms_count * 9);

	int i = 0;
	for (auto& frame : _frames)
	{
		frame = &__frames[i];
		ok::graphics::TransformAnimationFrame& ref = *(ok::graphics::TransformAnimationFrame*)frame;

		ref.trs_frames = &_frames_raw_data[i*transforms_count * 9];

		i++;
	}
}

std::vector<float>& ok::graphics::TransformAnimation::GetFramesRawData()
{
	return _frames_raw_data;
}

ok::graphics::TransformAnimationSkeleton::TransformAnimationSkeleton(int skeleton_id)
{
	_skeleton_id = skeleton_id;
}

void ok::graphics::TransformAnimationSkeleton::Update(float dt)
{
}

ok::Behaviour * ok::graphics::TransformAnimationSkeleton::Duplicate(ok::Behaviour * _clone)
{
	ok::graphics::TransformAnimationSkeleton* __clone = new ok::graphics::TransformAnimationSkeleton(_skeleton_id);
	ok::Behaviour::Duplicate(__clone);

	__clone->_bones_bind_pose_trs = _bones_bind_pose_trs;
	__clone->_bones_bind_pose_inv_mat = _bones_bind_pose_inv_mat;

	return __clone;
}

void ok::graphics::TransformAnimationSkeleton::RevisitBones()
{
	ok::GameObject* skeleton = &gameObject();

	int __skeleton_id = _skeleton_id;
	std::vector<ok::GameObject*>& __bones = _bones;

	__bones.resize(0);

	std::function<void(ok::GameObject*)> visitor = [&visitor, &__bones, __skeleton_id](ok::GameObject* bone) {

		ok::graphics::TransformAnimationBone* bone_asset = bone->GetComponent<ok::graphics::TransformAnimationBone>();

		if (bone_asset != nullptr && bone_asset->IsCompatible(__skeleton_id))
			__bones.push_back(bone);

		for (auto&& child : bone->GetChildrens())
		{
			visitor((ok::GameObject*)child);
		}
	};

	visitor(skeleton);

	auto sort_function = [](ok::GameObject*& left, ok::GameObject*& right) {
		return left->GetComponent<ok::graphics::TransformAnimationBone>()->GetBoneId() < right->GetComponent<ok::graphics::TransformAnimationBone>()->GetBoneId();
	};

	std::sort(__bones.begin(), __bones.end(), sort_function);
}

void ok::graphics::TransformAnimationSkeleton::OnGameObjectDuplicateEnd()
{
	RevisitBones();
}

void ok::graphics::TransformAnimationSkeleton::AnimateBones(ok::graphics::TransformAnimationFrame * left_frame, ok::graphics::TransformAnimationFrame * right_frame, float frames_blend)
{
	std::vector<ok::GameObject*>& skeleton_bones = GetBones();

	int skeleton_bone_index = 0;
	for (auto&& bone : skeleton_bones)
	{
		bone->BeginTransform();
		bone->SetPosition(
			glm::vec3(
				left_frame->trs_frames[skeleton_bone_index * 9 + 0],
				left_frame->trs_frames[skeleton_bone_index * 9 + 1],
				left_frame->trs_frames[skeleton_bone_index * 9 + 2]
			) * (1.f - frames_blend) +
			glm::vec3(
				right_frame->trs_frames[skeleton_bone_index * 9 + 0],
				right_frame->trs_frames[skeleton_bone_index * 9 + 1],
				right_frame->trs_frames[skeleton_bone_index * 9 + 2]
			) * (frames_blend)
		);
		bone->SetRotation(
			glm::vec3(
				left_frame->trs_frames[skeleton_bone_index * 9 + 3],
				left_frame->trs_frames[skeleton_bone_index * 9 + 4],
				left_frame->trs_frames[skeleton_bone_index * 9 + 5]
			) * (1.f - frames_blend) +
			glm::vec3(
				right_frame->trs_frames[skeleton_bone_index * 9 + 3],
				right_frame->trs_frames[skeleton_bone_index * 9 + 4],
				right_frame->trs_frames[skeleton_bone_index * 9 + 5]
			) * (frames_blend)
		);
		bone->SetScale(
			(glm::vec3(
				left_frame->trs_frames[skeleton_bone_index * 9 + 6],
				left_frame->trs_frames[skeleton_bone_index * 9 + 7],
				left_frame->trs_frames[skeleton_bone_index * 9 + 8]
			) * (1.f - frames_blend) +
				glm::vec3(
					right_frame->trs_frames[skeleton_bone_index * 9 + 6],
					right_frame->trs_frames[skeleton_bone_index * 9 + 7],
					right_frame->trs_frames[skeleton_bone_index * 9 + 8]
				) * (frames_blend))
		);
		bone->EndTransform(false);

		skeleton_bone_index++;
	}
}

void ok::graphics::TransformAnimationSkeleton::GrabBones(float * out)
{
	std::vector<ok::GameObject*>& skeleton_bones = GetBones();
	std::vector<glm::mat4>& skeleton_bind_pose = GetBindPoseInvMat();

	int skeleton_bone_index = 0;

	for (auto&& bone : skeleton_bones)
	{
		memcpy(&out[skeleton_bone_index * 16], glm::value_ptr(bone->GetAbsoluteTransformMatrix() * skeleton_bind_pose[skeleton_bone_index]), sizeof(float) * 16);
		skeleton_bone_index++;
	}
}

std::vector<ok::GameObject*>& ok::graphics::TransformAnimationSkeleton::GetBones()
{
	return _bones;
}

std::vector<float>& ok::graphics::TransformAnimationSkeleton::GetBindPose()
{
	return _bones_bind_pose_trs;
}

std::vector<glm::mat4>& ok::graphics::TransformAnimationSkeleton::GetBindPoseInvMat()
{
	return _bones_bind_pose_inv_mat;
}

ok::graphics::TransformAnimationBone::TransformAnimationBone(int skeleton_id, int bone_id)
{
	_skeleton_id = skeleton_id;
	_bone_id = bone_id;
}

void ok::graphics::TransformAnimationBone::Update(float dt)
{
}

ok::Behaviour * ok::graphics::TransformAnimationBone::Duplicate(ok::Behaviour * _clone)
{
	ok::graphics::TransformAnimationBone* __clone = new ok::graphics::TransformAnimationBone(_skeleton_id, _bone_id);
	ok::Behaviour::Duplicate(__clone);

	return __clone;
}

bool ok::graphics::TransformAnimationBone::IsCompatible(int skeleton_id)
{
	return (_skeleton_id == skeleton_id);
}

int ok::graphics::TransformAnimationBone::GetBoneId()
{
	return _bone_id;
}

void ok::graphics::TransformAnimationMixer::MixBegin(int bytes_per_frame)
{
	int count = bytes_per_frame / sizeof(float);
	_mix_result_data.resize(count);
	std::fill(_mix_result_data.begin(), _mix_result_data.end(), 0.f);

	_mix_result.trs_frames = _mix_result_data.data();
}

void ok::graphics::TransformAnimationMixer::MixAdd(ok::graphics::AnimationController * state, ok::graphics::AnimationMixFunction fnc, float blend)
{
	if (blend == 0.f) return;

	ok::graphics::TransformAnimationFrame* left_frame = static_cast<ok::graphics::TransformAnimationFrame*>(state->GetLeftFrame());
	ok::graphics::TransformAnimationFrame* right_frame = static_cast<ok::graphics::TransformAnimationFrame*>(state->GetRightFrame());
	float frames_blend = state->GetFramesBlend();

	if (fnc == ok::graphics::AnimationMixFunction::Lerp)
	{
		if (blend == 1.f)
		{
			if (frames_blend == 0.f)
				memcpy(_mix_result_data.data(), left_frame->trs_frames, sizeof(float) * _mix_result_data.size());
			else if (frames_blend == 1.f)
				memcpy(_mix_result_data.data(), right_frame->trs_frames, sizeof(float) * _mix_result_data.size());
			else
			{
				int chunk_index = 0;
				for (auto& chunk : _mix_result_data)
				{
					chunk = left_frame->trs_frames[chunk_index] * (1.f - frames_blend) + right_frame->trs_frames[chunk_index] * frames_blend;
					chunk_index++;
				}
			}
		}
		else
		{
			if (frames_blend == 0.f)
			{
				int chunk_index = 0;
				for (auto& chunk : _mix_result_data)
				{
					chunk = chunk * (1.f - blend) + left_frame->trs_frames[chunk_index] * blend;
					chunk_index++;
				}
			}
			else if (frames_blend == 1.f)
			{
				int chunk_index = 0;
				for (auto& chunk : _mix_result_data)
				{
					chunk = chunk * (1.f - blend) + right_frame->trs_frames[chunk_index] * blend;
					chunk_index++;
				}
			}
			else
			{
				int chunk_index = 0;
				for (auto& chunk : _mix_result_data)
				{
					chunk = chunk * (1.f - blend) + (left_frame->trs_frames[chunk_index] * (1.f - frames_blend) + right_frame->trs_frames[chunk_index] * frames_blend)*blend;
					chunk_index++;
				}
			}
		}
	}
	else if (fnc == ok::graphics::AnimationMixFunction::Add)
	{
		if (frames_blend == 0.f)
		{
			int chunk_index = 0;
			for (auto& chunk : _mix_result_data)
			{
				chunk = chunk + left_frame->trs_frames[chunk_index] * blend;
				chunk_index++;
			}
		}
		else if (frames_blend == 1.f)
		{
			int chunk_index = 0;
			for (auto& chunk : _mix_result_data)
			{
				chunk = chunk + right_frame->trs_frames[chunk_index] * blend;
				chunk_index++;
			}
		}
		else
		{
			int chunk_index = 0;
			for (auto& chunk : _mix_result_data)
			{
				chunk = chunk + (left_frame->trs_frames[chunk_index] * (1.f - frames_blend) + right_frame->trs_frames[chunk_index] * frames_blend)*blend;
				chunk_index++;
			}
		}
	}
}

void ok::graphics::TransformAnimationMixer::MixEnd()
{
}

ok::graphics::AnimationFrame * ok::graphics::TransformAnimationMixer::GetMixResult()
{
	return &_mix_result;
}
