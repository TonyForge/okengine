#pragma once

#include "Animation.h"

namespace ok
{
	namespace graphics
	{
		enum class VertexAnimationInfluence
		{
			Positions,
			Positions_Normals
		};

		class VertexAnimationFrame : public ok::graphics::AnimationFrame
		{
		public:
			float* positions;
			float* normals;
		};

		class VertexAnimation : public ok::graphics::Animation
		{
		public:
			VertexAnimation(ok::String& name, int frames_count, int vertex_count, ok::graphics::VertexAnimationInfluence influence);
			std::vector<float>& GetFramesRawData();
		protected:
		private:
			std::vector<ok::graphics::VertexAnimationFrame> __frames;
			std::vector<float> _frames_raw_data;
		};

		class VertexAnimationMixer : public ok::graphics::AnimationMixer
		{
		public:
			void MixBegin(int bytes_per_frame);
			void MixAdd(ok::graphics::AnimationController* state, ok::graphics::AnimationMixFunction fnc, float blend);
			void MixEnd();
			ok::graphics::AnimationFrame* GetMixResult();
		private:
			ok::graphics::VertexAnimationFrame _mix_result;
			std::vector<float> _mix_result_data;
		};
	}
}