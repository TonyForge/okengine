#pragma once

#include "..\..\SpriteBatch.h"
#include "..\..\AssetsBasic.h"
#include "IGame.h"
#include "Object.h"
#include "..\..\Input.h"
#include "..\..\Random.h"

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

	//Divides to several SlimeTiny when die
	class Slime : public Kripta::Object
	{
	public:
		Slime();
		virtual ~Slime();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};

	//Simple and 2x weaker than Slime
	class SlimeTiny : public Kripta::Object
	{
	public:
		SlimeTiny();
		virtual ~SlimeTiny();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};

	//Double health
	class Skeleton : public Kripta::Object
	{
	public:
		Skeleton();
		virtual ~Skeleton();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
		virtual void SetLevel(int level);
	};

	//Turns to bat when low health
	class Vampire : public Kripta::Object
	{
	public:
		Vampire();
		virtual ~Vampire();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
		bool bat_activated = false;
	};

	//2x weaker in attack than Vampire, but with 25% chance avoid strikes when hit and 50% chance to convert damage to health at attack
	class Bat : public Kripta::Object
	{
	public:
		Bat();
		virtual ~Bat();
		virtual void Update(float dt);
		virtual void YouKick(int target_level);
		ok::graphics::SpriteInfo spr;
		bool vampire_activated = false;
		virtual void Kick(int attack_level);
	};

	//Drops web (hero cant move for some turns while web alive).
	class Spider : public Kripta::Object
	{
	public:
		Spider();
		virtual ~Spider();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};

	//walks through walls
	class Ghost : public Kripta::Object
	{
	public:
		Ghost();
		virtual ~Ghost();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};

	class Goblin : public Kripta::Object
	{
	public:
		Goblin();
		virtual ~Goblin();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};

	class Tomb : public Kripta::Object 
	{
	public:
		Tomb();
		virtual ~Tomb();
		virtual void PreUpdate(float dt);
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
		virtual ~GoldPile();
		virtual void PreUpdate(float dt);
		virtual void PostUpdate(float dt);
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};

	class HealthPotion : public Kripta::Object
	{
	public:
		HealthPotion();
		virtual ~HealthPotion();
		virtual void PreUpdate(float dt);
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};

	class MysteriousPotion : public Kripta::Object
	{
	public:
		MysteriousPotion();
		virtual ~MysteriousPotion();
		virtual void PreUpdate(float dt);

		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
	};

	class GoldenGuard : public Kripta::Object
	{
	public:
		GoldenGuard();
		virtual ~GoldenGuard();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo spr;
		glm::vec2 home_grid_xy;
	};

	class Stair : public Kripta::Object
	{
	public:
		Stair();
		virtual ~Stair();
		virtual void PostUpdate(float dt);
		virtual void Update(float dt);
		ok::String room_name;
	};

	class Switch : public Kripta::Object
	{
	public:
		Switch();
		virtual ~Switch();
		virtual void PreUpdate(float dt);
		virtual void PostUpdate(float dt);
		virtual void Update(float dt);
		virtual void Touch(Kripta::Object* owner);
		int act_x;
		int act_y;
		int act_id;
		bool once;
		ok::graphics::SpriteInfo switch_down_spr;
		ok::graphics::SpriteInfo switch_up_spr;
		int state;// 0 - down, 1 - up
		int activated_at_turn = 0;
	};

	class Torch : public Kripta::Object
	{
	public:
		Torch();
		virtual ~Torch();
		virtual void Update(float dt);
		ok::graphics::SpriteInfo frame0;
		ok::graphics::SpriteInfo frame1;
		ok::graphics::SpriteInfo frame2;
		float animation_time = 0;
	};
}