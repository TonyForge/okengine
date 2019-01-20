#include "Object.h"

bool Kripta::TurnController::turn_in_progress = false;
int Kripta::TurnController::turn_members_ready = 0;
int Kripta::TurnController::turn_members_total = 0;
int Kripta::TurnController::turn_members_decision_made = 0;
int Kripta::TurnController::turn_members_died = 0;

Kripta::TurnController::TurnController()
{
	turn_members_total++;
}

Kripta::TurnController::~TurnController()
{
	turn_members_total--;
	Kripta::Object& obj = *static_cast<Kripta::Object*>(&gameObject());
	Kripta::IGame::instance->BlockGrid(obj.grid_x, obj.grid_y, nullptr);
}

void Kripta::TurnController::Update(float dt)
{

	Kripta::Object& obj = *static_cast<Kripta::Object*>(&gameObject());
	
	if (turn_in_progress)
	{
		if (obj.action_id == Kripta::ObjActionID::Wait)
		{
			//do nothing
		}
		else
		{
			if (obj.action_id == Kripta::ObjActionID::Idle)
			{
				turn_members_ready++;
				obj.action_id = Kripta::ObjActionID::Wait;
			}

			if (obj.action_id == Kripta::ObjActionID::Act)
			{
				auto grid_pick = Kripta::IGame::instance->PickRoom(obj.action_grid_x, obj.action_grid_y);

				if (grid_pick.wall)
				{
					obj.action_id = Kripta::ObjActionID::FalseMove;
					obj.movement_path.Reset();
					obj.movement_path.Add(glm::vec2(obj.grid_x, obj.grid_y));
					obj.movement_path.Add(
						glm::vec2(
							static_cast<float>(obj.grid_x) + static_cast<float>(obj.action_grid_x - obj.grid_x)*0.5f,
							static_cast<float>(obj.grid_y) + static_cast<float>(obj.action_grid_y - obj.grid_y)*0.5f
						)
					);
					obj.movement_path.Add(glm::vec2(obj.grid_x, obj.grid_y));
				}

				if (grid_pick.wall == false)
				{
					if (grid_pick.floor)
					{
						//same position
						if (obj.grid_x == obj.action_grid_x && obj.grid_y == obj.action_grid_y)
						{
							//pickup floor item
							obj.action_id = Kripta::ObjActionID::Pickup;
						}
						else
						{
							//other position
							if (grid_pick.place_obj == nullptr)
							{
								while (true)
								{
									if (obj.id == Kripta::ObjectID::Hero)
									{
										if (grid_pick.floor_obj != nullptr)
										{
											obj.action_id = Kripta::ObjActionID::MoveAndPickup;
											obj.movement_path.Reset();
											obj.movement_path.Add(glm::vec2(obj.grid_x, obj.grid_y));
											obj.movement_path.Add(glm::vec2(obj.action_grid_x, obj.action_grid_y));

											Kripta::IGame::instance->BlockGrid(obj.grid_x, obj.grid_y, nullptr);
											Kripta::IGame::instance->BlockGrid(obj.action_grid_x, obj.action_grid_y, &obj);

											break;
										}
										
									}

									obj.action_id = Kripta::ObjActionID::Move;
									obj.movement_path.Reset();
									obj.movement_path.Add(glm::vec2(obj.grid_x, obj.grid_y));
									obj.movement_path.Add(glm::vec2(obj.action_grid_x, obj.action_grid_y));

									Kripta::IGame::instance->BlockGrid(obj.grid_x, obj.grid_y, nullptr);
									Kripta::IGame::instance->BlockGrid(obj.action_grid_x, obj.action_grid_y, &obj);

									break;
								}
								
							}
							else
							{
								grid_pick.place_obj->Kick(obj.level);
								obj.action_id = Kripta::ObjActionID::Attack;
								obj.movement_path.Reset();
								obj.movement_path.Add(glm::vec2(obj.grid_x, obj.grid_y));
								obj.movement_path.Add(
									glm::vec2(
										static_cast<float>(obj.grid_x) + static_cast<float>(obj.action_grid_x - obj.grid_x)*0.5f,
										static_cast<float>(obj.grid_y) + static_cast<float>(obj.action_grid_y - obj.grid_y)*0.5f
									)
								);
								obj.movement_path.Add(glm::vec2(obj.grid_x, obj.grid_y));
							}
						}
					}
					else
					{
						if (grid_pick.door)
						{
							obj.action_id = Kripta::ObjActionID::ToDoor;

							obj.movement_path.Reset();
							obj.movement_path.Add(glm::vec2(obj.grid_x, obj.grid_y));
							obj.movement_path.Add(
								glm::vec2(obj.action_grid_x, obj.action_grid_y) + 
								glm::vec2(
									static_cast<float>(obj.action_grid_x - obj.grid_x),
									static_cast<float>(obj.action_grid_y - obj.grid_y)
								)
							);

							glm::vec2 h = glm::vec2(
								static_cast<float>(obj.action_grid_x - obj.grid_x),
								static_cast<float>(obj.action_grid_y - obj.grid_y)) * 2.f;

							obj.action_grid_x = static_cast<int>(static_cast<float>(obj.grid_x) + h.x);
							obj.action_grid_y = static_cast<int>(static_cast<float>(obj.grid_y) + h.y);

							Kripta::IGame::instance->BlockGrid(obj.grid_x, obj.grid_y, nullptr);
							Kripta::IGame::instance->BlockGrid(obj.action_grid_x, obj.action_grid_y, &obj);
						}

						if (grid_pick.stair)
						{
							obj.action_id = Kripta::ObjActionID::ToStair;

							obj.movement_path.Reset();
							obj.movement_path.Add(glm::vec2(obj.grid_x, obj.grid_y));
							obj.movement_path.Add(
								glm::vec2(obj.action_grid_x, obj.action_grid_y) +
								glm::vec2(
									static_cast<float>(obj.action_grid_x - obj.grid_x),
									static_cast<float>(obj.action_grid_y - obj.grid_y)
								)
							);
						}
					}

				}
			}

			if (obj.action_id == Kripta::ObjActionID::FalseMove)
			{
				if (obj.movement_path.IsEndOfPath() == false)
				{
					glm::vec2 next_wp = obj.movement_path.StepPath(dt * 4.f);

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(next_wp.x, next_wp.y, 0.f) * 32.f);
					obj.EndTransform(true);
				}
				else
				{
					obj.action_id = Kripta::ObjActionID::Idle;

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(obj.grid_x, obj.grid_y, 0.f) * 32.f);
					obj.EndTransform(true);
				}
			}

			if (obj.action_id == Kripta::ObjActionID::Attack)
			{
				if (obj.movement_path.IsEndOfPath() == false)
				{
					glm::vec2 next_wp = obj.movement_path.StepPath(dt * 4.f);

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(next_wp.x, next_wp.y, 0.f) * 32.f);
					obj.EndTransform(true);
				}
				else
				{
					obj.action_id = Kripta::ObjActionID::Idle;

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(obj.grid_x, obj.grid_y, 0.f) * 32.f);
					obj.EndTransform(true);
				}
			}

			if (obj.action_id == Kripta::ObjActionID::Move)
			{
				if (obj.movement_path.IsEndOfPath() == false)
				{
					glm::vec2 next_wp = obj.movement_path.StepPath(dt*4.f);

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(next_wp.x, next_wp.y, 0.f) * 32.f);
					obj.EndTransform(true);
				}
				else
				{
					obj.action_id = Kripta::ObjActionID::Idle;

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(obj.action_grid_x, obj.action_grid_y, 0.f) * 32.f);
					obj.EndTransform(true);

					obj.grid_x = obj.action_grid_x;
					obj.grid_y = obj.action_grid_y;
				}
			}

			if (obj.action_id == Kripta::ObjActionID::MoveAndPickup)
			{
				if (obj.movement_path.IsEndOfPath() == false)
				{
					glm::vec2 next_wp = obj.movement_path.StepPath(dt*4.f);

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(next_wp.x, next_wp.y, 0.f) * 32.f);
					obj.EndTransform(true);
				}
				else
				{
					auto grid_pick = Kripta::IGame::instance->PickRoom(obj.action_grid_x, obj.action_grid_y);

					Kripta::Object* floor_obj = static_cast<Kripta::Object*>(grid_pick.floor_obj);

					obj.PickUpObject(floor_obj);
					
					//if (floor_obj->id == Kripta::ObjectID::GoldPile)
					//{
						//obj.
					//}

					obj.action_id = Kripta::ObjActionID::Idle;

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(obj.action_grid_x, obj.action_grid_y, 0.f) * 32.f);
					obj.EndTransform(true);

					obj.grid_x = obj.action_grid_x;
					obj.grid_y = obj.action_grid_y;
				}
			}

			if (obj.action_id == Kripta::ObjActionID::ToDoor)
			{
				if (obj.movement_path.IsEndOfPath() == false)
				{
					glm::vec2 next_wp = obj.movement_path.StepPath(dt * 4.f);

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(next_wp.x, next_wp.y, 0.f) * 32.f);
					obj.EndTransform(true);
				}
				else
				{
					obj.action_id = Kripta::ObjActionID::Idle;

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(obj.action_grid_x, obj.action_grid_y, 0.f) * 32.f);
					obj.EndTransform(true);

					obj.grid_x = obj.action_grid_x;
					obj.grid_y = obj.action_grid_y;
				}
			}

			if (obj.action_id == Kripta::ObjActionID::ToStair)
			{
				if (obj.movement_path.IsEndOfPath() == false)
				{
					glm::vec2 next_wp = obj.movement_path.StepPath(dt * 4.f);

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(next_wp.x, next_wp.y, 0.f) * 32.f);
					obj.EndTransform(true);
				}
				else
				{
					obj.action_id = Kripta::ObjActionID::Idle;

					obj.BeginTransform();
					obj.SetPosition(glm::vec3(obj.action_grid_x, obj.action_grid_y, 0.f) * 32.f);
					obj.EndTransform(true);

					obj.grid_x = obj.action_grid_x;
					obj.grid_y = obj.action_grid_y;
				}
			}

			if (obj.action_id == Kripta::ObjActionID::Pickup)
			{
				obj.action_id = Kripta::ObjActionID::Idle;
			}
		}
	}
}

void Kripta::Object::PickUpObject(Kripta::Object * obj)
{
}

void Kripta::Object::PostUpdate(float dt)
{
	if (Kripta::IGame::instance->GetFov(grid_x, grid_y))
	{
		BeginTransform();
		glm::vec3 pos = GetPosition() + glm::vec3(16.f, 16.f, 0.f);
		EndTransform(false);

		DrawHealthbar();

		auto campos = ok::graphics::Camera::GetCurrent()->GetPosition();


		Kripta::IGame::instance->text_batch->SetBrushAlignHorizontal(ok::graphics::TextAlign::Center);
		Kripta::IGame::instance->text_batch->SetBrushAlignVertical(ok::graphics::TextAlign::Center);
		Kripta::IGame::instance->text_batch->SetBrushSize(12);
		Kripta::IGame::instance->text_batch->SetBrushPosition(glm::vec2(-campos.x, -campos.y)+ glm::vec2(pos.x, pos.y) + glm::vec2(0.f, -32.f));

		Kripta::IGame::instance->text_batch->Draw("-"+ok::String(std::to_string(level).c_str())+"-");
	}
}

void Kripta::Object::SetLevel(int level)
{
	this->level = level;
	hp = 3 * level;
}

void Kripta::Object::Place(int grid_x, int grid_y)
{
	if (id != Kripta::ObjectID::Unknown &&
		id != Kripta::ObjectID::Tomb &&
		id != Kripta::ObjectID::GoldPile &&
		id != Kripta::ObjectID::HealthPotion)
	Kripta::IGame::instance->BlockGrid(grid_x, grid_y, this);

	if (id == Kripta::ObjectID::GoldPile ||
		id == Kripta::ObjectID::HealthPotion)
		Kripta::IGame::instance->BlockFloor(grid_x, grid_y, this);



	this->grid_x = grid_x;
	this->grid_y = grid_y;

	last_seen_hero_xy.x = static_cast<float>(grid_x);
	last_seen_hero_xy.y = static_cast<float>(grid_y);

	BeginTransform();
	SetPosition(glm::vec3(grid_x, grid_y, 0.f) * 32.f);
	EndTransform(true);
}

void Kripta::Object::Kick(int attack_level)
{
	int hp_prev = hp;
	hp -= attack_level;

	if (hp <= 0 && hp_prev > 0)
	{
		Kripta::TurnController::turn_members_died++;
	}
}

void Kripta::Object::DrawHealthbar()
{
	BeginTransform();
	glm::vec3 pos = GetPosition() + glm::vec3(16.f, 16.f, 0.f);
	EndTransform(false);

	ok::graphics::SpriteInfo blank = ok::AssetsBasic::instance().GetSpriteAtlas("sprites.atlas")->Get(ok::String("blank"));

	blank.tint_color = ok::Color(1.f, 0.f, 0.f, 1.f);
	blank.tint_power = 1.f;
	Kripta::IGame::instance->sprite_batch->Draw(&blank, glm::vec2(pos.x - 12.f, pos.y - 16.f - 6.f), 0.f, glm::vec2((1.f / 32.f) * 24.f, (1.f / 32.f) * 4.f));

	blank.tint_color = ok::Color(0.f, 1.f, 0.f, 1.f);
	blank.tint_power = 1.f;
	Kripta::IGame::instance->sprite_batch->Draw(&blank, glm::vec2(pos.x - 12.f, pos.y - 16.f - 6.f), 0.f, glm::vec2((1.f / 32.f) * (24.f * (static_cast<float>(hp) / (3.f*static_cast<float>(level)))), (1.f / 32.f) * 4.f));
}
