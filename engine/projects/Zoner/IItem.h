#pragma once

#include "..\..\String.h"
#include "..\..\GameObject.h"
#include "..\..\SpriteBatch.h"
#include "UID.h"
#include "IGame.h"

namespace Zoner
{
	class ItemBlueprint : public ok::GameObject
	{
	public:
	private:
	protected:
	};

	class IItemBehaviour : public ok::Behaviour, public Zoner::ISaveable
	{
	public:
		virtual void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;
		virtual void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;
	private:
	protected:
	};

	class IItem : public ok::GameObject, public Zoner::UID, public Zoner::ISaveable
	{
	public:
		bool is_countable = false;
		int count = 1;

		virtual void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;
		virtual void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) = 0;
	private:
	protected:
	};

	class IItemBuilder
	{
	public:
		Zoner::IItemBuilder& o();

		virtual Zoner::IItem* BuildFromRecipe(ok::String& recipe) = 0;
	protected:
		Zoner::IItemBuilder* _instance = nullptr;
	};
}