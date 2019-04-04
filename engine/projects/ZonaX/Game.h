#pragma once

#include "..\..\Application.h"

namespace ZonaX
{
	class IGame : public ok::Application
	{
	public:
		Game();
		void Init();
		void Update(float dt);
	protected:
	private:
	};

	class Enemy : public ok::GameObject
	{
	public:
	private:
	protected:
	};

	class Player : public ok::GameObject
	{
	public:
	private:
	protected:
	};

	class Game : public ok::Application
	{
	public:
		Game();
		void Init();
		void Update(float dt);
	protected:
	private:
	};
}