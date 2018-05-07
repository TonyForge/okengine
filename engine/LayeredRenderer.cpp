#include "LayeredRenderer.h"

ok::graphics::LayeredRenderer & ok::graphics::LayeredRenderer::instance()
{
	static ok::graphics::LayeredRenderer s;
	return s;
}

void ok::graphics::LayeredRenderer::Flush()
{

	auto sort_function_opaque = [](const std::pair<int, ok::graphics::LayeredRenderable*>& left, const std::pair<int, ok::graphics::LayeredRenderable*>& right) {
		return left.first < right.first;
	};

	auto sort_function_transparent = [](const std::tuple<int, float, ok::graphics::LayeredRenderable*>& left, const std::tuple<int, float, ok::graphics::LayeredRenderable*>& right) {
		if (std::get<0>(left) == std::get<0>(right))
		{
			return std::get<1>(left) > std::get<1>(right);
		}
		
		return std::get<0>(left) < std::get<0>(right);
	};

	if (_immediate_mode_enabled == true)
	{
		std::stable_sort(im_back_stage_opaque.begin(), im_back_stage_opaque.end(), sort_function_opaque);
		std::stable_sort(im_back_stage_transparent.begin(), im_back_stage_transparent.end(), sort_function_transparent);
		std::stable_sort(im_stage_opaque.begin(), im_stage_opaque.end(), sort_function_opaque);
		std::stable_sort(im_stage_transparent.begin(), im_stage_transparent.end(), sort_function_transparent);
		std::stable_sort(im_front_stage_opaque.begin(), im_front_stage_opaque.end(), sort_function_opaque);
		std::stable_sort(im_front_stage_transparent.begin(), im_front_stage_transparent.end(), sort_function_transparent);

		for (auto& ptr : im_back_stage_opaque) ptr.second->Render();
		for (auto& ptr : im_back_stage_transparent) std::get<2>(ptr)->Render();
		for (auto& ptr : im_stage_opaque) ptr.second->Render();
		for (auto& ptr : im_stage_transparent) std::get<2>(ptr)->Render();
		for (auto& ptr : im_front_stage_opaque) ptr.second->Render();
		for (auto& ptr : im_front_stage_transparent) std::get<2>(ptr)->Render();

		im_back_stage_opaque.resize(0);
		im_back_stage_transparent.resize(0);
		im_stage_opaque.resize(0);
		im_stage_transparent.resize(0);
		im_front_stage_opaque.resize(0);
		im_front_stage_transparent.resize(0);
	}
	else
	{
		std::stable_sort(back_stage_opaque.begin(), back_stage_opaque.end(), sort_function_opaque);
		std::stable_sort(back_stage_transparent.begin(), back_stage_transparent.end(), sort_function_transparent);
		std::stable_sort(stage_opaque.begin(), stage_opaque.end(), sort_function_opaque);
		std::stable_sort(stage_transparent.begin(), stage_transparent.end(), sort_function_transparent);
		std::stable_sort(front_stage_opaque.begin(), front_stage_opaque.end(), sort_function_opaque);
		std::stable_sort(front_stage_transparent.begin(), front_stage_transparent.end(), sort_function_transparent);

		for (auto& ptr : back_stage_opaque) ptr.second->Render();
		for (auto& ptr : back_stage_transparent) std::get<2>(ptr)->Render();
		for (auto& ptr : stage_opaque) ptr.second->Render();
		for (auto& ptr : stage_transparent) std::get<2>(ptr)->Render();
		for (auto& ptr : front_stage_opaque) ptr.second->Render();
		for (auto& ptr : front_stage_transparent) std::get<2>(ptr)->Render();

		back_stage_opaque.resize(0);
		back_stage_transparent.resize(0);
		stage_opaque.resize(0);
		stage_transparent.resize(0);
		front_stage_opaque.resize(0);
		front_stage_transparent.resize(0);
	}
	
}

void ok::graphics::LayeredRenderer::Render(ok::graphics::LayeredRendererRequest& request)
{
	if (_immediate_mode_enabled)
	{
		switch (request.stage)
		{
		case ok::graphics::LayeredRendererStage::BackStage:
		{
			if (request.transparent)
			{
				im_back_stage_transparent.push_back(std::tuple<int, float, ok::graphics::LayeredRenderable*>(request.layer, _GetDistanceToCamera(request.world_space_position), request.GetOwner()));
			}
			else
			{
				im_back_stage_opaque.push_back(std::pair<int, ok::graphics::LayeredRenderable*>(request.layer, request.GetOwner()));
			}
		}
		break;
		case ok::graphics::LayeredRendererStage::Stage:
		{
			if (request.transparent)
			{
				im_stage_transparent.push_back(std::tuple<int, float, ok::graphics::LayeredRenderable*>(request.layer, _GetDistanceToCamera(request.world_space_position), request.GetOwner()));
			}
			else
			{
				im_stage_opaque.push_back(std::pair<int, ok::graphics::LayeredRenderable*>(request.layer, request.GetOwner()));
			}
		}
		break;
		case ok::graphics::LayeredRendererStage::FrontStage:
		{
			if (request.transparent)
			{
				im_front_stage_transparent.push_back(std::tuple<int, float, ok::graphics::LayeredRenderable*>(request.layer, _GetDistanceToCamera(request.world_space_position), request.GetOwner()));
			}
			else
			{
				im_front_stage_opaque.push_back(std::pair<int, ok::graphics::LayeredRenderable*>(request.layer, request.GetOwner()));
			}
		}
		break;
		}
	}
	else
	{
		switch (request.stage)
		{
		case ok::graphics::LayeredRendererStage::BackStage:
		{
			if (request.transparent)
			{
				back_stage_transparent.push_back(std::tuple<int, float, ok::graphics::LayeredRenderable*>(request.layer, _GetDistanceToCamera(request.world_space_position), request.GetOwner()));
			}
			else
			{
				back_stage_opaque.push_back(std::pair<int, ok::graphics::LayeredRenderable*>(request.layer, request.GetOwner()));
			}
		}
		break;
		case ok::graphics::LayeredRendererStage::Stage:
		{
			if (request.transparent)
			{
				stage_transparent.push_back(std::tuple<int, float, ok::graphics::LayeredRenderable*>(request.layer, _GetDistanceToCamera(request.world_space_position), request.GetOwner()));
			}
			else
			{
				stage_opaque.push_back(std::pair<int, ok::graphics::LayeredRenderable*>(request.layer, request.GetOwner()));
			}
		}
		break;
		case ok::graphics::LayeredRendererStage::FrontStage:
		{
			if (request.transparent)
			{
				front_stage_transparent.push_back(std::tuple<int, float, ok::graphics::LayeredRenderable*>(request.layer, _GetDistanceToCamera(request.world_space_position), request.GetOwner()));
			}
			else
			{
				front_stage_opaque.push_back(std::pair<int, ok::graphics::LayeredRenderable*>(request.layer, request.GetOwner()));
			}
		}
		break;
		}
	}
}

void ok::graphics::LayeredRenderer::BeginImmediateRender()
{
	_immediate_mode_enabled = true;
}

void ok::graphics::LayeredRenderer::EndImmediateRender()
{
	Flush();
	_immediate_mode_enabled = false;
}

float ok::graphics::LayeredRenderer::_GetDistanceToCamera(glm::vec3 world_space_position)
{
	ok::graphics::Camera* camera = ok::graphics::Camera::GetCurrent();
	camera->BeginTransform(ok::TransformSpace::WorldSpace);
	glm::vec3 camera_world_space_position = camera->GetPosition();
	camera->EndTransform(false);

	return static_cast<float>((world_space_position - camera_world_space_position).length());
}

ok::graphics::LayeredRenderable * ok::graphics::LayeredRendererRequest::GetOwner()
{
	return owner;
}
