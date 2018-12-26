#include "GameRoom.h"

void Kripta::GameRoom::Reset()
{
	memset(tiles_layer_0, 0, sizeof(int) * 100 * 100);
	memset(tiles_layer_1, 0, sizeof(int) * 100 * 100);
	memset(tiles_layer_2, 0, sizeof(int) * 100 * 100);

}
