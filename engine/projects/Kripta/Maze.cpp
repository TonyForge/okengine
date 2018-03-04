#include "Maze.h"

std::vector<ok::graphics::Mesh*> Kripta::Maze::_meshes;

ok::GameObject * Kripta::MazeDecoration::Duplicate(ok::GameObject * _clone)
{
	Kripta::MazeDecoration* duplicate = new Kripta::MazeDecoration(*static_cast<Kripta::MazeDecoration*>(_clone));
	return _clone;
}

Kripta::Maze::Maze(int x, int y, int w, int h)
{
	_blocks.resize(w*h);

	int bx = x;
	int by = y;
	int bw = x + w;
	int bh = y + h;

	for (auto& block : _blocks)
	{
		block.block_type = Kripta::MazeBlockType::Full;
		block.orientation = Kripta::MazeBlockOrientation::North;

		block.decorations_root.BeginTransform();
		block.decorations_root.SetPosition(glm::vec3(bx, by, 0.f));
		block.decorations_root.EndTransform(false);

		bx++;
		if (bx < bw)
		{
			//do nothing
		}
		else
		{
			bx = x;
			by++;
		}
	}
}

void Kripta::Maze::LoadMeshes()
{
	if (_meshes.size() == 0)
	{

	}
}
