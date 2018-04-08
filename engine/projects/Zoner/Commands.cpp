#include "Commands.h"

std::vector<std::pair<int, Zoner::ICommand*>> Zoner::CommandsList::_commands_cache;

Zoner::CommandsList::CommandsList()
{
	_commands_shift = 0;
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

Zoner::ICommand::ICommand() : poolable(false), deletable(false)
{
}
