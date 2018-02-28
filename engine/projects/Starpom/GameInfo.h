#pragma once

namespace Starpom
{
	enum class GameMode
	{
		GM_LOADING,
		GM_STAR_SYSTEM,
		GM_UNIVERSE
	};

	class GameInfo
	{
	public:
		static Starpom::GameInfo& instance();
		static Starpom::GameInfo& o();

		Starpom::GameMode GetMode();

	protected:
	private:
		GameInfo() {}
		~GameInfo() {}
		Starpom::GameInfo(Starpom::GameInfo const&) {}
		Starpom::GameInfo& operator= (Starpom::GameInfo const&) {}

		Starpom::GameMode _game_mode;
	};

}