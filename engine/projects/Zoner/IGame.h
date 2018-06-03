#pragma once

#include "..\..\SpriteBatch.h"

namespace Zoner
{
	class ISaveable
	{
	public:
		virtual void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;
		virtual void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;
	private:
	protected:
	};

	class ShipBlueprint;
	class ItemBlueprint;

	enum class GameScreen
	{
		Space
	};

	enum class GameStates
	{
		PreloaderVisible = 0,
		DefaultResourcesLoaded,
		CancelAutoPause,
		PauseEnabled,
		PauseRequest,
		LoadGame,
		LoadGameCompleted,
		SaveGame,
		SaveGameCompleted,
		AtStartup,

		InspectorSwitch,
		InspectorVisible,
		Count
	};

	class IShip;
	class ISpace;

	class IGame : public Zoner::ISaveable
	{
	public:
		virtual int GetScreenWidth() = 0;
		virtual int GetScreenHeight() = 0;

		virtual std::unordered_map<std::string, Zoner::ShipBlueprint*>& GetShipBlueprints() = 0;
		virtual std::unordered_map<std::string, Zoner::ItemBlueprint*>& GetItemBlueprints() = 0;
		virtual std::unordered_map<std::string, ok::graphics::SpriteAtlas*>& GetSpriteAtlases() = 0;

		virtual void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;
		virtual void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;

		virtual bool StateTrue(Zoner::GameStates state) = 0;
		virtual bool StateFalse(Zoner::GameStates state) = 0;
		virtual void State(Zoner::GameStates state, bool value) = 0;
		virtual void StateSwitch(Zoner::GameStates state) = 0;

		virtual Zoner::IShip* GetCurrentPlayerShip() = 0;
		virtual void ChangeCurrentLocation(Zoner::ISpace* location) = 0;

		static Zoner::IGame& o();
	protected:
		static Zoner::IGame* _instance;
	private:
		
	};
}