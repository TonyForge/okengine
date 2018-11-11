#pragma once

#include "..\..\Application.h"
#include "..\..\Assets.h"
#include "..\..\Camera.h"

#include "PlayerNode.h"

namespace Treasurer
{
	class Game : public ok::Application
	{
	public:
		Game();
		void Init();
		void Update(float dt);

		ok::graphics::Camera* camera;

		std::vector<ok::Treasurer::PlayerNode*> blue_team;
		std::vector<ok::Treasurer::PlayerNode*> red_team;

		ok::Treasurer::PlayerNode* tank;
	};
}