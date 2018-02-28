#pragma once

#include "Ship.h"
#include "..\..\Assets.h"

namespace Starpom
{
	class LoadingTask
	{
	public:
		int steps_left;
		int steps_total;

		virtual void Begin();
		virtual void Step();
		virtual void End();
	protected:
	private:
	};

	class Task_SS_Blueprints : public Starpom::LoadingTask
	{
	public:
		void Begin();
		void Step();
		void End();
	protected:
	private:
		std::vector<std::unique_ptr<std::string>> _names;
		std::vector<std::unique_ptr<std::string>> _files;
	};

	//��������� ����� ����� ���-�� ��������� � ������������� program flow ���� �� �������� ��������.
	class Loading
	{
	public:
		Loading(Starpom::LoadingTask* task);
		~Loading();

		//Draws GUI, Process Tasks
		bool Update(float dt);

		static Loading* _instance;
	protected:
	private:
		Starpom::LoadingTask* _task;
	};
}