#pragma once

#include "..\..\GameObject.h"
#include "..\..\Mesh.h"

namespace Kripta
{
	enum class MazeBlockType
	{
		Empty,
		Floor,
		Wall,
		Gate,
		Window
	};

	enum class MazeBlockOrientation
	{
		North,
		East,
		West,
		South
	};

	class MazeDecoration : public ok::GameObject
	{
	public:
		ok::GameObject* Duplicate(ok::GameObject* _clone = nullptr);
	};

	class MazeBlock
	{
	public:
		Kripta::MazeBlockType block_type;
		Kripta::MazeBlockOrientation orientation;
	};

	class Maze
	{
	public:
	protected:
	private:
		std::vector<ok::graphics::Mesh*> _tiles;
	};
}