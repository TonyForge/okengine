#include "Maze.h"

ok::GameObject * Kripta::MazeDecoration::Duplicate(ok::GameObject * _clone)
{
	Kripta::MazeDecoration* duplicate = new Kripta::MazeDecoration(*static_cast<Kripta::MazeDecoration*>(_clone));
	return _clone;
}
