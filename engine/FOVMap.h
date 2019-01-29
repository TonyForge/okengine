#pragma once

namespace ok
{
	class FOVMap;

	class FOVMapCell
	{
	public:
		FOVMapCell() : _fov(false), _transparent(false), _walkable(false) {}
		bool IsFov();
		bool IsTransparent();
		bool IsWalkable();
	private:
		friend class ok::FOVMap;
		bool _fov;
		bool _transparent;
		bool _walkable;
	};

	class FOVMap
	{
	public:
		FOVMap(int size_x, int size_y);
		void Resize(int size_x, int size_y);
		void SetWalkable(int x, int y, bool walkable);
		void SetTransparent(int x, int y, bool transparent);
		bool GetFOV(int x, int y);
		ok::FOVMapCell& Get(int x, int y);
		void CalculateFOV(int x, int y, int radius, bool allow_fov_walls);
		void Copy(int from_x, int from_y, ok::FOVMap& to);
		void Clear(bool walkable = true, bool transparent = true, bool fov = true);
	protected:
	private:
		std::vector<ok::FOVMapCell> map;
		std::vector<ok::FOVMapCell>* map_ptr;
		int width;
		int height;

		void _ComputeFOV(int x, int y, int radius, bool allow_fov_walls);
		void _ComputeFOV_Quadrant(int x, int y, int radius, bool allow_fov_walls, int dx, int dy);

		double * start_angle = NULL;
		double * end_angle = NULL;
		int allocated = 0;
	};
}