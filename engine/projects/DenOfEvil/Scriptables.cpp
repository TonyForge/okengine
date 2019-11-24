#include "Scriptables.h"

DOE::ScriptNode::ScriptNode() : ok::Behaviour()
{
	events.reserve(6);
	events.push_back(&OnExplore);
	events.push_back(&OnShow);
	events.push_back(&OnHide);
	events.push_back(&OnTeamStepIn);
	events.push_back(&OnTeamStepOut);
	events.push_back(&OnMapInit);
}

void DOE::ScriptNode::Update(float dt)
{

}

void DOE::ScriptedEvent::LoadFromText(ok::String & text)
{
	auto lines = text.split("\\n");
	int line_number = 0;

	script_lines.reserve(lines.size());
	DOE::ScriptLine script_line_data;

	for (auto& line : lines)
	{
		std::vector<ok::String>& words = line.split_whitespace();

		std::memset(script_line_data.arguments, 0, sizeof(script_line_data.arguments));

		if (words[0][0] == ':')
		{
			int label_id = std::atoi(words[0].substring(1, words[0].getSize() - 1).toAnsiString().c_str());
			script_labels.resize(label_id);
			script_labels[label_id - 1] = line_number;
			script_line_data.cmd_id = DOE::ScriptCommandId::SCID_LABEL;
		}
		else
		{
			/*
			SCID_ARGS,
			SCID_GOTO,
			SCID_KILL,
			SCID_OPEN,
			SCID_CLOSE,
			SCID_SET_TILE,
			SCID_TOUCH,
			SCID_ENABLE_SCRIPT,
			SCID_DISABLE_SCRIPT,
			SCID_SPAWN
			*/
			if (words[0] == "args")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_ARGS;
				if (words.size() > 1)
				arguments[0] = std::atoi(words[1].toAnsiString().c_str());
				if (words.size() > 2)
				arguments[1] = std::atoi(words[2].toAnsiString().c_str());
				if (words.size() > 3)
				arguments[2] = std::atoi(words[3].toAnsiString().c_str());
				if (words.size() > 4)
				arguments[3] = std::atoi(words[4].toAnsiString().c_str());
				if (words.size() > 5)
				arguments[4] = std::atoi(words[5].toAnsiString().c_str());
			}
			else
			if (words[0] == "goto")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_GOTO;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
			}
			else
			if (words[0] == "kill")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_KILL;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
			}
			else
			if (words[0] == "open")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_OPEN;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
				script_line_data.arguments[1] = std::atoi(words[2].toAnsiString().c_str());
			}
			else
			if (words[0] == "close")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_CLOSE;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
				script_line_data.arguments[1] = std::atoi(words[2].toAnsiString().c_str());
			}
			else
			if (words[0] == "settile")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_SET_TILE;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
				script_line_data.arguments[1] = std::atoi(words[2].toAnsiString().c_str());
				script_line_data.arguments[2] = std::atoi(words[3].toAnsiString().c_str());
			}
			else
			if (words[0] == "touch")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_TOUCH;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
				script_line_data.arguments[1] = std::atoi(words[2].toAnsiString().c_str());
			}
			else
			if (words[0] == "enablescript")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_ENABLE_SCRIPT;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
			}
			else
			if (words[0] == "disablescript")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_DISABLE_SCRIPT;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
			}
			else
			if (words[0] == "spawn")
			{
				script_line_data.cmd_id = DOE::ScriptCommandId::SCID_SPAWN;
				script_line_data.arguments[0] = std::atoi(words[1].toAnsiString().c_str());
				script_line_data.arguments[1] = std::atoi(words[2].toAnsiString().c_str());
				script_line_data.arguments[2] = std::atoi(words[3].toAnsiString().c_str());
			}
		}

		script_lines[line_number] = script_line_data;

		line_number++;
	}
}

int DOE::ToInt(const DOE::ScriptCommandId & scriptCommandId)
{
	int result = 0;
	switch (scriptCommandId)
	{
	case DOE::ScriptCommandId::SCID_ARGS :
		result = 1;
		break;
	case DOE::ScriptCommandId::SCID_CLOSE:
		result = 2;
		break;
	case DOE::ScriptCommandId::SCID_DISABLE_SCRIPT:
		result = 3;
		break;
	case DOE::ScriptCommandId::SCID_ENABLE_SCRIPT:
		result = 4;
		break;
	case DOE::ScriptCommandId::SCID_GOTO:
		result = 5;
		break;
	case DOE::ScriptCommandId::SCID_KILL:
		result = 6;
		break;
	case DOE::ScriptCommandId::SCID_LABEL:
		result = 7;
		break;
	case DOE::ScriptCommandId::SCID_OPEN:
		result = 8;
		break;
	case DOE::ScriptCommandId::SCID_SET_TILE:
		result = 9;
		break;
	case DOE::ScriptCommandId::SCID_SPAWN:
		result = 10;
		break;
	case DOE::ScriptCommandId::SCID_TOUCH:
		result = 11;
		break;
	}
	return result;
}

DOE::ScriptCommandId DOE::FromInt(int value)
{
	DOE::ScriptCommandId result;

	switch (value)
	{
	case 1 :
		result = DOE::ScriptCommandId::SCID_ARGS;
		break;
	case 2 :
		result = DOE::ScriptCommandId::SCID_CLOSE;
		break;
	case 3 :
		result = DOE::ScriptCommandId::SCID_DISABLE_SCRIPT;
		break;
	case 4 :
		result = DOE::ScriptCommandId::SCID_ENABLE_SCRIPT;
		break;
	case 5 :
		result = DOE::ScriptCommandId::SCID_GOTO;
		break;
	case 6 :
		result = DOE::ScriptCommandId::SCID_KILL;
		break;
	case 7 :
		result = DOE::ScriptCommandId::SCID_LABEL;
		break;
	case 8 :
		result = DOE::ScriptCommandId::SCID_OPEN;
		break;
	case 9 :
		result = DOE::ScriptCommandId::SCID_SET_TILE;
		break;
	case 10:
		result = DOE::ScriptCommandId::SCID_SPAWN;
		break;
	case 11 :
		result = DOE::ScriptCommandId::SCID_TOUCH;
		break;
	}
	return result;
}
