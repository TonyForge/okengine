#include "Space.h"

void Zoner::Space::Update(float dt)
{
	ok::GameObject::Update(dt);

	CameraUpdate(dt);

	ok::graphics::Camera::PushCamera(&camera);

	for (auto& visitor : visitors)
	{
		visitor->Update(dt);
	}

	RelocateVisitors();

	for (auto& jump_hole : jump_holes)
	{
		jump_hole->Update(dt);
	}

	ok::graphics::LayeredRenderer::instance().Flush();

	ok::graphics::Camera::PopCamera();
}

void Zoner::Space::VisitorIn(Zoner::IShip * visitor)
{
	visitors.push_back(visitor);

	//visitor->Location() in this line keeps space ptr of visitors previous location
	//and the it changes to this

	visitor->Location() = this;
	visitor->RelocationComplete();
}

void Zoner::Space::VisitorOut(Zoner::IShip * visitor, Zoner::ISpace* destination)
{
	visitors_out.push_back(std::make_pair(visitor, destination));
}

void Zoner::Space::RelocateVisitors()
{
	for (auto& visitor : visitors_out)
	{
		visitors.erase(std::remove(visitors.begin(), visitors.end(), visitor.first), visitors.end());
		visitor.second->VisitorIn(visitor.first);
	}

	visitors_out.clear();
}

std::vector<Zoner::IShip*>& Zoner::Space::WhoIsThere(glm::vec2 space_xy)
{
	_who_is_there_container.clear();

	for (auto&& visitor : visitors)
	{
		Zoner::Collision::Point pick = visitor->Pick(glm::vec3(space_xy, 0.f));

		if (pick.hit)
		{
			_who_is_there_container.push_back(visitor);
		}
	}

	for (auto&& jump_hole : jump_holes)
	{
		Zoner::Collision::Point pick = jump_hole->Pick(glm::vec3(space_xy, 0.f));

		if (pick.hit)
		{
			_who_is_there_container.push_back(jump_hole);
		}
	}

	return _who_is_there_container;
}

std::vector<Zoner::IShip*>& Zoner::Space::Visitors()
{
	return visitors;
}

void Zoner::Space::DoCameraFollow()
{
	if (camera_follow_enabled == false)
	{
		camera_follow_start = camera.GetPosition();
		camera_follow_enabled = true;
	}
}

void Zoner::Space::CameraUpdate(float dt)
{
	float camera_border_x = 8.f / camera.GetViewportWidth();
	float camera_border_y = 8.f / camera.GetViewportHeight();
	const float camera_shift_speed = 600.f; //pixels per second
	float camera_shift_x = 0.f;
	float camera_shift_y = 0.f;

	glm::vec3 camera_grab_target;

	if (ok::Input::o().KeyDown(ok::MKey::Right))
	{
		if (camera_grab_enabled)
		{
			camera_shift_x = (ok::Input::o().MouseX() - camera_grab_start.x) * camera.GetViewportWidth();
			camera_shift_y = (ok::Input::o().MouseY() - camera_grab_start.y) * camera.GetViewportHeight();
		}
		else
		{
			camera_follow_start = camera.GetPosition();
			camera_grab_start.x = ok::Input::o().MouseX();
			camera_grab_start.y = ok::Input::o().MouseY();
			camera_grab_enabled = true;
			camera_follow_enabled = false;
		}
	}
	else
	{
		camera_grab_enabled = false;
	}
	
	if (camera_grab_enabled)
	{
		camera.SetPosition(camera_follow_start - glm::vec3(camera_shift_x, camera_shift_y, 0.f));
	}
	else
	{
		/*if (Zoner::IGame::o().StateFalse(Zoner::GameStates::PauseEnabled))
		{
			DoCameraFollow();
		}*/

		if (ok::Input::o().MouseX() <= camera_border_x)
		{
			camera_shift_x -= camera_shift_speed * dt;
			camera_follow_enabled = false;
		}

		if (ok::Input::o().MouseX() >= 1.f - camera_border_x)
		{
			camera_shift_x += camera_shift_speed * dt;
			camera_follow_enabled = false;
		}

		if (ok::Input::o().MouseY() <= camera_border_y)
		{
			camera_shift_y -= camera_shift_speed * dt;
			camera_follow_enabled = false;
		}

		if (ok::Input::o().MouseY() >= 1.f - camera_border_y)
		{
			camera_shift_y += camera_shift_speed * dt;
			camera_follow_enabled = false;
		}

		if (camera_follow_enabled)
		{
			Zoner::IShip* _current_player_ship = Zoner::IGame::o().GetCurrentPlayerShip();

			if (_current_player_ship != nullptr)
			{
				glm::vec3 camera_follow_end;
				
				if (_current_player_ship->trajectory.Length() > 0.f)
					camera_follow_end = _current_player_ship->trajectory.Pick(1.f).position;
				else
					camera_follow_end = _current_player_ship->GetPosition();

				glm::vec3& camera_position = camera.GetPosition();
				ok::Rect2Df camera_follow_rect;
				camera_follow_rect.SetXYWH(
					camera_position.x - camera.GetViewportWidth()*0.5f,
					camera_position.y - camera.GetViewportHeight()*0.5f,
					camera.GetViewportWidth(),
					camera.GetViewportHeight()
				);

				glm::vec2 camera_follow_rect_pick = camera_follow_rect.PickRayFromCenter(camera_follow_start - camera_follow_end) - camera_follow_rect.GetCenter();

				if (glm::length2(_current_player_ship->GetPosition() - camera_follow_end) >= glm::length2(camera_follow_rect_pick))
				{
					camera_follow_end = _current_player_ship->GetPosition() - glm::vec3(camera_follow_rect_pick, 0.f);

					camera_follow_end.z = camera.GetPosition().z;
					camera.SetPosition(camera_follow_end);
				}
				else
				{
					camera_follow_end.z = camera.GetPosition().z;
					camera.SetPosition(camera_follow_end);
				}
				
			}
		}
		else
		{
			camera.SetPosition(camera.GetPosition() + glm::vec3(camera_shift_x, camera_shift_y, 0.f));
		}
	}
	
}
