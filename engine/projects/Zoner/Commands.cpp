#include "Commands.h"

std::vector<Zoner::CommandsList*> Zoner::CommandsList::all_lists;
size_t alive_lists = 0;

std::vector<std::pair<int, Zoner::ICommand*>> Zoner::CommandsList::_commands_cache;

Zoner::CommandsList::CommandsList()
{
	all_lists.push_back(this);
	this_in_all_lists_index = all_lists.size() - 1;

	alive_lists++;

	_commands_shift = 0;

	_commands_realtime_60_timer = 0.f;
	_commands_realtime_30_timer = 0.f;

	_commands_daily_240_timer = 0.f;
	_commands_daily_96_timer = 0.f;
	_commands_daily_48_timer = 0.f;
	_commands_daily_4_timer = 0.f;
	_commands_daily_1_timer = 0.f;

	dt_accumulator = 0;
	hours_accumulator = 0;

	realtime_frames_accumulator = 0;
	gametime_frames_accumulator = 0;
}

Zoner::CommandsList::~CommandsList()
{
	all_lists[this_in_all_lists_index] = nullptr;
	alive_lists--;
}

void Zoner::CommandsList::Push(Zoner::ICommand * cmd, int group_id)
{
	_commands.push_back(std::make_pair(group_id, cmd));
	cmd->OnEnterList();
}

void Zoner::CommandsList::Remove(int group_id)
{
	_commands_cache.clear();
	_commands_cache.reserve(_commands.size());

	size_t i = 0;

	for (auto&& command : _commands)
	{
		if (i >= _commands_shift)
		{
			if (command.first != group_id)
			{
				_commands_cache.push_back(command);
			}
			else
			{
				Zoner::ICommand*& cmd = command.second;

				cmd->OnExitList();

				if (cmd->deletable)
				{
					if (cmd->poolable) cmd->ReturnToPool();
					else delete cmd;
				}
			}
		}

		i++;
	}

	_commands = _commands_cache;
	_commands_shift = 0;
}

void Zoner::CommandsList::Replace(Zoner::ICommand * cmd, int group_id)
{
	size_t i = 0;
	for (auto& command : _commands)
	{
		if (i >= _commands_shift)
		{
			if (command.first == group_id)
			{
				Zoner::ICommand*& _cmd = command.second;

				cmd->OnExitList();

				if (_cmd->deletable)
				{
					if (_cmd->poolable) _cmd->ReturnToPool();
					else delete _cmd;
				}

				command.second = cmd;
				cmd->OnEnterList();
				return;
			}
		}

		i++;
	}

	//if there is no commands to replace just push new
	Push(cmd, group_id);
}

void Zoner::CommandsList::ExecuteAll(float dt)
{
	size_t i = 0;

	for (auto&& command : _commands)
	{
		if (i >= _commands_shift)
		{
			command.second->Execute(dt);
		}

		i++;
	}
}

void Zoner::CommandsList::PopExecute(float dt)
{
	if (_commands.size() > _commands_shift)
	{
		Zoner::ICommand*& cmd = _commands[_commands_shift].second;

		if (cmd->Execute(dt))
		{
			cmd->OnExitList();

			if (cmd->deletable)
			{
				if (cmd->poolable) cmd->ReturnToPool();
				else delete cmd;
			}

			_commands_shift++;
		}
	}
	

	if (_commands.size() <= _commands_shift)
	{
		_commands.clear();
		_commands_shift = 0;
	}
}

void Zoner::CommandsList::Pop()
{
	if (_commands.size() > _commands_shift)
	{
		Zoner::ICommand*& cmd = _commands[_commands_shift].second;

		cmd->OnExitList();

		if (cmd->deletable)
		{
			if (cmd->poolable) cmd->ReturnToPool();
			else delete cmd;
		}

		_commands_shift++;
	}

	if (_commands.size() <= _commands_shift)
	{
		_commands.clear();
		_commands_shift = 0;
	}
}

void Zoner::CommandsList::Clear()
{
	for (auto& command : _commands)
	{
		Zoner::ICommand*& cmd = command.second;

		cmd->OnExitList();

		if (cmd->deletable)
		{
			if (cmd->poolable) cmd->ReturnToPool();
			else delete cmd;
		}
	}

	_commands.clear();
	_commands_shift = 0;
}

void Zoner::CommandsList::PassTime(float hours_passed)
{
	size_t i = 0;

	for (auto&& command : _commands)
	{
		if (i >= _commands_shift)
		{
			command.second->PassTime(hours_passed);
		}

		i++;
	}
}

void Zoner::CommandsList::ApplyPassedTime()
{
	size_t i = 0;

	for (auto&& command : _commands)
	{
		if (i >= _commands_shift)
		{
			command.second->ApplyPassedTime();
		}

		i++;
	}
}

void Zoner::CommandsList::OnNewDay()
{
	size_t i = 0;

	for (auto&& command : _commands)
	{
		if (i >= _commands_shift)
		{
			command.second->OnNewDay();
		}

		i++;
	}
}

void Zoner::CommandsList::Update(float dt, float hours_passed, bool new_day)
{
	//if (owner is inside current space)
	//{
	_commands_realtime_60_timer += dt;
	_commands_realtime_30_timer += dt;

	if (_commands_realtime_60_timer >= 0.01666667f)
	{
		//if owner is in viewport
		for (auto&& cmd : _commands_realtime_60)
		{
			//cmd.Execute
		}

		_commands_realtime_60_timer = 0.f;
	}

	if (_commands_realtime_30_timer >= 0.03333333f)
	{
		//if (owner is not in viewport)
		/*{
		for (auto&& cmd : _commands_realtime_60)
		{
		//cmd.Execute
		}
		}*/

		for (auto&& cmd : _commands_realtime_30)
		{
			//cmd.Execute
		}

		_commands_realtime_30_timer = 0.f;
	}

	//}

	_commands_daily_240_timer += hours_passed;
	_commands_daily_96_timer += hours_passed;
	_commands_daily_48_timer += hours_passed;
	_commands_daily_4_timer += hours_passed;
	_commands_daily_1_timer += hours_passed;

	if (_commands_daily_240_timer >= 0.1f)
	{
		for (auto&& cmd : _commands_daily_240)
		{
			//cmd.Execute
		}

		_commands_daily_240_timer = 0.f;
	}

	if (_commands_daily_96_timer >= 0.25f)
	{
		//if owner is inside current space
		for (auto&& cmd : _commands_daily_96)
		{
			//cmd.Execute
		}

		_commands_daily_96_timer = 0.f;
	}

	if (_commands_daily_48_timer >= 0.5f)
	{
		//if owner is not inside current space
		/*for (auto&& cmd : _commands_daily_96)
		{
			//cmd.Execute
		}*/

		for (auto&& cmd : _commands_daily_48)
		{
			//cmd.Execute
		}

		_commands_daily_48_timer = 0.f;
	}

	if (_commands_daily_4_timer >= 6.0f)
	{
		for (auto&& cmd : _commands_daily_4)
		{
			//cmd.Execute
		}

		_commands_daily_4_timer = 0.f;
	}

	if (_commands_daily_1_timer >= 24.0f)
	{
		for (auto&& cmd : _commands_daily_1)
		{
			//cmd.Execute
		}

		_commands_daily_1_timer = 0.f;
	}
}

void Zoner::CommandsList::AllListsUpdate(float dt, float hours_passed, bool new_day)
{
	bool repack_needed = false;

	if (alive_lists == 0) all_lists.clear();

	if (all_lists.size() > 0)
	{
		if (static_cast<float>(all_lists.size() - alive_lists) / static_cast<float>(all_lists.size()) > 0.25f) repack_needed = true;

		if (repack_needed)
		{
			std::vector<Zoner::CommandsList*> all_lists_temp;
			all_lists_temp.reserve(alive_lists);

			for (auto&& cmd_list : all_lists)
			{
				if (cmd_list != nullptr)
				{
					all_lists_temp.push_back(cmd_list);
					all_lists_temp[all_lists_temp.size() - 1]->this_in_all_lists_index = all_lists_temp.size() - 1;
				}
			}

			all_lists = all_lists_temp;
		}

		for (auto&& cmd_list : all_lists)
		{
			if (cmd_list != nullptr)
			{
				cmd_list->Update(dt, hours_passed, new_day);
			}
		}
	}
}

Zoner::ICommand::ICommand() : poolable(false), deletable(false)
{
}
