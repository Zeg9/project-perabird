/*
 * Map.cpp
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

#include "Map.h"

#include <SDL/SDL_image.h>

#include "Resources.h"
#include "sdlglutils.h"

#define MAX_HEIGHT 15

Map::Map(std::string name)
{
	terrain = IMG_Load((getPath("maps/"+name+"/terrain.png")).c_str());
}
Map::~Map()
{
	SDL_FreeSurface(terrain);
	// TODO When resource manager is done, this may cause segfaults.
}

glm::vec2 Map::getSize()
{
	return glm::vec2(terrain->w, terrain->h);
}

double Map::terrainHeight(int x, int y)
{
	if (x < 0 || y < 0 || x >= getSize().x || y >= getSize().y) return 0;
	Uint8 r,g,b;
	SDL_GetRGB(SDL_GetPixel(terrain,x,y),terrain->format,&r,&g,&b);
	return (double)(r)/255.0f*MAX_HEIGHT;
}


