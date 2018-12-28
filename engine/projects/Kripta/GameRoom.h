#pragma once

namespace Kripta
{
	class IObject;

	class GameRoom
	{
	public:
		int tiles_layer_0[100 * 100];
		int tiles_layer_1[100 * 100];
		int tiles_layer_2[100 * 100];
		Kripta::IObject* objects_grid_ground[100 * 100];
		Kripta::IObject* objects_grid[100 * 100];
		void Reset();
	private:
	protected:
	};
}