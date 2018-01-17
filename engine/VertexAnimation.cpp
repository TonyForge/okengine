#include "VertexAnimation.h"

ok::graphics::VertexAnimation::VertexAnimation(ok::String& name, int frames_count, int vertex_count, ok::graphics::VertexAnimationInfluence influence) : ok::graphics::Animation(name, frames_count, ok::graphics::AnimationType::Vertex)
{
	int record_length = (
		((influence == ok::graphics::VertexAnimationInfluence::Positions) ? 3 : 0) +
		((influence == ok::graphics::VertexAnimationInfluence::Positions_Normals) ? 6 : 0)
		);

	_frames.resize(frames_count);
	__frames.resize(frames_count);
	_frames_raw_data.resize(frames_count * vertex_count * record_length);

	int i = 0;
	for (auto& frame : _frames)
	{
		frame = &__frames[i];
		ok::graphics::VertexAnimationFrame& ref = *(ok::graphics::VertexAnimationFrame*)frame;

		ref.positions = &_frames_raw_data[i*vertex_count*record_length];

		if (influence == ok::graphics::VertexAnimationInfluence::Positions_Normals)
			ref.normals = ref.positions + 3 * vertex_count;
		else
			ref.normals = nullptr;

		i++;
	}
}

std::vector<float>& ok::graphics::VertexAnimation::GetFramesRawData()
{
	return _frames_raw_data;
}

void ok::graphics::VertexAnimationMixer::MixBegin(int bytes_per_frame)
{
	int count = bytes_per_frame / sizeof(float);
	_mix_result_data.resize(count);
	std::fill(_mix_result_data.begin(), _mix_result_data.end(), 0.f);

	_mix_result.positions = _mix_result_data.data();
	_mix_result.normals = _mix_result_data.data() + (count/2);
}

void ok::graphics::VertexAnimationMixer::MixAdd(ok::graphics::AnimationController * state, ok::graphics::AnimationMixFunction fnc, float blend)
{
	if (blend == 0.f) return;

	ok::graphics::VertexAnimationFrame* left_frame = static_cast<ok::graphics::VertexAnimationFrame*>(state->GetLeftFrame());
	ok::graphics::VertexAnimationFrame* right_frame = static_cast<ok::graphics::VertexAnimationFrame*>(state->GetRightFrame());
	float frames_blend = state->GetFramesBlend();

	bool normals_enabled = (left_frame->normals != nullptr);
	int positions_end = _mix_result_data.size() / 2;

	if (fnc == ok::graphics::AnimationMixFunction::Lerp)
	{
		if (blend == 1.f)
		{
			if (frames_blend == 0.f)
			{
				if (normals_enabled)
				{
					memcpy(_mix_result.positions, left_frame->positions, sizeof(float) * _mix_result_data.size() / 2);
					memcpy(_mix_result.normals, left_frame->normals, sizeof(float) * _mix_result_data.size() / 2);
				}
				else
				{
					memcpy(_mix_result.positions, left_frame->positions, sizeof(float) * _mix_result_data.size());
				}
				
			}
			else if (frames_blend == 1.f)
			{
				if (normals_enabled)
				{
					memcpy(_mix_result.positions, right_frame->positions, sizeof(float) * _mix_result_data.size() / 2);
					memcpy(_mix_result.normals, right_frame->normals, sizeof(float) * _mix_result_data.size() / 2);
				}
				else
				{
					memcpy(_mix_result.positions, right_frame->positions, sizeof(float) * _mix_result_data.size());
				}
			}
			else
			{
				int chunk_index = 0;

				if (normals_enabled)
				{
					for (auto& chunk : _mix_result_data)
					{
						if (chunk_index < positions_end)
							chunk = left_frame->positions[chunk_index] * (1.f - frames_blend) + right_frame->positions[chunk_index] * frames_blend;
						else
							chunk = left_frame->normals[chunk_index - positions_end] * (1.f - frames_blend) + right_frame->normals[chunk_index - positions_end] * frames_blend;
						chunk_index++;
					}
				}
				else
				{
					for (auto& chunk : _mix_result_data)
					{
						chunk = left_frame->positions[chunk_index] * (1.f - frames_blend) + right_frame->positions[chunk_index] * frames_blend;
						chunk_index++;
					}
				}
			}
		}
		else
		{
			if (frames_blend == 0.f)
			{
				int chunk_index = 0;

				if (normals_enabled)
				{
					for (auto& chunk : _mix_result_data)
					{
						if (chunk_index < positions_end)
							chunk = chunk * (1.f - blend) + left_frame->positions[chunk_index] * blend;
						else
							chunk = chunk * (1.f - blend) + left_frame->normals[chunk_index - positions_end] * blend;
						chunk_index++;
					}
				}
				else
				{
					for (auto& chunk : _mix_result_data)
					{
						chunk = chunk * (1.f - blend) + left_frame->positions[chunk_index] * blend;
						chunk_index++;
					}
				}

			}
			else if (frames_blend == 1.f)
			{
				int chunk_index = 0;

				if (normals_enabled)
				{
					for (auto& chunk : _mix_result_data)
					{
						if (chunk_index < positions_end)
							chunk = chunk * (1.f - blend) + right_frame->positions[chunk_index] * blend;
						else
							chunk = chunk * (1.f - blend) + right_frame->normals[chunk_index - positions_end] * blend;
						chunk_index++;
					}
				}
				else
				{
					for (auto& chunk : _mix_result_data)
					{
						chunk = chunk * (1.f - blend) + right_frame->positions[chunk_index] * blend;
						chunk_index++;
					}
				}
			}
			else
			{
				int chunk_index = 0;

				if (normals_enabled)
				{
					for (auto& chunk : _mix_result_data)
					{
						if (chunk_index < positions_end)
							chunk = chunk * (1.f - blend) + (left_frame->positions[chunk_index] * (1.f - frames_blend) + right_frame->positions[chunk_index] * frames_blend)*blend;
						else
							chunk = chunk * (1.f - blend) + (left_frame->normals[chunk_index- positions_end] * (1.f - frames_blend) + right_frame->normals[chunk_index- positions_end] * frames_blend)*blend;
						chunk_index++;
					}
				}
				else
				{
					for (auto& chunk : _mix_result_data)
					{
						chunk = chunk * (1.f - blend) + (left_frame->positions[chunk_index] * (1.f - frames_blend) + right_frame->positions[chunk_index] * frames_blend)*blend;
						chunk_index++;
					}
				}

			}
		}
	}
	else if (fnc == ok::graphics::AnimationMixFunction::Add)
	{
		if (frames_blend == 0.f)
		{
			int chunk_index = 0;

			if (normals_enabled)
			{
				for (auto& chunk : _mix_result_data)
				{
					if (chunk_index < positions_end)
						chunk = chunk + left_frame->positions[chunk_index] * blend;
					else
						chunk = chunk + left_frame->normals[chunk_index - positions_end] * blend;
					chunk_index++;
				}
			}
			else
			{
				for (auto& chunk : _mix_result_data)
				{
					chunk = chunk + left_frame->positions[chunk_index] * blend;
					chunk_index++;
				}
			}
		}
		else if (frames_blend == 1.f)
		{
			int chunk_index = 0;

			if (normals_enabled)
			{
				for (auto& chunk : _mix_result_data)
				{
					if (chunk_index < positions_end)
						chunk = chunk + right_frame->positions[chunk_index] * blend;
					else
						chunk = chunk + right_frame->normals[chunk_index - positions_end] * blend;
					chunk_index++;
				}
			}
			else
			{
				for (auto& chunk : _mix_result_data)
				{
					chunk = chunk + right_frame->positions[chunk_index] * blend;
					chunk_index++;
				}
			}
		}
		else
		{
			int chunk_index = 0;

			if (normals_enabled)
			{
				for (auto& chunk : _mix_result_data)
				{
					if (chunk_index < positions_end)
						chunk = chunk + (left_frame->positions[chunk_index] * (1.f - frames_blend) + right_frame->positions[chunk_index] * frames_blend)*blend;
					else
						chunk = chunk + (left_frame->normals[chunk_index - positions_end] * (1.f - frames_blend) + right_frame->normals[chunk_index - positions_end] * frames_blend)*blend;
					chunk_index++;
				}
			}
			else
			{
				for (auto& chunk : _mix_result_data)
				{
					chunk = chunk + (left_frame->positions[chunk_index] * (1.f - frames_blend) + right_frame->positions[chunk_index] * frames_blend)*blend;
					chunk_index++;
				}
			}
		}
	}
}

void ok::graphics::VertexAnimationMixer::MixEnd()
{
}

ok::graphics::AnimationFrame * ok::graphics::VertexAnimationMixer::GetMixResult()
{
	return &_mix_result;
}
