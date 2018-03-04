#pragma once

#ifdef STARPOM

#include "Application.h"
#include "LayeredRenderer.h"
#include "projects\Starpom\Ship.h"
#include "projects\Starpom\Loading.h"


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