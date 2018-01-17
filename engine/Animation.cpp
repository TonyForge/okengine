#include "Animation.h"

ok::graphics::Animation::Animation(ok::String& name, int frames_count, ok::graphics::AnimationType type)
{
	_length = frames_count;
	_name = name;
	_type = type;
}

int ok::graphics::Animation::GetLength()
{
	return _length;
}

ok::graphics::AnimationFrame* ok::graphics::Animation::GetFrame(int frame_index)
{
	return _frames[frame_index];
}

ok::String ok::graphics::Animation::GetName()
{
	return _name;
}

ok::graphics::AnimationType ok::graphics::Animation::GetType()
{
	return _type;
}

void ok::graphics::AnimationController::GoToFrame(float frame, ok::graphics::AnimationPlayMode mode)
{
	int _animation_length = _animation->GetLength() - 1;

	if (_animation_length == 0)
	{
		_current_frame = 0;
		_next_frame = 0;
		_frames_blend = 0.f;
		return;
	}

	_current_frame = static_cast<int>(glm::floor(frame));
	_frames_blend = frame - _current_frame;

	if (mode == ok::graphics::AnimationPlayMode::Loop)
	{
		_current_frame = _current_frame % _animation_length;

		if (_current_frame < 0)
		{
			_current_frame = (_animation_length + 1) + _current_frame;
		}

		_next_frame = (_current_frame + 1) % _animation_length;
	}
	else if (mode == ok::graphics::AnimationPlayMode::Clamp)
	{
		_current_frame = glm::clamp(_current_frame, 0, _animation_length);
		_next_frame = glm::clamp(_current_frame + 1, 0, _animation_length);
	}

	_OnChange();
}

void ok::graphics::AnimationController::GoToNormalizedTime(float normalized_time, ok::graphics::AnimationPlayMode mode)
{
	GoToFrame(normalized_time * static_cast<float>(_animation->GetLength() - 1), mode);
}

void ok::graphics::AnimationController::SetAnimation(ok::graphics::Animation * animation)
{
	_animation = animation;
}

ok::graphics::Animation * ok::graphics::AnimationController::GetAnimation()
{
	return _animation;
}

ok::graphics::AnimationFrame* ok::graphics::AnimationController::GetLeftFrame()
{
	return _left_frame;
}

ok::graphics::AnimationFrame* ok::graphics::AnimationController::GetRightFrame()
{
	return _right_frame;
}

float ok::graphics::AnimationController::GetFramesBlend()
{
	return _frames_blend;
}

void ok::graphics::AnimationController::_OnChange()
{
	_left_frame = _animation->GetFrame(_current_frame);
	_right_frame = _animation->GetFrame(_next_frame);
}
