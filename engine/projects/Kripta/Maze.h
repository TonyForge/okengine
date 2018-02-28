#pragma once

#include "..\..\GameObject.h"
#include "..\..\Mesh.h"

namespace Kripta
{
	enum class MazeBlockType
	{
		Empty,
		Full,
		Floor,
		Wall,
		Gate,
		Tunnel,
		Stairs,
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
		ok::GameObject decorations_root;
	};

	class Maze
	{
	public:
		Maze(int x, int y, int w, int h);
		static void LoadMeshes();
	protected:
	private:
		std::vector<Kripta::MazeBlock> _blocks;
		static std::vector<ok::graphics::Mesh*> _meshes;
	};
}