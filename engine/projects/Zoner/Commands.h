#pragma once

namespace Zoner
{
	enum class CommandExecutionStrategy
	{
		realtime_60,
		realtime_30,
		daily_240,
		daily_96,
		daily_48,
		daily_4,
		daily_1
	};

	class ICommand
	{
	public:

		ICommand();

		virtual void OnEnterList() {};
		virtual void OnExitList() {};

		virtual bool Execute() { return true; }

		virtual void Update(float dt) {}
		virtual void PassTime(float hours_passed) {}
		virtual void ApplyPassedTime() {}
		virtual void OnNewDay() {}

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
		~CommandsList();

		void Push(Zoner::ICommand* cmd, int group_id, Zoner::CommandExecutionStrategy execution_strategy);
		void Remove(int group_id, Zoner::CommandExecutionStrategy execution_strategy);
		void Replace(Zoner::ICommand* cmd, int group_id, Zoner::CommandExecutionStrategy execution_strategy);
		void ExecuteAll(Zoner::CommandExecutionStrategy execution_strategy);
		void PopExecute(Zoner::CommandExecutionStrategy execution_strategy);
		void Pop(Zoner::CommandExecutionStrategy execution_strategy);
		void Clear(Zoner::CommandExecutionStrategy execution_strategy);

		void Update(float dt);
		void PassTime(float hours_passed);
		void ApplyPassedTime();
		void OnNewDay();

		size_t this_in_all_lists_index;
		bool sleep;
		bool sequence;
		bool parallel;
		bool restriction_a; //meanings: owner is in current_space
		bool restriction_b; //meanings: owner is in viewport

		static void AllListsRepackIfNeeded();
		static void AllListsUpdate(float dt);
		static void AllListsOnNewDay();
		static void AllListsPassTime(float hours_passed);
		static void AllListsApplyPassedTime();
		static void AllListsExecute();

		static std::vector<Zoner::CommandsList*> all_lists;
		static size_t alive_lists;
	protected:
	private:
		size_t* __commands_shift;
		std::vector<std::pair<int, Zoner::ICommand*>>* __commands;

		//realtime commands execute happens only in current space!
		std::vector<std::pair<int, Zoner::ICommand*>> _commands_realtime_60; //for smooth and slow animations in viewport
		std::vector<std::pair<int, Zoner::ICommand*>> _commands_realtime_30; //for rough and fast animations in viewport, and for all animations outside of viewport

		std::vector<std::pair<int, Zoner::ICommand*>> _commands_daily_240; //realtime, 60 per real second, 240 times per 24 game hours which is 4 real seconds (animations, movements)
		std::vector<std::pair<int, Zoner::ICommand*>> _commands_daily_96; //24 times per real second, 96 times per game day (most of ai logic here)
		std::vector<std::pair<int, Zoner::ICommand*>> _commands_daily_48; //12 times per real second, 48 times per game day (ai logic at spaces that is not current)
		std::vector<std::pair<int, Zoner::ICommand*>> _commands_daily_4; //1 per real second, 4 per game day, every 6 game hours (some very rare ai logic)
		std::vector<std::pair<int, Zoner::ICommand*>> _commands_daily_1; //1 per game day, executes at the beginning of the game day

		//call priority ((((240) 96) 48) 4) 1
		//timers
		float _commands_realtime_60_timer;
		float _commands_realtime_30_timer;

		float _commands_daily_240_timer;
		float _commands_daily_96_timer;
		float _commands_daily_48_timer;
		float _commands_daily_4_timer;
		float _commands_daily_1_timer;

		size_t _commands_realtime_60_shift;
		size_t _commands_realtime_30_shift;

		size_t _commands_daily_240_shift;
		size_t _commands_daily_96_shift;
		size_t _commands_daily_48_shift;
		size_t _commands_daily_4_shift;
		size_t _commands_daily_1_shift;

		bool _commands_daily_240_dirty;
		bool _commands_daily_96_dirty;
		bool _commands_daily_48_dirty;
		bool _commands_daily_4_dirty;
		bool _commands_daily_1_dirty;

		void _InstallExecutionStrategy(Zoner::CommandExecutionStrategy execution_strategy);

		static std::vector<std::pair<int,Zoner::ICommand*>> _commands_cache;
	};
}