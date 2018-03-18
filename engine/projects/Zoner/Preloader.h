#pragma once

#include "..\..\GameObject.h"
#include "..\..\Camera.h"
#include "..\..\LineBatch.h"

namespace Zoner
{
	class Preloader : public ok::GameObject
	{
	public:
		void LoadItself();
		void UnloadItself();
		
		void Task_DefaultResources();

		void Update(float dt);
	private:
		int task;
		int task_step;
		int task_step_max;
		ok::graphics::Camera* camera;
		ok::graphics::LineBatch* progress_bar;

		void _Task_DefaultResources();
	};
}