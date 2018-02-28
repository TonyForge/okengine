#pragma once

#include "..\..\GameObject.h"
#include "GameInfo.h"

namespace Starpom
{
	//SS - Star System - ����� �������� ������� (������� � ��������� � ��������)

	/*
	SS_ShipAgent - ��� �����, ���������� ������������� ����������� �������� � ������ �������� �������.
	- �������,
	- �������,
	- �������,
	- ������,
	- ���������,
	- ������������ ������,
	- �������
	*/
	class SS_ShipAgent : public ok::GameObject
	{
	public:
		void Update(float dt);
		ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);

		static std::unordered_map<std::string, Starpom::SS_ShipAgent*> blueprints;
	protected:
	private:
		
	};

	/*
	Ship - ��� ����������� ������ (����������� � �� ������� �� �������� ������, �������� ������� ��� ������� �� �������)
	*/
	class Ship : public ok::GameObject
	{
	public:
		Ship();
		void Update(float dt);
		ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);

		Starpom::SS_ShipAgent* ss_agent;
		glm::vec3 GetPosition();
	protected:
	private:
	};
}