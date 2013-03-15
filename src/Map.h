/*
 * Map.h
 * This file is part of Perabird
 *
 * Copyright (C) 2013 - Zeg9
 *
 * Perabird is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Perabird is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Perabird. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __MAP_H__
#define __MAP_H__

#include <string>
#include <SDL/SDL.h>
#include <glm/glm.hpp>

class Map
{
	public:
		Map(std::string name);
		~Map();
		glm::vec2 getSize();
		double terrainHeight(int x, int y);
		double terrainHeightf(double x, double y);
	private:
		SDL_Surface *terrain;
};

#endif /* __MAP_H__ */

