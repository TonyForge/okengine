#include "Commands.h"

std::vector<Zoner::CommandsList*> Zoner::CommandsList::all_lists;
size_t Zoner::CommandsList::alive_lists = 0;

std::vector<std::pair<int, Zoner::ICommand*>> Zoner::CommandsList::_commands_cache;

Zoner::CommandsList::CommandsList()
{
	all_lists.push_back(this);
	this_in_all_lists_index = all_lists.size() - 1;

	alive_lists++;

	_commands_realtime_60_shift = 0;
	_commands_realtime_30_shift = 0;

	_commands_daily_240_shift = 0;
	_commands_daily_96_shift = 0;
	_commands_daily_48_shift = 0;
	_commands_daily_4_shift = 0;
	_commands_daily_1_shift = 0;

	_commands_realtime_60_timer = 0.f;
	_commands_realtime_30_timer = 0.f;

	_commands_daily_240_timer = 0.f;
	_commands_daily_96_timer = 0.f;
	_commands_daily_48_timer = 0.f;
	_commands_daily_4_timer = 0.f;
	_commands_daily_1_timer = 0.f;

	_commands_daily_240_dirty = false;
	_commands_daily_96_dirty = false;
	_commands_daily_48_dirty = false;
	_commands_daily_4_dirty = false;
	_commands_daily_1_dirty = false;

	__commands_shift = nullptr;
	__commands = nullptr;

	restriction_a = true;
	restriction_b = true;

	sleep = true;
	sequence = false;
	parallel = false;
}

Zoner::CommandsList::~CommandsList()
{
	all_lists[this_in_all_lists_index] = nullptr;
	alive_lists--;
}

void Zoner::CommandsList::Push(Zoner::ICommand * cmd, int group_id, Zoner::CommandExecutionStrategy execution_strategy)
{
	_InstallExecutionStrategy(execution_strategy);
	__commands->push_back(std::make_pair(group_id, cmd));
	cmd->OnEnterList();
}

void Zoner::CommandsList::Remove(int group_id, Zoner::CommandExecutionStrategy execution_strategy)
{
	_InstallExecutionStrategy(execution_strategy);
	std::vector<std::pair<int, Zoner::ICommand*>>& _commands = *__commands;
	size_t& _commands_shift = *__commands_shift;

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

void Zoner::CommandsList::Replace(Zoner::ICommand * cmd, int group_id, Zoner::CommandExecutionStrategy execution_strategy)
{
	_InstallExecutionStrategy(execution_strategy);
	std::vector<std::pair<int, Zoner::ICommand*>>& _commands = *__commands;
	size_t& _commands_shift = *__commands_shift;


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
	Push(cmd, group_id, execution_strategy);
}

void Zoner::CommandsList::ExecuteAll(Zoner::CommandExecutionStrategy execution_strategy)
{
	_InstallExecutionStrategy(execution_strategy);
	std::vector<std::pair<int, Zoner::ICommand*>>& _commands = *__commands;
	size_t& _commands_shift = *__commands_shift;


	size_t i = 0;

	for (auto&& command : _commands)
	{
		if (i >= _commands_shift)
		{
			command.second->Execute();
		}

		i++;
	}
}

void Zoner::CommandsList::PopExecute(Zoner::CommandExecutionStrategy execution_strategy)
{
	_InstallExecutionStrategy(execution_strategy);
	std::vector<std::pair<int, Zoner::ICommand*>>& _commands = *__commands;
	size_t& _commands_shift = *__commands_shift;


	if (_commands.size() > _commands_shift)
	{
		Zoner::ICommand*& cmd = _commands[_commands_shift].second;

		if (cmd->Execute())
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

void Zoner::CommandsList::Pop(Zoner::CommandExecutionStrategy execution_strategy)
{
	_InstallExecutionStrategy(execution_strategy);
	std::vector<std::pair<int, Zoner::ICommand*>>& _commands = *__commands;
	size_t& _commands_shift = *__commands_shift;


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

void Zoner::CommandsList::Clear(Zoner::CommandExecutionStrategy execution_strategy)
{
	_InstallExecutionStrategy(execution_strategy);
	std::vector<std::pair<int, Zoner::ICommand*>>& _commands = *__commands;
	size_t& _commands_shift = *__commands_shift;


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
	_commands_daily_240_dirty = false;
	_commands_daily_96_dirty = false;
	_commands_daily_48_dirty = false;
	_commands_daily_4_dirty = false;
	_commands_daily_1_dirty = false;

	_commands_daily_240_timer += hours_passed;
	_commands_daily_96_timer += hours_passed;
	_commands_daily_48_timer += hours_passed;
	_commands_daily_4_timer += hours_passed;
	_commands_daily_1_timer += hours_passed;

	if (_commands_daily_240_timer >= 0.1f)
	{
		for (auto&& cmd : _commands_daily_240)
		{
			cmd.second->PassTime(hours_passed);
		}

		_commands_daily_240_dirty = true;
		_commands_daily_240_timer = 0.f;
	}

	if (_commands_daily_96_timer >= 0.25f)
	{
		if (restriction_a == true)
		{
			for (auto&& cmd : _commands_daily_96)
			{
				cmd.second->PassTime(hours_passed);
			}
		}

		_commands_daily_96_dirty = true;
		_commands_daily_96_timer = 0.f;
	}

	if (_commands_daily_48_timer >= 0.5f)
	{
		if (restriction_a == false)
		{
			for (auto&& cmd : _commands_daily_96)
			{
				cmd.second->PassTime(hours_passed);
			}

			_commands_daily_96_dirty = true;
		}

		for (auto&& cmd : _commands_daily_48)
		{
			cmd.second->PassTime(hours_passed);
		}

		_commands_daily_48_dirty = true;
		_commands_daily_48_timer = 0.f;
	}

	if (_commands_daily_4_timer >= 6.0f)
	{
		for (auto&& cmd : _commands_daily_4)
		{
			cmd.second->PassTime(hours_passed);
		}

		_commands_daily_4_dirty = true;
		_commands_daily_4_timer = 0.f;
	}

	if (_commands_daily_1_timer >= 24.0f)
	{
		for (auto&& cmd : _commands_daily_1)
		{
			cmd.second->PassTime(hours_passed);
		}

		_commands_daily_1_dirty = true;
		_commands_daily_1_timer = 0.f;
	}
}

void Zoner::CommandsList::ApplyPassedTime()
{
	if (_commands_daily_240_dirty)
	{
		for (auto&& cmd : _commands_daily_240)
		{
			cmd.second->ApplyPassedTime();
		}

		_commands_daily_240_dirty = false;
	}

	if (_commands_daily_96_dirty)
	{
		for (auto&& cmd : _commands_daily_96)
		{
			cmd.second->ApplyPassedTime();
		}

		_commands_daily_96_dirty = false;
	}

	if (_commands_daily_48_dirty)
	{
		for (auto&& cmd : _commands_daily_48)
		{
			cmd.second->ApplyPassedTime();
		}

		_commands_daily_48_dirty = false;
	}

	if (_commands_daily_4_dirty)
	{
		for (auto&& cmd : _commands_daily_4)
		{
			cmd.second->ApplyPassedTime();
		}

		_commands_daily_4_dirty = false;
	}

	if (_commands_daily_1_dirty)
	{
		for (auto&& cmd : _commands_daily_1)
		{
			cmd.second->ApplyPassedTime();
		}

		_commands_daily_1_dirty = false;
	}
}

void Zoner::CommandsList::OnNewDay()
{
	for (auto&& cmd : _commands_daily_240)
	{
		cmd.second->OnNewDay();
	}

	for (auto&& cmd : _commands_daily_96)
	{
		cmd.second->OnNewDay();
	}

	for (auto&& cmd : _commands_daily_48)
	{
		cmd.second->OnNewDay();
	}

	for (auto&& cmd : _commands_daily_4)
	{
		cmd.second->OnNewDay();
	}

	for (auto&& cmd : _commands_daily_1)
	{
		cmd.second->OnNewDay();
	}
}

void Zoner::CommandsList::Update(float dt)
{
	if (restriction_a == true)
	{
		_commands_realtime_60_timer += dt;
		_commands_realtime_30_timer += dt;

		if (_commands_realtime_60_timer >= 0.01666667f)
		{
			if (restriction_b == true)
			{

				for (auto&& cmd : _commands_realtime_60)
				{
					cmd.second->Update(dt);
				}
			}

			_commands_realtime_60_timer = 0.f;
		}

		if (_commands_realtime_30_timer >= 0.03333333f)
		{
			if (restriction_b == false)
			{
				for (auto&& cmd : _commands_realtime_60)
				{
					cmd.second->Update(dt);
				}
			}

			for (auto&& cmd : _commands_realtime_30)
			{
				cmd.second->Update(dt);
			}

			_commands_realtime_30_timer = 0.f;
		}
	}
}

void Zoner::CommandsList::AllListsRepackIfNeeded()
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
	}
}

void Zoner::CommandsList::AllListsUpdate(float dt)
{
	AllListsRepackIfNeeded();

	if (all_lists.size() > 0)
	{
		for (auto&& cmd_list : all_lists)
		{
			if (cmd_list != nullptr && cmd_list->sleep == false)
			{
				cmd_list->Update(dt);
			}
		}
	}
}

void Zoner::CommandsList::AllListsOnNewDay()
{
	AllListsRepackIfNeeded();

	if (all_lists.size() > 0)
	{
		for (auto&& cmd_list : all_lists)
		{
			if (cmd_list != nullptr && cmd_list->sleep == false)
			{
				cmd_list->OnNewDay();
			}
		}
	}
}

void Zoner::CommandsList::AllListsPassTime(float hours_passed)
{
	AllListsRepackIfNeeded();

	if (all_lists.size() > 0)
	{
		for (auto&& cmd_list : all_lists)
		{
			if (cmd_list != nullptr && cmd_list->sleep == false)
			{
				cmd_list->PassTime(hours_passed);
			}
		}
	}
}

void Zoner::CommandsList::AllListsApplyPassedTime()
{
	AllListsRepackIfNeeded();

	if (all_lists.size() > 0)
	{
		for (auto&& cmd_list : all_lists)
		{
			if (cmd_list != nullptr && cmd_list->sleep == false)
			{
				cmd_list->ApplyPassedTime();
			}
		}
	}
}

void Zoner::CommandsList::AllListsExecute()
{
	AllListsRepackIfNeeded();

	if (all_lists.size() > 0)
	{
		for (auto&& cmd_list : all_lists)
		{
			if (cmd_list != nullptr && cmd_list->sleep == false)
			{
				if (cmd_list->sequence)
				{
					cmd_list->PopExecute(Zoner::CommandExecutionStrategy::realtime_60);
					cmd_list->PopExecute(Zoner::CommandExecutionStrategy::realtime_30);
					cmd_list->PopExecute(Zoner::CommandExecutionStrategy::daily_240);
					cmd_list->PopExecute(Zoner::CommandExecutionStrategy::daily_96);
					cmd_list->PopExecute(Zoner::CommandExecutionStrategy::daily_48);
					cmd_list->PopExecute(Zoner::CommandExecutionStrategy::daily_4);
					cmd_list->PopExecute(Zoner::CommandExecutionStrategy::daily_1);
				}
				
				if (cmd_list->parallel)
				{
					cmd_list->ExecuteAll(Zoner::CommandExecutionStrategy::realtime_60);
					cmd_list->ExecuteAll(Zoner::CommandExecutionStrategy::realtime_30);
					cmd_list->ExecuteAll(Zoner::CommandExecutionStrategy::daily_240);
					cmd_list->ExecuteAll(Zoner::CommandExecutionStrategy::daily_96);
					cmd_list->ExecuteAll(Zoner::CommandExecutionStrategy::daily_48);
					cmd_list->ExecuteAll(Zoner::CommandExecutionStrategy::daily_4);
					cmd_list->ExecuteAll(Zoner::CommandExecutionStrategy::daily_1);
				}
			}
		}
	}
}

void Zoner::CommandsList::_InstallExecutionStrategy(Zoner::CommandExecutionStrategy execution_strategy)
{
	switch (execution_strategy)
	{
	case Zoner::CommandExecutionStrategy::realtime_60 :
		__commands = &_commands_realtime_60;
		__commands_shift = &_commands_realtime_60_shift;
		break;
	case Zoner::CommandExecutionStrategy::realtime_30:
		__commands = &_commands_realtime_30;
		__commands_shift = &_commands_realtime_30_shift;
		break;
	case Zoner::CommandExecutionStrategy::daily_240:
		__commands = &_commands_daily_240;
		__commands_shift = &_commands_daily_240_shift;
		break;
	case Zoner::CommandExecutionStrategy::daily_96:
		__commands = &_commands_daily_96;
		__commands_shift = &_commands_daily_96_shift;
		break;
	case Zoner::CommandExecutionStrategy::daily_48:
		__commands = &_commands_daily_48;
		__commands_shift = &_commands_daily_48_shift;
		break;
	case Zoner::CommandExecutionStrategy::daily_4:
		__commands = &_commands_daily_4;
		__commands_shift = &_commands_daily_4_shift;
		break;
	case Zoner::CommandExecutionStrategy::daily_1:
		__commands = &_commands_daily_1;
		__commands_shift = &_commands_daily_1_shift;
		break;
	}
}

Zoner::ICommand::ICommand() : poolable(false), deletable(false)
{
}
