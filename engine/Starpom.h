#pragma once

#ifdef STARPOM

#include "Application.h"
#include "LayeredRenderer.h"
#include "projects\Starpom\Ship.h"
#include "projects\Starpom\Loading.h"
#include "projects\Starpom\Action.h"
#include "projects\Starpom\Planet.h"
#include "projects\Starpom\Spaceship.h"
#include "LineBatch.h"

namespace Starpom
{
	/*
	��� ������ ��������� ���� ����� ���������� (������ �� ������ ����� ���� � ����),
	��� ����������� ������� ���� ����� ��������� ��������� ��������.

	��������/���������� ���� � ����� �������� ����� � ���� (� ����� �� �������).
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