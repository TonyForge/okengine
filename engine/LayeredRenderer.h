#pragma once

#include "Camera.h"

namespace ok
{
	namespace graphics
	{
		enum class LayeredRendererStage
		{
			BackStage = 0,
			Stage,
			FrontStage
		};

		class LayeredRenderable
		{
		public:
			virtual void Render() = 0;
		};

		class LayeredRendererRequest
		{
		public:
			ok::graphics::LayeredRendererStage stage;
			int layer;
			bool transparent;

			glm::vec3 world_space_position;

			LayeredRendererRequest(
				ok::graphics::LayeredRenderable* _owner,
				ok::graphics::LayeredRendererStage _stage = ok::graphics::LayeredRendererStage::Stage,
				int _layer = 0,
				bool _transparent = false,
				glm::vec3 _world_space_position = glm::vec3(0.f, 0.f, 0.f)
			)
			{
				owner = _owner;
				stage = _stage;
				layer = _layer;
				transparent = _transparent;
				world_space_position = _world_space_position;
			}

			ok::graphics::LayeredRenderable* GetOwner();

		private:
			ok::graphics::LayeredRenderable* owner;
		};

		class LayeredRenderer
		{
		public:
			static ok::graphics::LayeredRenderer& instance();

			void Flush();
			void Render(ok::graphics::LayeredRendererRequest& request);

			void BeginImmediateRender();
			void EndImmediateRender();
		protected:
		private:
			LayeredRenderer() {}
			~LayeredRenderer() {}
			ok::graphics::LayeredRenderer(ok::graphics::LayeredRenderer const&) {}
			ok::graphics::LayeredRenderer& operator= (ok::graphics::LayeredRenderer const&) {}

			std::vector<std::pair<int, ok::graphics::LayeredRenderable*>> back_stage_opaque;
			std::vector<std::tuple<int, float, ok::graphics::LayeredRenderable*>> back_stage_transparent;

			std::vector<std::pair<int, ok::graphics::LayeredRenderable*>> stage_opaque;
			std::vector<std::tuple<int, float, ok::graphics::LayeredRenderable*>> stage_transparent;

			std::vector<std::pair<int, ok::graphics::LayeredRenderable*>> front_stage_opaque;
			std::vector<std::tuple<int, float, ok::graphics::LayeredRenderable*>> front_stage_transparent;

			//immediate mode collections
			std::vector<std::pair<int, ok::graphics::LayeredRenderable*>> im_back_stage_opaque;
			std::vector<std::tuple<int, float, ok::graphics::LayeredRenderable*>> im_back_stage_transparent;

			std::vector<std::pair<int, ok::graphics::LayeredRenderable*>> im_stage_opaque;
			std::vector<std::tuple<int, float, ok::graphics::LayeredRenderable*>> im_stage_transparent;

			std::vector<std::pair<int, ok::graphics::LayeredRenderable*>> im_front_stage_opaque;
			std::vector<std::tuple<int, float, ok::graphics::LayeredRenderable*>> im_front_stage_transparent;

			bool _immediate_mode_enabled = false;

			float _GetDistanceToCamera(glm::vec3 world_space_position);
		};
	}
}