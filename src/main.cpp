/*
 * main.cpp
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

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include "utils.h"
#include "sdlglutils.h"
#include "Resources.h"
#include "Mesh.h"
#include "Map.h"

#define deg2rad(x) 0.017453293*x

void letterMesh(Mesh &m, char c)
{
	float x1 = (c%16)/16.0f;
	float y1 = 1-(c/16+1)/16.0f;
	float x2 = x1 + (1.0f/16.0f);
	float y2 = y1 + (1.0f/16.0f);
	GLfloat *uvs = m.uvs;
	uvs[0] = x1;  uvs[1] = y1;
	uvs[2] = x2;  uvs[3] = y1;
	uvs[4] = x1;  uvs[5] = y2;
	uvs[6] = x2;  uvs[7] = y2;
	uvs[8] = x1;  uvs[9] = y2;
	uvs[10] = x2; uvs[11] = y1;
	m.updateBuffers();
}



int main(int argc, char**argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("Perabird",0);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(SDL_DISABLE);
	int width(640), height(480);
	if (SDL_SetVideoMode(width,height,32,SDL_OPENGL|SDL_RESIZABLE) == 0)
	{
		std::cerr << "Couldn't initialize video: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Couldn't initilize GLEW !" << std::endl;
		SDL_Quit();
		return -1;
	}
	
	// OpenGL settings
	glClearColor(.5,.5,1,0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	// Vertex array
	GLuint vaID;
	glGenVertexArrays(1,&vaID);
	glBindVertexArray(vaID);
	// Load shaders
	GLuint simple_programID = loadShaders(
		getPath("shaders/simple/vertex.glsl"),
		getPath("shaders/simple/fragment.glsl")
	);
	GLuint map_programID = loadShaders(
		getPath("shaders/map/vertex.glsl"),
		getPath("shaders/map/fragment.glsl")
	);
	GLuint water_programID = loadShaders(
		getPath("shaders/water/vertex.glsl"),
		getPath("shaders/water/fragment.glsl")
	);
	// Load map
	Map map("test");
	glm::vec2 map_size = map.getSize();
	Mesh map_mesh(map.getMeshSize());  // 2 triangles per pixel
	map.renderMesh(map_mesh,map_programID);
	
	Mesh water_mesh(4*3);
	map.waterMesh(water_mesh,water_programID);
	
	Mesh letter(2*3); // it is a quad so 2 triangles
	{
		int w(16),h(16);
		letter.texture = loadTexture(getPath("FreeMono.png"));
		letter.programID = simple_programID;
		GLfloat *vertices = letter.vertices;
		GLfloat *uvs = letter.uvs;
		vertices[0] = 0;  vertices[1] = 0;  vertices[2] = 0;
		vertices[3] = 1;  vertices[4] = 0;  vertices[5] = 0;
		vertices[6] = 0;  vertices[7] = 1;  vertices[8] = 0;
		vertices[9] = 1;  vertices[10] = 1; vertices[11] = 0;
		vertices[12] = 0; vertices[13] = 1; vertices[14] = 0;
		vertices[15] = 1; vertices[16] = 0; vertices[17] = 0;
		
		uvs[0] = 0;  uvs[1] = 0;
		uvs[2] = 1;  uvs[3] = 0;
		uvs[4] = 0;  uvs[5] = 1;
		uvs[6] = 1;  uvs[7] = 1;
		uvs[8] = 0;  uvs[9] = 1;
		uvs[10] = 1; uvs[11] = 0;
		letter.updateBuffers();
	}
	
	Mesh entry_test(2*3); // it is a quad so 2 triangles
	{
		int w(256),h(32);
		entry_test.texture = loadTexture(getPath("gui/entry.png"));
		entry_test.programID = simple_programID;
		GLfloat *vertices = entry_test.vertices;
		GLfloat *uvs = entry_test.uvs;
		vertices[0] = 0;  vertices[1] = 0;  vertices[2] = 0;
		vertices[3] = w;  vertices[4] = 0;  vertices[5] = 0;
		vertices[6] = 0;  vertices[7] = h;  vertices[8] = 0;
		vertices[9] = w;  vertices[10] = h; vertices[11] = 0;
		vertices[12] = 0; vertices[13] = h; vertices[14] = 0;
		vertices[15] = w; vertices[16] = 0; vertices[17] = 0;
		
		uvs[0] = 0;  uvs[1] = 0;
		uvs[2] = 1;  uvs[3] = 0;
		uvs[4] = 0;  uvs[5] = 1;
		uvs[6] = 1;  uvs[7] = 1;
		uvs[8] = 0;  uvs[9] = 1;
		uvs[10] = 1; uvs[11] = 0;
		entry_test.updateBuffers();
	}
	
	
	glm::vec3 position(map_size.x/2.0f,0.0f,map_size.y/2.0f);
	double mspeed(.05), speed(0),sspeed(0), vspeed(0);
	float rx(0), ry(0);
	SDL_Event e; bool done = false;
	while (!done)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::vec3 direction(
			sin(deg2rad(rx)),
			0,
			cos(deg2rad(rx))
		);
		glm::vec3 right = glm::vec3(
			sin(deg2rad(rx)-M_PI/2.0f),
			0,
			cos(deg2rad(rx)-M_PI/2.0f)
		);
		glm::vec3 newpos = position
			+ direction*glm::vec3(speed)
			+ right*glm::vec3(sspeed)
			+ glm::vec3(0,vspeed,0);
		if(newpos.y < map.terrainHeightf(newpos.x,newpos.z)+.5)
		{
			float f = map.terrainHeightf(newpos.x,newpos.z)+.5-newpos.y;
			f = .025/f;
			if (f < 1)
			{
				newpos = position
				+ direction*glm::vec3(speed*f)
				+ right*glm::vec3(sspeed*f)
				+ glm::vec3(0,vspeed*f,0); // slow down when climbing
			}
		} else if (
			vspeed <= 0
			&& position.y <= map.terrainHeightf(position.x,position.z)+.6
			&& newpos.y > map.terrainHeightf(newpos.x,newpos.z)+.5)
				newpos.y = map.terrainHeightf(newpos.x,newpos.z)+.5; // if on ground, stay on ground
		if (newpos.x < 0) newpos.x = 0;
		if (newpos.x >= map_size.x) newpos.x = map_size.x;
		if (newpos.z < 0) newpos.z = 0;
		if (newpos.z >= map_size.y) newpos.z = map_size.y;
		position = newpos;
		if(position.y < map.terrainHeightf(position.x,position.z)+.5)
		{
			position.y = map.terrainHeightf(position.x,position.z)+.5;
			vspeed = 0;
		}
		else if (vspeed > -10)
			vspeed -= mspeed*.05;
		
		// Render
		glm::mat4 projection = glm::perspective(60.0f,4.0f/3.0f,.1f,1000.0f);
		glm::mat4 view(1.0f);
		view = glm::rotate(view, -ry, glm::vec3(1,0,0));
		view = glm::rotate(view, 180-rx, glm::vec3(0,1,0));
		view = glm::translate(view, -position);
		glm::mat4 model(1.0f);
		glm::mat4 mvp = projection*view*model;
		map_mesh.render(mvp);
		glEnable(GL_BLEND);
		water_mesh.render(mvp);
		glDisable(GL_BLEND);
		
		projection = glm::ortho(0.0f,(float)width,0.0f,(float)height,0.0f,1.0f);
		view = glm::mat4(1.0f);
		view = glm::scale(view,glm::vec3(32.0f));
		model = glm::mat4(1.0f);
		mvp = projection*view*model;
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		std::string text = "Pos="
			+toString(position.x)+", "
			+toString(position.y)+", "
			+toString(position.z);
		for (unsigned int i = 0; i < text.size(); i++)
		{
			letterMesh(letter,text[i]);
			letter.color = glm::vec4(glm::vec3(0.0f),1.0f);
			letter.render(mvp);
			model = glm::translate(model,glm::vec3(0.5f,0.0f,0.0f));
			mvp = projection*view*model;
		}
		view = glm::mat4(1.0f);
		model = glm::mat4(1.0f);
		mvp = projection*view*model;
		//entry_test.render(mvp);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	
		SDL_GL_SwapBuffers();
		
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
					done = true;
					break;
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym)
					{
						case SDLK_z:
						case SDLK_w:
							speed = mspeed;
							break;
						case SDLK_s:
							speed = -mspeed;
							break;
						case SDLK_q:
						case SDLK_a:
							sspeed = -mspeed;
							break;
						case SDLK_d:
							sspeed = mspeed;
							break;
						case SDLK_SPACE:
							if (position.y <= WATER_HEIGHT+.6 || // allow swimming
								position.y <= map.terrainHeightf(position.x,position.z)+.6) // jump if you touch the ground
								vspeed = mspeed;
							break;
						case SDLK_ESCAPE:
							done = true;
							break;
						default: break;
					} break;
				case SDL_KEYUP:
					switch(e.key.keysym.sym)
					{
						case SDLK_z:
						case SDLK_w:
						case SDLK_s:
							speed = 0;
							break;
						case SDLK_q:
						case SDLK_a:
						case SDLK_d:
							sspeed = 0;
							break;
						default: break;
					} break;
				case SDL_MOUSEMOTION:
					if (e.motion.x != width/2 && e.motion.y != height/2)
					{
						float nrx(rx), nry(ry);
						nrx -= (float)(e.motion.xrel)/5;
						if (nrx < 0) nrx = 360+nrx;
						if (nrx > 360) nrx = nrx-360;
						rx = nrx;
						nry -= (float)(e.motion.yrel)/5;
						if (nry < -90)
							ry = -90;
						else if (nry > 90)
							ry = 90;
						else
							ry = nry;
						SDL_WarpMouse(width/2,height/2);
					}
					break;
				case SDL_VIDEORESIZE:
					width = e.resize.w;
					height = e.resize.h;
					SDL_SetVideoMode(width,height,32,SDL_OPENGL|SDL_RESIZABLE);
					glViewport(0,0,width,height);
					break;
				default: break;
			}
		}
	}
	SDL_Quit();
}

// TODO move this to a separate file, render code too

