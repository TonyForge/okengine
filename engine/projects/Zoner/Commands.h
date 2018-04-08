#pragma once

namespace Zoner
{
	class ICommand
	{
	public:

		ICommand();

		virtual void OnEnterList() {};
		virtual void OnExitList() {};
		virtual bool Execute(float dt) { return true; };

		virtual void PassTime(float hours_passed) {};
		virtual void ApplyPassedTime() {};
		virtual void OnNewDay() {};

		virtual void ReturnToPool() {};

		bool poolable;
		bool deletable;
	protected:
	private:
	};

	class CommandsList
	{
	public:
		CommandsList();

		void Push(Zoner::ICommand* cmd, int group_id);
		void Remove(int group_id);
		void Replace(Zoner::ICommand* cmd, int group_id);
		void ExecuteAll(float dt);
		void PopExecute(float dt);
		void Pop();
		void Clear();

		void PassTime(float hours_passed);
		void ApplyPassedTime();
		void OnNewDay();

	protected:
	private:
		size_t _commands_shift;
		std::vector<std::pair<int, Zoner::ICommand*>> _commands;
		static std::vector<std::pair<int,Zoner::ICommand*>> _commands_cache;
	};
}