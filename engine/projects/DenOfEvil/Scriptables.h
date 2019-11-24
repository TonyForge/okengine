#pragma once

#include "..\\..\\Behaviour.h"

namespace DOE
{

	enum class ScriptCommandId
	{
		SCID_ARGS,
		SCID_LABEL,
		SCID_GOTO,
		SCID_KILL,
		SCID_OPEN,
		SCID_CLOSE,
		SCID_SET_TILE,
		SCID_TOUCH,
		SCID_ENABLE_SCRIPT,
		SCID_DISABLE_SCRIPT,
		SCID_SPAWN
	};

	int ToInt(const DOE::ScriptCommandId& scriptCommandId);
	DOE::ScriptCommandId FromInt(int value);

	class ScriptLine
	{
	public:
		DOE::ScriptCommandId cmd_id;
		int arguments[5];
	};

	class ScriptedEvent
	{
	public:
		std::vector<DOE::ScriptLine> script_lines;
		std::vector<int> script_labels;
		int arguments[5];
		void LoadFromText(ok::String& text);
	};

	class ScriptNode : public ok::Behaviour
	{
	public:
		ScriptNode();
		virtual void Update(float dt);
		DOE::ScriptedEvent OnExplore;
		DOE::ScriptedEvent OnShow;
		DOE::ScriptedEvent OnHide;
		DOE::ScriptedEvent OnTeamStepIn;
		DOE::ScriptedEvent OnTeamStepOut;
		DOE::ScriptedEvent OnMapInit;
		std::vector<DOE::ScriptedEvent*> events;
		bool global = false;
	protected:
	private:
	};
}