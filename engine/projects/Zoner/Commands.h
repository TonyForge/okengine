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
		~CommandsList();

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

		void Update(float dt, float hours_passed, bool new_day); //must be called as fast as possible (realtime, 60 fps limit inside)

		size_t this_in_all_lists_index;
		float dt_accumulator;
		float hours_accumulator;
		int realtime_frames_accumulator;
		int gametime_frames_accumulator;

		static void AllListsUpdate(float dt, float hours_passed, bool new_day);
		static std::vector<Zoner::CommandsList*> all_lists;
		static size_t alive_lists;
	protected:
	private:
		size_t _commands_shift;
		std::vector<std::pair<int, Zoner::ICommand*>> _commands;

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

		std::vector<std::pair<int, Zoner::ICommand*>> _commands_fps_;
		static std::vector<std::pair<int,Zoner::ICommand*>> _commands_cache;
	};
}