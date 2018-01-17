#pragma once

#include "GLBufferObject.h"
#include "Transform.h"


namespace ok
{
	namespace graphics
	{
		enum class AnimationMixFunction
		{
			Add,
			Lerp
		};

		enum class AnimationPlayMode
		{
			Loop,
			Clamp
		};

		enum class AnimationType
		{
			Vertex,
			Transform,
			Skeletal
		};

		class AnimationFrame
		{
		public:
		protected:
		private:
		};

		class Animation
		{
		public:
			Animation(ok::String& name, int frames_count, ok::graphics::AnimationType type);
			int GetLength();
			ok::graphics::AnimationFrame* GetFrame(int frame_index);
			ok::String GetName();
			ok::graphics::AnimationType GetType();
		protected:
			int _length;
			ok::String _name;
			std::vector<ok::graphics::AnimationFrame*> _frames;
		private:	
			ok::graphics::AnimationType _type;
		};

		class AnimationController
		{
		public:
			void GoToFrame(float frame, ok::graphics::AnimationPlayMode mode = ok::graphics::AnimationPlayMode::Loop);
			void GoToNormalizedTime(float normalized_time, ok::graphics::AnimationPlayMode mode = ok::graphics::AnimationPlayMode::Loop);
			void SetAnimation(ok::graphics::Animation* animation);
			ok::graphics::Animation* GetAnimation();
			ok::graphics::AnimationFrame* GetLeftFrame();
			ok::graphics::AnimationFrame* GetRightFrame();
			float GetFramesBlend();
		protected:		
		private:
			void _OnChange();
			ok::graphics::Animation* _animation;
			ok::graphics::AnimationFrame* _left_frame;
			ok::graphics::AnimationFrame* _right_frame;
			float _frames_blend;
			int _current_frame;
			int _next_frame;
		};

		class AnimationMixer
		{
		public:
			virtual void MixBegin(int bytes_per_frame) = 0;
			virtual void MixAdd(ok::graphics::AnimationController* state, ok::graphics::AnimationMixFunction fnc, float blend) = 0;
			virtual void MixEnd() = 0;
			virtual ok::graphics::AnimationFrame* GetMixResult() = 0;
		protected:
		private:
		};
	}
}