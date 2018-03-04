#pragma once

#ifdef STARPOM

#include "Application.h"
#include "LayeredRenderer.h"
#include "projects\Starpom\Ship.h"
#include "projects\Starpom\Loading.h"


namespace Starpom
{
	/*
	При первом включении игра сразу начинается (словно мы нажали Новая Игра в меню),
	все последующие запуски игра сразу загружает последнюю сохранку.

	Загрузка/Сохранение игры и Опции доступны прямо в игре (в любом из режимов).
	*/

	class App : public ok::Application
	{
	public:
		App();
		void Init();
		void Update(float dt);
	protected:
	private:
	};
}

#endif