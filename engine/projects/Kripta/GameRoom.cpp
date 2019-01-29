#include "GameRoom.h"

void Kripta::GameRoom::Reset()
{
	memset(objects_grid_ground_special, 0, sizeof(Kripta::IObject*) * 100 * 100);
	memset(objects_grid_ground, 0, sizeof(Kripta::IObject*) * 100 * 100);
	memset(objects_grid, 0, sizeof(Kripta::IObject*) * 100 * 100);
	memset(tiles_layer_0, 0, sizeof(int) * 100 * 100);
	memset(tiles_layer_1, 0, sizeof(int) * 100 * 100);
	memset(tiles_layer_2, 0, sizeof(int) * 100 * 100);

}
