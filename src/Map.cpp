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
#include "Mesh.h"


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

double Map::terrainHeight(int _x, int _y)
{
	int x(_x),y(_y);
	if (x < 0) x=0;
	if (y < 0) y=0;
	if (x >= getSize().x) x = getSize().x-1;
	if (y >= getSize().y) y = getSize().y-1;
	Uint8 r,g,b;
	SDL_GetRGB(
		SDL_GetPixel(terrain,x,y),
		terrain->format,
		&r,&g,&b);
	return (double)(r)/255.0f*MAX_HEIGHT;
}

double Map::terrainHeightf(double x, double y)
{
	// This function is just doing bilinear interpolation
	int x1 = (int)x;
	int y1 = (int)y;
	int x2 = x1+1;
	int y2 = y1+1;
	double p11 = terrainHeight(x1,y1);
	double p21 = terrainHeight(x2,y1);
	double p12 = terrainHeight(x1,y2);
	double p22 = terrainHeight(x2,y2);
	double o = 
		(1.0/((x2-x1)*(y2-y1))) * (
			p11*(x2-x)*(y2-y) +
			p21*(x-x1)*(y2-y) +
			p12*(x2-x)*(y-y1) +
			p22*(x-x1)*(y-y1)
		);
	return o;
}

unsigned int Map::getMeshSize()
{
	return 2*3*getSize().x*getSize().y;
}

void Map::renderMesh(Mesh &map_mesh, GLuint map_programID)
{
	map_mesh.texture = loadTexture(getPath("grass.png"));
	map_mesh.programID = map_programID;
	GLfloat *vertices = map_mesh.vertices;
	GLfloat *uvs = map_mesh.uvs;

	unsigned int current_quad = 0; // 1 quad = 2 triangles = 6 vertices (let's say)
	for (float x = 0; x < getSize().x; x+=1)
	for (float y = 0; y < getSize().y; y+=1)
	{
		float z00, z10, z01, z11;
		Uint8 r,g,b;
		z00 = terrainHeightf(x,y);
		if (x+1 < getSize().x) {
			z10 = terrainHeightf(x+1,y);
		} else z10 = z00;
		if (y+1 < getSize().y) {
			z01 = terrainHeightf(x,y+1);
		} else z01 = z00;
		if (x+1 < getSize().x && y+1 < getSize().y) {
			z11 = terrainHeightf(x+1,y+1);
		} else if (z01 != z00) z11=z01;
		  else if (z10 != z00) z11=z10;
		  else z11=z00;
		unsigned int v = current_quad*2*3*3;
		vertices[v] = x;      vertices[v+1] = z00;  vertices[v+2] = y;
		vertices[v+3] = x;    vertices[v+4] = z01;  vertices[v+5] = y+1;
		vertices[v+6] = x+1;  vertices[v+7] = z10;  vertices[v+8] = y;
		vertices[v+9] = x+1;  vertices[v+10] = z11; vertices[v+11] = y+1;
		vertices[v+12] = x+1; vertices[v+13] = z10; vertices[v+14] = y;
		vertices[v+15] = x;   vertices[v+16] = z01; vertices[v+17] = y+1;
		unsigned int u = current_quad*2*3*2;
		float s = 5; // uv Scale
		uvs[u] = x*s; uvs[u+1] = y*s;
		uvs[u+2] = x*s; uvs[u+3] = (y+1)*s;
		uvs[u+4] = (x+1)*s; uvs[u+5] = y*s;
		uvs[u+6] = (x+1)*s; uvs[u+7] = (y+1)*s;
		uvs[u+8] = (x+1)*s; uvs[u+9] = y*s;
		uvs[u+10] = x*s; uvs[u+11] = (y+1)*s;
		current_quad ++;
	}
	map_mesh.updateBuffers();
}

void Map::waterMesh(Mesh &water_mesh, GLuint water_programID)
{

	int w(getSize().x),h(getSize().y),y(WATER_HEIGHT);
	water_mesh.texture = loadTexture(getPath("water.png"));
	water_mesh.programID = water_programID;
	GLfloat *vertices = water_mesh.vertices;
	GLfloat *uvs = water_mesh.uvs;
	vertices[0] = 0;  vertices[1] = y;  vertices[2] = 0;
	vertices[3] = 0;  vertices[4] = y;  vertices[5] = h;
	vertices[6] = w;  vertices[7] = y;  vertices[8] = 0;
	vertices[9] = w;  vertices[10] = y; vertices[11] = h;
	vertices[12] = w; vertices[13] = y; vertices[14] = 0;
	vertices[15] = 0; vertices[16] = y; vertices[17] = h;
	
	vertices[18] = 0; vertices[19] = y;  vertices[20] = 0;
	vertices[21] = w; vertices[25] = y; vertices[23] = 0;
	vertices[24] = 0; vertices[22] = y; vertices[26] = h;
	vertices[27] = w; vertices[28] = y; vertices[29] = h;
	vertices[30] = 0; vertices[34] = y; vertices[32] = h;
	vertices[33] = w; vertices[31] = y; vertices[35] = 0;
	
	uvs[0] = 0;  uvs[1] = 0;
	uvs[2] = 0;  uvs[3] = h;
	uvs[4] = w;  uvs[5] = 0;
	uvs[6] = w;  uvs[7] = h;
	uvs[8] = w;  uvs[9] = 0;
	uvs[10] = 0; uvs[11] = h;
	
	uvs[12] = 0; uvs[13] = 0;
	uvs[14] = w; uvs[15] = 0;
	uvs[16] = 0; uvs[17] = h;
	uvs[18] = w; uvs[19] = h;
	uvs[20] = 0; uvs[21] = h;
	uvs[22] = w; uvs[23] = 0;
	water_mesh.updateBuffers();
}


