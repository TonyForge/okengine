#pragma once

namespace Zoner
{
	class ShipBlueprint;

	enum class GameScreen
	{
		Space
	};

	enum class GameStates
	{
		PreloaderVisible = 0,
		DefaultResourcesLoaded,
		PauseEnabled,
		PauseRequest,
		LoadGame,
		LoadGameCompleted,
		SaveGame,
		SaveGameCompleted,
		AtStartup,
		Count
	};

	class IShip;
	class ISpace;

	class IGame
	{
	public:
		virtual int GetScreenWidth() = 0;
		virtual int GetScreenHeight() = 0;

		virtual std::unordered_map<std::string, Zoner::ShipBlueprint*>& GetShipBlueprints() = 0;

		virtual bool StateTrue(Zoner::GameStates state) = 0;
		virtual bool StateFalse(Zoner::GameStates state) = 0;
		virtual void State(Zoner::GameStates state, bool value) = 0;

		virtual Zoner::IShip* GetCurrentPlayerShip() = 0;
		virtual void ChangeCurrentLocation(Zoner::ISpace* location) = 0;

		static Zoner::IGame& o();
	protected:
		static Zoner::IGame* _instance;
	private:
		
	};
}