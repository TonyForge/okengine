#pragma once

#include "..\..\GameObject.h"
#include "..\..\Camera.h"
#include "IGame.h"
#include "Commands.h"

namespace Zoner
{
	class IShip;

	class ISpace : public ok::GameObject
	{
	public:
		ISpace();

		virtual std::vector<Zoner::IShip*>& Visitors() = 0;

		virtual void VisitorIn(Zoner::IShip* visitor) = 0;
		virtual void VisitorOut(Zoner::IShip* visitor, Zoner::ISpace* destination) = 0;

		ok::graphics::Camera camera;
		glm::vec3 camera_follow_start;
		bool camera_follow_enabled;
		glm::vec2 camera_grab_start;
		bool camera_grab_enabled;

		virtual void DoCameraFollow() = 0;
		virtual void CameraUpdate(float dt) = 0;

		virtual std::vector<Zoner::IShip*>& WhoIsThere(glm::vec2 space_xy) = 0;
		static std::vector<Zoner::IShip*> _who_is_there_container;

		bool isCurrent;

		Zoner::CommandsList cmd_sequence;
		Zoner::CommandsList cmd_parallel;
	protected:
	private:
	};
}