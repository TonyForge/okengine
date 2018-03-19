#pragma once

namespace Zoner
{
	class ShipBlueprint;

	enum class GameStates
	{
		PreloaderVisible = 0,
		DefaultResourcesLoaded,
		PauseEnabled,
		PauseRequest,
		Count
	};

	class IGame
	{
	public:
		virtual int GetScreenWidth() = 0;
		virtual int GetScreenHeight() = 0;

		virtual std::unordered_map<std::string, Zoner::ShipBlueprint*>& GetShipBlueprints() = 0;

		virtual bool StateTrue(Zoner::GameStates state) = 0;
		virtual bool StateFalse(Zoner::GameStates state) = 0;
		virtual void State(Zoner::GameStates state, bool value) = 0;

		static Zoner::IGame& o();
	protected:
		static Zoner::IGame* _instance;
	private:
		
	};
}