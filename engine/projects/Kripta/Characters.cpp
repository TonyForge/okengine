#include "Characters.h"

Kripta::Hero::Hero()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("hero_spr"));

	id = Kripta::ObjectID::Hero;
}

Kripta::Hero::~Hero()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::Hero::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (gold >= gold_to_levelup)
	{
		gold_to_levelup += 1000;
		SetLevel(level + 1);
	}

	Kripta::IGame::instance->PushToPostUpdateList(this);

	if (dead)
	{
		return;
	}

	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y)+glm::vec2(16.f,16.f), 0.f, glm::vec2(1.f, 1.f));
		//DrawHealthbar();
	}
	EndTransform(false);

	Kripta::IGame::instance->SetHeroXY(pos.x, pos.y);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 11 && turn_controller->turn_decision_made == false)
	{
		if (Kripta::IGame::instance->main_menu_enabled)
		{
			//do nothing
		}
		else
		{


			bool turn_complete = false;

			if (ok::Input::instance().KeyDown(ok::KKey::Left))
			{
				action_grid_x = grid_x - 1;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}

			if (ok::Input::instance().KeyDown(ok::KKey::Right))
			{
				action_grid_x = grid_x + 1;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}

			if (ok::Input::instance().KeyDown(ok::KKey::Up))
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y - 1;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}

			if (ok::Input::instance().KeyDown(ok::KKey::Down))
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y + 1;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}

			if (turn_complete)
			{
				Kripta::IGame::instance->SetHeroActionXY(static_cast<float>(action_grid_x), static_cast<float>(action_grid_y));
				turn_controller->turn_decision_made = true;
				Kripta::TurnController::turn_members_decision_made++;
			}
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			//SetParent(nullptr);
			//delete this;
		}
	}
}

void Kripta::Hero::PickUpObject(Kripta::Object * obj)
{
	if (obj->id == Kripta::ObjectID::GoldPile)
	{
		gold += obj->level;
	}

	if (obj->id == Kripta::ObjectID::HealthPotion)
	{
		hp += level;
		if (hp > level * 3) hp = level * 3;
	}

	if (obj->id == Kripta::ObjectID::Switch)
	{
		obj->Touch(this);
		return;
	}

	if (obj->id == Kripta::ObjectID::Torch)
	{
		obj->Touch(this);
		return;
	}

	if (obj->id == Kripta::ObjectID::MysteriousPotion)
	{
		
	}

	Kripta::IGame::instance->BlockFloor(obj->grid_x, obj->grid_y, nullptr);
	obj->dead = true;
	Kripta::IGame::instance->PushToDeathList(obj);
}


Kripta::Goblin::Goblin()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("goblin_spr"));
	id = Kripta::ObjectID::Goblin;
}

Kripta::Goblin::~Goblin()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);
		Kripta::IGame::instance->CreateTombForMe(this);
	}
	

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::Goblin::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}
		
	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;

		

		if (turn_complete == false)
		{
			glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();
			glm::vec2 hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

			int i_hero_action_x = static_cast<int>(hero_action_xy.x);
			int i_hero_action_y = static_cast<int>(hero_action_xy.y);

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				if (hero_action_room_pick.door == false)
					last_seen_hero_xy = hero_xy;
				else
					last_seen_hero_xy = glm::vec2(static_cast<float>(grid_x), static_cast<float>(grid_y));
			}

			

			if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) && hero_action_room_pick.door == false)
			{
				float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
				float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
				float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
				float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


				if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y - 1;
					}

				if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y + 1;
					}

				if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
					{
						action_grid_x = grid_x + 1;
						action_grid_y = grid_y;
					}

				if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
					if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
					{
						action_grid_x = grid_x - 1;
						action_grid_y = grid_y;
					}

				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
		}
		
		

		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}
	}
}

Kripta::Tomb::Tomb()
{
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("tomb"));
	initial_turn = Kripta::IGame::instance->turn_number;

	id = Kripta::ObjectID::Tomb;
}

Kripta::Tomb::~Tomb()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockFloorSpecial(grid_x, grid_y, nullptr);
	}
}

void Kripta::Tomb::PreUpdate(float dt)
{
	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
	}
	EndTransform(false);
}

void Kripta::Tomb::PostUpdate(float dt)
{
	BeginTransform();
	glm::vec3 pos = GetPosition() + glm::vec3(16.f, 16.f, 0.f);
	EndTransform(false);

	ok::graphics::SpriteInfo blank = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("blank"));

	blank.tint_color = ok::Color(0.f, 0.f, 0.f, 1.f);
	blank.tint_power = 1.f;
	Kripta::IGame::instance->sprite_batch->Draw(&blank, glm::vec2(pos.x - 12.f, pos.y - 16.f - 6.f), 0.f, glm::vec2((1.f / 32.f) * 24.f, (1.f / 32.f) * 4.f));

	blank.tint_color = ok::Color(0.f, 0.5f, 1.f, 1.f);
	blank.tint_power = 1.f;
	Kripta::IGame::instance->sprite_batch->Draw(&blank, glm::vec2(pos.x - 12.f, pos.y - 16.f - 6.f), 0.f, glm::vec2((1.f / 32.f) * (24.f * (static_cast<float>(Kripta::IGame::instance->turn_number - initial_turn) / (static_cast<float>(turns_to_respawn)))), (1.f / 32.f) * 4.f));
}

void Kripta::Tomb::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (dead)
	{
		return;
	}

	if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
	{
		Kripta::IGame::instance->PushToPreUpdateList(this);
		Kripta::IGame::instance->PushToPostUpdateList(this);
	}
	
	if (Kripta::IGame::instance->TurnStage() == 44)
	{
		bool pause_progress = false;

		auto room_pick = Kripta::IGame::instance->PickRoom(grid_x, grid_y);

		if (room_pick.place_obj != nullptr)
		{
			pause_progress = true;
			initial_turn++;
		}
		
		if (pause_progress == false && (Kripta::IGame::instance->turn_number - initial_turn) >= turns_to_respawn)
		{
			if (creature_id == Kripta::ObjectID::Goblin)
			{
				auto character = new Kripta::Goblin();
				character->Place(grid_x, grid_y);
				GetParent()->AddChild(character);
				character->SetLevel(level);
			}

			if (creature_id == Kripta::ObjectID::Skeleton)
			{
				auto character = new Kripta::Skeleton();
				character->Place(grid_x, grid_y);
				GetParent()->AddChild(character);
				character->SetLevel(level);
			}

			if (creature_id == Kripta::ObjectID::Slime)
			{
				auto character = new Kripta::Slime();
				character->Place(grid_x, grid_y);
				GetParent()->AddChild(character);
				character->SetLevel(level);
			}

			if (creature_id == Kripta::ObjectID::Vampire)
			{
				auto character = new Kripta::Vampire();
				character->Place(grid_x, grid_y);
				GetParent()->AddChild(character);
				character->SetLevel(level);
			}

			if (creature_id == Kripta::ObjectID::Bat)
			{
				auto character = new Kripta::Bat();
				character->Place(grid_x, grid_y);
				GetParent()->AddChild(character);
				character->SetLevel(level);
				character->vampire_activated = (hp == 1 ? true : false);
			}

			if (creature_id == Kripta::ObjectID::GoldenGuard)
			{
				auto character = new Kripta::GoldenGuard();
				character->Place(grid_x, grid_y);
				GetParent()->AddChild(character);
				character->home_grid_xy.x = static_cast<float>(grid_x);
				character->home_grid_xy.y = static_cast<float>(grid_y);
				character->SetLevel(level);
			}

			Kripta::IGame::instance->PushToDeathList(this);
		}
	}
}

Kripta::GoldPile::GoldPile()
{
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("gold_pile"));
	
	id = Kripta::ObjectID::GoldPile;
}

Kripta::GoldPile::~GoldPile()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
	}
}

void Kripta::GoldPile::PreUpdate(float dt)
{
	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
	}
	EndTransform(false);
}

void Kripta::GoldPile::PostUpdate(float dt)
{
}

void Kripta::GoldPile::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (dead)
	{
		return;
	}

	if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
	{
		Kripta::IGame::instance->PushToPreUpdateList(this);
	}
}

Kripta::HealthPotion::HealthPotion()
{
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("health_potion"));

	id = Kripta::ObjectID::HealthPotion;
}

Kripta::HealthPotion::~HealthPotion()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
	}
}

void Kripta::HealthPotion::PreUpdate(float dt)
{
	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
	}
	EndTransform(false);
}

void Kripta::HealthPotion::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (dead)
	{
		return;
	}

	if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
	{
		Kripta::IGame::instance->PushToPreUpdateList(this);
	}
}

Kripta::GoldenGuard::GoldenGuard()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("golden_guard"));
	id = Kripta::ObjectID::GoldenGuard;
}

Kripta::GoldenGuard::~GoldenGuard()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);
		Kripta::IGame::instance->CreateTombForMe(this);
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::GoldenGuard::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}

	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;

		glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			last_seen_hero_xy = hero_xy;
		}

		auto hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

		if (static_cast<int>(hero_action_xy.x) == grid_x &&
			static_cast<int>(hero_action_xy.y) == grid_y)
		{
			action_grid_x = static_cast<int>(hero_xy.x);
			action_grid_y = static_cast<int>(hero_xy.y);

			action_id = Kripta::ObjActionID::Act;
			turn_complete = true;
		}
		else
		{
			action_grid_x = grid_x;
			action_grid_y = grid_y;

			action_id = Kripta::ObjActionID::Act;
			turn_complete = true;
		}

		

		/*if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) &&
			(glm::abs(last_seen_hero_xy.x - grid_x) < 1.5f) &&
			(glm::abs(last_seen_hero_xy.y - grid_y) < 1.5f) &&
			(glm::abs(home_grid_xy.x - grid_x) < 0.5f) && 
			(glm::abs(home_grid_xy.y - grid_y) < 0.5f))
		{
			float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
			float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
			float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
			float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


			if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
				if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
				{
					action_grid_x = grid_x;
					action_grid_y = grid_y - 1;
				}

			if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
				if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
				{
					action_grid_x = grid_x;
					action_grid_y = grid_y + 1;
				}

			if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
				if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
				{
					action_grid_x = grid_x + 1;
					action_grid_y = grid_y;
				}

			if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
				if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
				{
					action_grid_x = grid_x - 1;
					action_grid_y = grid_y;
				}

			action_id = Kripta::ObjActionID::Act;
			turn_complete = true;
		}
		else
		{
			if ((glm::abs(home_grid_xy.x - grid_x) > 0.5f) ||
				(glm::abs(home_grid_xy.y - grid_y) > 0.5f))
			{
				action_grid_x = static_cast<int>(home_grid_xy.x);
				action_grid_y = static_cast<int>(home_grid_xy.y);
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
			}

			action_id = Kripta::ObjActionID::Act;
			turn_complete = true;
		}*/


		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}
	}
}

Kripta::Stair::Stair()
{
	id = Kripta::ObjectID::Stair;
}

Kripta::Stair::~Stair()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
	}
}

void Kripta::Stair::PostUpdate(float dt)
{
}

void Kripta::Stair::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (dead)
	{
		return;
	}
}

Kripta::Switch::Switch()
{
	switch_down_spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("switch_down_spr"));
	switch_up_spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("switch_up_spr"));
	id = Kripta::ObjectID::Switch;
}

Kripta::Switch::~Switch()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
	}
}

void Kripta::Switch::PreUpdate(float dt)
{
	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (state == 0)
			Kripta::IGame::instance->sprite_batch->Draw(&switch_down_spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
		else
			Kripta::IGame::instance->sprite_batch->Draw(&switch_up_spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));

	}
	EndTransform(false);
}

void Kripta::Switch::PostUpdate(float dt)
{
	/*if (!once && state == 0)
	{
		if (Kripta::IGame::instance->turn_number - activated_at_turn > 1)
		{
			Touch(this);
		}
	}*/
}

void Kripta::Switch::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
	{
		Kripta::IGame::instance->PushToPreUpdateList(this);
	}


	if (dead) return;
}

void Kripta::Switch::Touch(Kripta::Object * owner)
{
	if (once && state == 0) return;

	state = !state;

	if (state == 0)
	{
		//do something
		if (act_id == 1)
		{
			auto room_pick = Kripta::IGame::instance->PickRoom(act_x, act_y);
			int& tile = *(room_pick.tile_ptr);

			switch (tile)
			{
				case 7 : //closed door to open door
				{
					tile = 6;
				}
				break;

				case 6: //open door to closed door
				{
					tile = 7;
				}
				break;
			}
		}

		activated_at_turn = Kripta::IGame::instance->turn_number;
	}
	else
	{
		//do nothing
		if (act_id == 1)
		{
			auto room_pick = Kripta::IGame::instance->PickRoom(act_x, act_y);
			int& tile = *(room_pick.tile_ptr);

			switch (tile)
			{
			case 7: //closed door to open door
			{
				tile = 6;
			}
			break;

			case 6: //open door to closed door
			{
				tile = 7;
			}
			break;
			}
		}
	}
}

Kripta::Torch::Torch()
{
	id = Kripta::ObjectID::Torch;

	frame0 = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("torch_frame_0"));
	frame1 = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("torch_frame_1"));
	frame2 = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("torch_frame_2"));
}

Kripta::Torch::~Torch()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		
	}
}

void Kripta::Torch::Update(float dt)
{
	animation_time += dt* 1.75f;
	if (animation_time > 1.f) animation_time = 0.f;

	int frame = 0;
	int frame_to = 1;
	float blend_to = 0.f;

	if (animation_time < 0.25f) { frame = 0; frame_to = 1; blend_to = animation_time / 0.25f; }
	else
	if (animation_time < 0.5f) { frame = 1; frame_to = 2; blend_to = (animation_time - 0.25f) / 0.25f; }
	else
	if (animation_time < 0.75f) { frame = 2; frame_to = 1; blend_to = (animation_time - 0.5f) / 0.25f; }
	else
	if (animation_time < 1.f) { frame = 1; frame_to = 0; blend_to = (animation_time - 0.75f) / 0.25f; }

	if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
	{
		glm::vec3 pos;
		BeginTransform();
		{
			pos = GetPosition();

			frame0.tint_power = 0.f;//0.49f * (1.0f - blend_to);
			if (frame == 0)
			Kripta::IGame::instance->sprite_batch->Draw(&frame0, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));

			frame1.tint_power = 0.f;//0.49f * (1.0f - blend_to);
			if (frame == 1)
				Kripta::IGame::instance->sprite_batch->Draw(&frame1, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));

			frame2.tint_power = 0.f;//0.49f * (1.0f - blend_to);
			if (frame == 2)
				Kripta::IGame::instance->sprite_batch->Draw(&frame2, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));

			frame0.tint_power = 0.49f * blend_to;
			if (frame_to == 0)
				Kripta::IGame::instance->sprite_batch->Draw(&frame0, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));

			frame1.tint_power = 0.49f * blend_to;
			if (frame_to == 1)
				Kripta::IGame::instance->sprite_batch->Draw(&frame1, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));

			frame2.tint_power = 0.49f * blend_to;
			if (frame_to == 2)
				Kripta::IGame::instance->sprite_batch->Draw(&frame2, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
		}
		EndTransform(false);
	}
}

Kripta::MysteriousPotion::MysteriousPotion()
{
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("mysterious_potion"));

	id = Kripta::ObjectID::MysteriousPotion;
}

Kripta::MysteriousPotion::~MysteriousPotion()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
	}
}

void Kripta::MysteriousPotion::PreUpdate(float dt)
{
	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
	}
	EndTransform(false);
}

void Kripta::MysteriousPotion::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (dead)
	{
		return;
	}

	if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
	{
		Kripta::IGame::instance->PushToPreUpdateList(this);
	}
}

Kripta::Slime::Slime()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("slime_spr"));

	id = Kripta::ObjectID::Slime;
}

Kripta::Slime::~Slime()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);
		Kripta::IGame::instance->CreateTombForMe(this);


		auto is_door_nearby = [](int g_x, int g_y)->bool 
		{
			if (Kripta::IGame::instance->PickRoom(g_x, g_y - 1).door) return true;
			if (Kripta::IGame::instance->PickRoom(g_x, g_y + 1).door) return true;
			if (Kripta::IGame::instance->PickRoom(g_x - 1, g_y).door) return true;
			if (Kripta::IGame::instance->PickRoom(g_x + 1, g_y).door) return true;

			return false;
		};

		auto is_empty_at = [](int g_x, int g_y)->bool
		{
			auto room_pick = Kripta::IGame::instance->PickRoom(g_x, g_y);
			if (room_pick.place_obj == nullptr &&
				room_pick.floor == true) return true;

			return false;
		};

		int slimes_limit = 2;

		if (slimes_limit > 0 && is_door_nearby(grid_x + 1, grid_y) == false && is_empty_at(grid_x + 1, grid_y) == true)
		{
			auto slime_tiny = new Kripta::SlimeTiny();
			slime_tiny->Place(grid_x + 1, grid_y);
			Kripta::IGame::instance->GetRoom()->AddChild(slime_tiny);
			slime_tiny->SetLevel(static_cast<int>(glm::ceil(static_cast<float>(level / 2.0f))));

			slimes_limit--;
		}

		if (slimes_limit > 0 && is_door_nearby(grid_x - 1, grid_y) == false && is_empty_at(grid_x - 1, grid_y) == true)
		{
			auto slime_tiny = new Kripta::SlimeTiny();
			slime_tiny->Place(grid_x - 1, grid_y);
			Kripta::IGame::instance->GetRoom()->AddChild(slime_tiny);
			slime_tiny->SetLevel(static_cast<int>(glm::ceil(static_cast<float>(level / 2.0f))));

			slimes_limit--;
		}

		if (slimes_limit > 0 && is_door_nearby(grid_x, grid_y - 1) == false && is_empty_at(grid_x, grid_y - 1) == true)
		{
			auto slime_tiny = new Kripta::SlimeTiny();
			slime_tiny->Place(grid_x, grid_y - 1);
			Kripta::IGame::instance->GetRoom()->AddChild(slime_tiny);
			slime_tiny->SetLevel(static_cast<int>(glm::ceil(static_cast<float>(level / 2.0f))));

			slimes_limit--;
		}

		if (slimes_limit > 0 && is_door_nearby(grid_x, grid_y + 1) == false && is_empty_at(grid_x, grid_y + 1) == true)
		{
			auto slime_tiny = new Kripta::SlimeTiny();
			slime_tiny->Place(grid_x, grid_y + 1);
			Kripta::IGame::instance->GetRoom()->AddChild(slime_tiny);
			slime_tiny->SetLevel(static_cast<int>(glm::ceil(static_cast<float>(level / 2.0f))));

			slimes_limit--;
		}


		//Kripta::IGame::instance->PickRoom(grid_x, grid_y)
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::Slime::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}

	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;



		if (turn_complete == false)
		{
			glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();
			glm::vec2 hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

			int i_hero_action_x = static_cast<int>(hero_action_xy.x);
			int i_hero_action_y = static_cast<int>(hero_action_xy.y);

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				if (hero_action_room_pick.door == false)
					last_seen_hero_xy = hero_xy;
				else
					last_seen_hero_xy = glm::vec2(static_cast<float>(grid_x), static_cast<float>(grid_y));
			}



			if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) && hero_action_room_pick.door == false)
			{
				float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
				float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
				float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
				float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


				if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y - 1;
					}

				if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y + 1;
					}

				if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
					{
						action_grid_x = grid_x + 1;
						action_grid_y = grid_y;
					}

				if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
					if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
					{
						action_grid_x = grid_x - 1;
						action_grid_y = grid_y;
					}

				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
		}



		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}
	}
}

Kripta::SlimeTiny::SlimeTiny()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("slime_tiny_spr"));

	id = Kripta::ObjectID::SlimeTiny;
}

Kripta::SlimeTiny::~SlimeTiny()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);
		//Kripta::IGame::instance->CreateTombForMe(this);
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::SlimeTiny::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}

	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;



		if (turn_complete == false)
		{
			glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();
			glm::vec2 hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

			int i_hero_action_x = static_cast<int>(hero_action_xy.x);
			int i_hero_action_y = static_cast<int>(hero_action_xy.y);

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				if (hero_action_room_pick.door == false)
					last_seen_hero_xy = hero_xy;
				else
					last_seen_hero_xy = glm::vec2(static_cast<float>(grid_x), static_cast<float>(grid_y));
			}



			if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) && hero_action_room_pick.door == false)
			{
				float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
				float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
				float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
				float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


				if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y - 1;
					}

				if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y + 1;
					}

				if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
					{
						action_grid_x = grid_x + 1;
						action_grid_y = grid_y;
					}

				if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
					if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
					{
						action_grid_x = grid_x - 1;
						action_grid_y = grid_y;
					}

				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
		}



		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}
	}
}

Kripta::Skeleton::Skeleton()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("skeleton_spr"));

	id = Kripta::ObjectID::Skeleton;
}

Kripta::Skeleton::~Skeleton()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);
		Kripta::IGame::instance->CreateTombForMe(this);
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::Skeleton::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(18.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}

	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;



		if (turn_complete == false)
		{
			glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();
			glm::vec2 hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

			int i_hero_action_x = static_cast<int>(hero_action_xy.x);
			int i_hero_action_y = static_cast<int>(hero_action_xy.y);

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				if (hero_action_room_pick.door == false)
					last_seen_hero_xy = hero_xy;
				else
					last_seen_hero_xy = glm::vec2(static_cast<float>(grid_x), static_cast<float>(grid_y));
			}



			if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) && hero_action_room_pick.door == false)
			{
				float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
				float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
				float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
				float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


				if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y - 1;
					}

				if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y + 1;
					}

				if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
					{
						action_grid_x = grid_x + 1;
						action_grid_y = grid_y;
					}

				if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
					if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
					{
						action_grid_x = grid_x - 1;
						action_grid_y = grid_y;
					}

				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
		}



		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}
	}
}

void Kripta::Skeleton::SetLevel(int level)
{
	Kripta::Object::SetLevel(level);
	hp *= 2;
	hpmax = hp;
}

Kripta::Vampire::Vampire()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("vampire_spr"));

	id = Kripta::ObjectID::Vampire;
}

Kripta::Vampire::~Vampire()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);
		if (bat_activated)
		{
			auto bat = new Kripta::Bat();
			bat->Place(grid_x, grid_y);
			Kripta::IGame::instance->GetRoom()->AddChild(bat);
			bat->SetLevel(static_cast<int>(glm::ceil(static_cast<float>(level / 2.0f))));
			bat->vampire_activated = true;
			bat->hpmax = hpmax;
		}
		else
		{
			Kripta::IGame::instance->CreateTombForMe(this);
		}
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::Vampire::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}

	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;



		if (turn_complete == false)
		{
			glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();
			glm::vec2 hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

			int i_hero_action_x = static_cast<int>(hero_action_xy.x);
			int i_hero_action_y = static_cast<int>(hero_action_xy.y);

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				if (hero_action_room_pick.door == false)
					last_seen_hero_xy = hero_xy;
				else
					last_seen_hero_xy = glm::vec2(static_cast<float>(grid_x), static_cast<float>(grid_y));
			}



			if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) && hero_action_room_pick.door == false)
			{
				float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
				float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
				float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
				float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


				if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y - 1;
					}

				if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y + 1;
					}

				if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
					{
						action_grid_x = grid_x + 1;
						action_grid_y = grid_y;
					}

				if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
					if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
					{
						action_grid_x = grid_x - 1;
						action_grid_y = grid_y;
					}

				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
		}



		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp > 0 && static_cast<float>(hp) <= (static_cast<float>(hpmax) / 3.0f))
		{
			bat_activated = true;

			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
		}
		
		if (bat_activated == false && hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}

		
	}
}

Kripta::Bat::Bat()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("bat_spr"));

	id = Kripta::ObjectID::Bat;
}

Kripta::Bat::~Bat()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);

		if (vampire_activated && hp == hpmax)
		{
			auto vampire = new Kripta::Vampire();
			vampire->Place(grid_x, grid_y);
			Kripta::IGame::instance->GetRoom()->AddChild(vampire);
			vampire->SetLevel(level * 2);
		}
		else
		{
			Kripta::IGame::instance->CreateTombForMe(this);
			((Kripta::Tomb*)(Kripta::IGame::instance->PickRoom(grid_x, grid_y).special_obj))->hp = (vampire_activated == true ? 1 : 0);
		}
		
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::Bat::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}

	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;



		if (turn_complete == false)
		{
			glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();
			glm::vec2 hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

			int i_hero_action_x = static_cast<int>(hero_action_xy.x);
			int i_hero_action_y = static_cast<int>(hero_action_xy.y);

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				if (hero_action_room_pick.door == false)
					last_seen_hero_xy = hero_xy;
				else
					last_seen_hero_xy = glm::vec2(static_cast<float>(grid_x), static_cast<float>(grid_y));
			}



			if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) && hero_action_room_pick.door == false)
			{
				float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
				float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
				float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
				float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


				if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y - 1;
					}

				if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y + 1;
					}

				if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
					{
						action_grid_x = grid_x + 1;
						action_grid_y = grid_y;
					}

				if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
					if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
					{
						action_grid_x = grid_x - 1;
						action_grid_y = grid_y;
					}

				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
		}



		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp == hpmax && vampire_activated)
		{
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
		}
		
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}
	}
}

void Kripta::Bat::YouKick(int target_level)
{
	if (Kripta::IGame::instance->random.Value() < 0.5f)
	{
		hp += level;
		if (hp > hpmax) hp = hpmax;
	}
}

void Kripta::Bat::Kick(int attack_level)
{
	if (dead == true) return;

	int hp_prev = hp;
	
	if (Kripta::IGame::instance->random.Value() < 0.25f)
	{
		//do nothing
	}
	else
	{
		hp -= attack_level;
	}

	if (hp <= 0 && hp_prev > 0)
	{
		Kripta::TurnController::turn_members_died++;
	}
	else
	{
		//do nothing
	}
}

Kripta::Spider::Spider()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("goblin_spr"));

	id = Kripta::ObjectID::Spider;
}

Kripta::Spider::~Spider()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);

		Kripta::IGame::instance->CreateTombForMe(this);
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::Spider::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}

	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;



		if (turn_complete == false)
		{
			glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();
			glm::vec2 hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

			int i_hero_action_x = static_cast<int>(hero_action_xy.x);
			int i_hero_action_y = static_cast<int>(hero_action_xy.y);

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				if (hero_action_room_pick.door == false)
					last_seen_hero_xy = hero_xy;
				else
					last_seen_hero_xy = glm::vec2(static_cast<float>(grid_x), static_cast<float>(grid_y));
			}



			if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) && hero_action_room_pick.door == false)
			{
				float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
				float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
				float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
				float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


				if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y - 1;
					}

				if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y + 1;
					}

				if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
					{
						action_grid_x = grid_x + 1;
						action_grid_y = grid_y;
					}

				if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
					if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
					{
						action_grid_x = grid_x - 1;
						action_grid_y = grid_y;
					}

				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
		}



		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}
	}
}

Kripta::Ghost::Ghost()
{
	gameObject().AddComponent(new Kripta::TurnController());
	spr = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("goblin_spr"));

	id = Kripta::ObjectID::Ghost;
}

Kripta::Ghost::~Ghost()
{
	if (Kripta::IGame::instance->main_menu_enabled)
	{
		//do nothing
	}
	else
	{
		Kripta::IGame::instance->BlockGrid(grid_x, grid_y, nullptr);

		Kripta::IGame::instance->CreateTombForMe(this);
	}

	delete gameObject().GetComponent<Kripta::TurnController>();
}

void Kripta::Ghost::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);


	if (dead) return;


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
			//DrawHealthbar();
		}

	}
	EndTransform(false);

	auto turn_controller = GetComponent<Kripta::TurnController>();

	if (Kripta::IGame::instance->TurnStage() == 3 && turn_controller->turn_decision_made == true)
	{
		turn_controller->turn_decision_made = false;
		Kripta::TurnController::turn_members_decision_made--;
	}

	if (Kripta::IGame::instance->TurnStage() == 1 && turn_controller->turn_decision_made == false)
	{
		bool turn_complete = false;



		if (turn_complete == false)
		{
			glm::vec2 hero_xy = Kripta::IGame::instance->GetHeroXY();
			glm::vec2 hero_action_xy = Kripta::IGame::instance->GetHeroActionXY();

			int i_hero_action_x = static_cast<int>(hero_action_xy.x);
			int i_hero_action_y = static_cast<int>(hero_action_xy.y);

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				if (hero_action_room_pick.door == false)
					last_seen_hero_xy = hero_xy;
				else
					last_seen_hero_xy = glm::vec2(static_cast<float>(grid_x), static_cast<float>(grid_y));
			}



			if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) && hero_action_room_pick.door == false)
			{
				float cost_n = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y - 1));
				float cost_s = glm::length2(last_seen_hero_xy - glm::vec2(grid_x, grid_y + 1));
				float cost_e = glm::length2(last_seen_hero_xy - glm::vec2(grid_x + 1, grid_y));
				float cost_w = glm::length2(last_seen_hero_xy - glm::vec2(grid_x - 1, grid_y));


				if (cost_n <= cost_s && cost_n <= cost_e && cost_n <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y - 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y - 1;
					}

				if (cost_s <= cost_n && cost_s <= cost_e && cost_s <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x, grid_y + 1).floor)
					{
						action_grid_x = grid_x;
						action_grid_y = grid_y + 1;
					}

				if (cost_e <= cost_n && cost_e <= cost_s && cost_e <= cost_w)
					if (Kripta::IGame::instance->PickRoom(grid_x + 1, grid_y).floor)
					{
						action_grid_x = grid_x + 1;
						action_grid_y = grid_y;
					}

				if (cost_w <= cost_n && cost_w <= cost_s && cost_w <= cost_e)
					if (Kripta::IGame::instance->PickRoom(grid_x - 1, grid_y).floor)
					{
						action_grid_x = grid_x - 1;
						action_grid_y = grid_y;
					}

				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
			else
			{
				action_grid_x = grid_x;
				action_grid_y = grid_y;
				action_id = Kripta::ObjActionID::Act;
				turn_complete = true;
			}
		}



		if (turn_complete)
		{
			turn_controller->turn_decision_made = true;
			Kripta::TurnController::turn_members_decision_made++;
		}
	}

	if (Kripta::IGame::instance->TurnStage() == 4)
	{
		if (hp <= 0 && Kripta::TurnController::turn_members_died > 0)
		{
			Kripta::TurnController::turn_members_died--;
			dead = true;
			Kripta::IGame::instance->PushToDeathList(this);
			//SetParent(nullptr);
			//delete this;
		}
	}
}
