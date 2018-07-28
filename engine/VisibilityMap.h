#pragma once

#include "GameObject.h"
#include "FOVMap.h"

namespace ok
{
	class VisibilityMap
	{
	public:
		VisibilityMap(int width, int height);
		ok::GameObject& GetPivot(int x, int y);
		void HideAll();
		void ShowAll();
		void UpdateVisibility(ok::FOVMap& fov_map, int x, int y, int radius, int fov_map_left = 0, int fov_map_top = 0);
	protected:
	private:
		std::vector<ok::GameObject> _map;
		int _width;
		int _height;
	};
}