#include "VisibilityMap.h"

ok::VisibilityMap::VisibilityMap(int width, int height)
{
	_width = width;
	_height = height;
	_map.resize(width * height);

	for (auto& obj : _map)
	{
		obj.EnableOverrideChildrenVisibility();
	}
}

ok::GameObject & ok::VisibilityMap::GetPivot(int x, int y)
{
	static ok::GameObject _dummy;

	if (x < 0 || y < 0 || x >= _width || y >= _height)
	{
		return _dummy;
	}
	
	return _map[x + y * _width];
}

void ok::VisibilityMap::HideAll()
{
	for (auto& obj : _map)
	{
		obj.Hide();
	}
}

void ok::VisibilityMap::ShowAll()
{
	for (auto& obj : _map)
	{
		obj.Show();
	}
}

void ok::VisibilityMap::UpdateVisibility(ok::FOVMap & fov_map, int x, int y, int radius, int fov_map_left, int fov_map_top)
{
	HideAll();

	int inner_x = fov_map_left + x;
	int inner_y = fov_map_top + y;
	int left = inner_x - radius;
	int top = inner_y - radius;
	int width = radius * 2;
	int height = radius * 2;

	for (int iy = 0; iy < height; iy++)
	{
		for (int ix = 0; ix < width; ix++)
		{
			if (true == fov_map.GetFOV(ix, iy))
			{
				GetPivot(left + ix, top + iy).Show();
			}
		}
	}
	
}
