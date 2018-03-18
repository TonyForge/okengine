#pragma once

namespace Zoner
{
	class IGame
	{
	public:
		virtual int GetScreenWidth() = 0;
		virtual int GetScreenHeight() = 0;
		static Zoner::IGame& o();
	protected:
		static Zoner::IGame* _instance;
	private:
		
	};
}