#pragma once

#include "..\..\GameObject.h"
#include "..\..\Camera.h"
#include "..\..\LineBatch.h"
#include "..\..\Assets.h"
#include "IGame.h"
#include "Ship.h"

namespace Zoner
{
	class Preloader : public ok::GameObject
	{
	public:
		void LoadItself();
		void UnloadItself();
		
		void Task_DefaultResources();
		void Task_ShowProgress();

		void Task_ShowProgress_Update(int _task_step, int _task_step_max);

		void Update(float dt);
	private:
		int task_step = 0;
		int task_step_max = 1;

		ok::graphics::Camera* camera;
		ok::graphics::LineBatch* progress_bar;

		class Task_Object
		{
		public:
			int steps_left;
			int steps_total;

			virtual void Begin() = 0;
			virtual void Step() = 0;
			virtual void End() = 0;
		};

		class Task_DefaultResources_Object : public Zoner::Preloader::Task_Object
		{
		public:
			void Begin();
			void Step();
			void End();
		private:
			ok::GameObject* LoadPart_Spaceship(tinyxml2::XMLElement* part_xml, ok::GameObject* parent = nullptr);
			std::vector<std::unique_ptr<std::string>> _names;
			std::vector<std::unique_ptr<std::string>> _files;
			std::vector<std::unique_ptr<std::string>> _types;

			int _atlases_begin_step;
		};

		Zoner::Preloader::Task_Object* _task = nullptr;
	};
}