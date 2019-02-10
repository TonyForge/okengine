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

			if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
			{
				last_seen_hero_xy = hero_xy;
			}

			auto hero_action_room_pick = Kripta::IGame::instance->PickRoom(i_hero_action_x, i_hero_action_y);

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

void Kripta::Tomb::PostUpdate(float dt)
{
	if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
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
}

void Kripta::Tomb::Update(float dt)
{
	ok::GameObject::Update(dt);

	Kripta::IGame::instance->PushToPostUpdateList(this);

	if (dead)
	{
		return;
	}

	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
		}
	}
	EndTransform(false);

	if ((Kripta::IGame::instance->turn_number - initial_turn) >= turns_to_respawn)
	{
		if (creature_id == Kripta::ObjectID::Goblin)
		{ 
			auto character = new Kripta::Goblin();
			character->Place(grid_x, grid_y);
			GetParent()->AddChild(character);
			character->SetLevel(level);
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


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
		}
	}
	EndTransform(false);
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

void Kripta::HealthPotion::PostUpdate(float dt)
{
}

void Kripta::HealthPotion::Update(float dt)
{
	ok::GameObject::Update(dt);

	if (dead)
	{
		return;
	}


	glm::vec3 pos;
	BeginTransform();
	{
		pos = GetPosition();

		if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
		{
			Kripta::IGame::instance->sprite_batch->Draw(&spr, glm::vec2(pos.x, pos.y) + glm::vec2(16.f, 16.f), 0.f, glm::vec2(1.f, 1.f));
		}
	}
	EndTransform(false);
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

		if (last_seen_hero_xy != glm::vec2(grid_x, grid_y) &&
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
