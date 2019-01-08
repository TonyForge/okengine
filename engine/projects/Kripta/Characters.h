#pragma once

#include "..\..\SpriteBatch.h"
#include "..\..\AssetsBasic.h"
#include "IGame.h"
#include "Object.h"
#include "..\..\Input.h"

namespace Kripta
{
	class Hero : public Kripta::Object
	{
	public:
		Hero();
		virtual ~Hero();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
		int gold = 0;
		int gold_to_levelup = 1000;

		virtual void PickUpObject(Kripta::Object* obj);
	};

	class Goblin : public Kripta::Object
	{
	public:
		Goblin();
		virtual ~Goblin();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
		glm::vec2 last_seen_hero_xy;
	};

	class Tomb : public Kripta::Object 
	{
	public:
		Tomb();
		virtual ~Tomb();
		virtual void PostUpdate(float dt);
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
		int initial_turn = 0;
		int turns_to_respawn = 5;
		Kripta::ObjectID creature_id;
	};

	class GoldPile : public Kripta::Object
	{
	public:
		GoldPile();
		~GoldPile();
		virtual void PostUpdate(float dt);
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};
}