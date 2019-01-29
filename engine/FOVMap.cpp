/*
* libtcod
* Copyright (c) 2008-2018 Jice & Mingos & rmtew
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "FOVMap.h"

void ok::FOVMap::_ComputeFOV(int x, int y, int radius, bool allow_fov_walls)
{
	std::vector<ok::FOVMapCell>& m = *map_ptr;

	int max_obstacles;

	/* first, zero the FOV map */
	for (auto& cell : m)
	{
		cell._fov = false;
	}

	/* calculate an approximated (excessive, just in case) maximum number of obstacles per octant */
	max_obstacles = (width*height) / 7;

	/* check memory for angles */
	if (max_obstacles > allocated) {
		allocated = max_obstacles;
		if (start_angle != NULL) free(start_angle);
		if (end_angle != NULL) free(end_angle);
		start_angle = (double*)calloc(max_obstacles, sizeof(double));
		end_angle = (double*)calloc(max_obstacles, sizeof(double));
	}

	/* set PC's position as visible */
	m[x + (y*width)]._fov = true;

	/* compute the 4 quadrants of the map */
	_ComputeFOV_Quadrant(x, y, radius, allow_fov_walls, 1, 1);
	_ComputeFOV_Quadrant(x, y, radius, allow_fov_walls, 1, -1);
	_ComputeFOV_Quadrant(x, y, radius, allow_fov_walls, -1, 1);
	_ComputeFOV_Quadrant(x, y, radius, allow_fov_walls, -1, -1);
}

void ok::FOVMap::_ComputeFOV_Quadrant(int x, int y, int radius, bool allow_fov_walls, int dx, int dy)
{
	std::vector<ok::FOVMapCell>& m = *map_ptr;

	/* octant: vertical edge */
	{
		int iteration = 1; /* iteration of the algo for this octant */
		bool done = false;
		int total_obstacles = 0;
		int obstacles_in_last_line = 0;
		double min_angle = 0.0;
		int ix;
		int iy;

		/* do while there are unblocked slopes left and the algo is within the map's boundaries
		scan progressive lines/columns from the PC outwards */
		iy = y + dy; /* the outer slope's coordinates (first processed line) */
		if (iy < 0 || iy >= height) {
			done = true;
		}
		while (!done) {
			/* process cells in the line */
			double slopes_per_cell = 1.0 / (double)(iteration);
			double half_slopes = slopes_per_cell * 0.5;
			int processed_cell = (int)((min_angle + half_slopes) / slopes_per_cell);
			int minx = glm::max(0, x - iteration);
			int maxx = glm::min(width - 1, x + iteration);
			done = true;
			for (ix = x + (processed_cell * dx); ix >= minx && ix <= maxx; ix += dx) {
				int c = ix + (iy * width);
				/* calculate slopes per cell */
				bool visible = true;
				bool extended = false;
				double centre_slope = (double)processed_cell * slopes_per_cell;
				double start_slope = centre_slope - half_slopes;
				double end_slope = centre_slope + half_slopes;
				if (obstacles_in_last_line > 0) {
					if (
						!(
							m[c - (width * dy)]._fov &&
							m[c - (width * dy)]._transparent
							) &&
						!(
							m[c - (width * dy) - dx]._fov &&
							m[c - (width * dy) - dx]._transparent
							)
						) {
						visible = false;
					}
					else {
						int idx;
						for (idx = 0; idx < obstacles_in_last_line && visible; ++idx) {
							if (
								start_slope <= end_angle[idx] &&
								end_slope >= start_angle[idx]
								) {
								if (m[c]._transparent) {
									if (
										centre_slope > start_angle[idx] &&
										centre_slope < end_angle[idx]
										) {
										visible = false;
									}
								}
								else {
									if (
										start_slope >= start_angle[idx] &&
										end_slope <= end_angle[idx]
										) {
										visible = false;
									}
									else {
										start_angle[idx] = glm::min(start_angle[idx], start_slope);
										end_angle[idx] = glm::max(end_angle[idx], end_slope);
										extended = true;
									}
								}
							}
						}
					}
				}
				if (visible) {
					done = false;
					m[c]._fov = true;
					/* if the cell is opaque, block the adjacent slopes */
					if (!m[c]._transparent) {
						if (min_angle >= start_slope) {
							min_angle = end_slope;
							/* if min_angle is applied to the last cell in line, nothing more
							needs to be checked. */
							if (processed_cell == iteration) {
								done = true;
							}
						}
						else if (!extended) {
							start_angle[total_obstacles] = start_slope;
							end_angle[total_obstacles++] = end_slope;
						}
						if (!allow_fov_walls) {
							m[c]._fov = false;
						}
					}
				}
				processed_cell++;
			}
			if (iteration == radius) {
				done = true;
			}
			iteration++;
			obstacles_in_last_line = total_obstacles;
			iy += dy;
			if (iy < 0 || iy >= height) {
				done = true;
			}
		}
	}

	/* octant: horizontal edge */
	{
		int iteration = 1; /* iteration of the algo for this octant */
		bool done = false;
		int total_obstacles = 0;
		int obstacles_in_last_line = 0;
		double min_angle = 0.0;
		int ix;
		int iy;

		/* do while there are unblocked slopes left and the algo is within the map's boundaries
		scan progressive lines/columns from the PC outwards */
		ix = x + dx; /*the outer slope's coordinates (first processed line) */
		if (ix < 0 || ix >= width) {
			done = true;
		}
		while (!done) {
			/* process cells in the line */
			double slopes_per_cell = 1.0 / (double)(iteration);
			double half_slopes = slopes_per_cell * 0.5;
			int processed_cell = (int)((min_angle + half_slopes) / slopes_per_cell);
			int miny = glm::max(0, y - iteration);
			int maxy = glm::min(height - 1, y + iteration);
			done = true;
			for (iy = y + (processed_cell * dy); iy >= miny && iy <= maxy; iy += dy) {
				int c = ix + (iy * width);
				/* calculate slopes per cell */
				bool visible = true;
				bool extended = false;
				double centre_slope = (double)processed_cell * slopes_per_cell;
				double start_slope = centre_slope - half_slopes;
				double end_slope = centre_slope + half_slopes;
				if (obstacles_in_last_line > 0) {
					if (
						!(
							m[c - dx]._fov &&
							m[c - dx]._transparent
							) &&
						!(
							m[c - (width * dy) - dx]._fov &&
							m[c - (width * dy) - dx]._transparent
							)
						) {
						visible = false;
					}
					else {
						int idx;
						for (idx = 0; idx < obstacles_in_last_line && visible; ++idx) {
							if (
								start_slope <= end_angle[idx] &&
								end_slope >= start_angle[idx]
								) {
								if (m[c]._transparent) {
									if (
										centre_slope > start_angle[idx] &&
										centre_slope < end_angle[idx]
										) {
										visible = false;
									}
								}
								else {
									if (
										start_slope >= start_angle[idx] &&
										end_slope <= end_angle[idx]
										) {
										visible = false;
									}
									else {
										start_angle[idx] = glm::min(start_angle[idx], start_slope);
										end_angle[idx] = glm::max(end_angle[idx], end_slope);
										extended = true;
									}
								}
								++idx;
							}
						}
					}
				}
				if (visible) {
					done = false;
					m[c]._fov = true;
					/* if the cell is opaque, block the adjacent slopes */
					if (!m[c]._transparent) {
						if (min_angle >= start_slope) {
							min_angle = end_slope;
							/* if min_angle is applied to the last cell in line, nothing more
							needs to be checked. */
							if (processed_cell == iteration) {
								done = true;
							}
						}
						else if (!extended) {
							start_angle[total_obstacles] = start_slope;
							end_angle[total_obstacles++] = end_slope;
						}
						if (!allow_fov_walls) {
							m[c]._fov = false;
						}
					}
				}
				processed_cell++;
			}
			if (iteration == radius) {
				done = true;
			}
			iteration++;
			obstacles_in_last_line = total_obstacles;
			ix += dx;
			if (ix < 0 || ix >= width) {
				done = true;
			}
		}
	}
}

ok::FOVMap::FOVMap(int size_x, int size_y)
{
	map_ptr = &map;
	map.resize(size_x*size_y);
	width = size_x;
	height = size_y;
	Clear();
}

void ok::FOVMap::Resize(int size_x, int size_y)
{
	map.resize(size_x*size_y);
	width = size_x;
	height = size_y;
	Clear();
}

void ok::FOVMap::SetWalkable(int x, int y, bool walkable)
{
	Get(x, y)._walkable = walkable;
}

void ok::FOVMap::SetTransparent(int x, int y, bool transparent)
{
	Get(x, y)._transparent = transparent;
}

bool ok::FOVMap::GetFOV(int x, int y)
{
	return Get(x, y)._fov;
}

ok::FOVMapCell& ok::FOVMap::Get(int x, int y)
{
	static ok::FOVMapCell _dummy;

	if (x >= 0 && y >= 0 && x < width && y < height)
	{
		return map[x + y * width];
	}

	return _dummy;
}

void ok::FOVMap::CalculateFOV(int x, int y, int radius, bool allow_fov_walls)
{
	_ComputeFOV(x, y, radius, allow_fov_walls);
}

void ok::FOVMap::Copy(int from_x, int from_y, ok::FOVMap & to)
{
	for (int y = 0; y < to.height; y++)
	{
		for (int x = 0; x < to.width; x++)
		{
			to.map[x + y * to.width] = Get(from_x + x, from_y + y);
		}
	}

}

void ok::FOVMap::Clear(bool walkable, bool transparent, bool fov)
{
	for (auto& cell : map)
	{
		cell._fov = fov;
		cell._walkable = walkable;
		cell._transparent = transparent;
	}
}

bool ok::FOVMapCell::IsFov()
{
	return _fov;
}

bool ok::FOVMapCell::IsTransparent()
{
	return _transparent;
}

bool ok::FOVMapCell::IsWalkable()
{
	return _walkable;
}
