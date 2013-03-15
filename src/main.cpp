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
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "sdlglutils.h"
#include "Resources.h"
#include "Mesh.h"
#include "Map.h"

#define deg2rad(x) 0.017453293*x

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

int main(int argc, char**argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("Perabird",0);
	//SDL_WM_GrabInput(SDL_GRAB_ON);
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
	glClearColor(.4,.4,1,0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// Vertex array
	GLuint vaID;
	glGenVertexArrays(1,&vaID);
	glBindVertexArray(vaID);
	// Load map
	Map map("test");
	glm::vec2 map_size = map.getSize();
	Mesh map_mesh(2*3*map_size.x*map_size.y);  // 2 triangles per pixel
	map_mesh.texture = loadTexture(getPath("grass.png"));
	GLfloat *vertices = map_mesh.vertices;
	GLfloat *uvs = map_mesh.uvs;
	
	unsigned int current_quad = 0; // 1 quad = 2 triangles = 6 vertices (let's say)
	for (unsigned int x = 0; x < map_size.x; x++)
	for (unsigned int y = 0; y < map_size.y; y++)
	{
		float z00, z10, z01, z11;
		Uint8 r,g,b;
		z00 = map.terrainHeight(x,y);
		if (x+1 < map_size.x) {
			z10 = map.terrainHeight(x+1,y);
		} else z10 = z00;
		if (y+1 < map_size.y) {
			z01 = map.terrainHeight(x,y+1);
		} else z01 = z00;
		if (x+1 < map_size.x && y+1 < map_size.y) {
			z11 = map.terrainHeight(x+1,y+1);
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
	
	
	GLuint programID = LoadShaders(
		getPath("vertex.glsl"),
		getPath("fragment.glsl")
	);
	
	
	glm::vec3 position(0,0,3);
	double mspeed(.05), speed(0),sspeed(0);
	float rx(0), ry(0);
	SDL_Event e; bool done = false;
	while (!done)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		
		glm::vec3 direction(
			sin(deg2rad(rx)),
			0,
			cos(deg2rad(rx))
		);
		position += direction*glm::vec3(speed);
		position.y --;
		glm::vec3 right = glm::vec3(
			sin(deg2rad(rx)-M_PI/2.0f),
			0,
			cos(deg2rad(rx)-M_PI/2.0f)
		);
		position += right*glm::vec3(sspeed);
		position.y = map.terrainHeightf(position.x,position.z)+.5; // TODO improve...
		
		glm::mat4 projection = glm::perspective(60.0f,4.0f/3.0f,.1f,100.0f);
		glm::mat4 view(1.0f);
		view = glm::rotate(view, -ry, glm::vec3(1,0,0));
		view = glm::rotate(view, 180-rx, glm::vec3(0,1,0));
		view = glm::translate(view, -position);
		glm::mat4 model;
		glm::mat4 mvp = projection*view*model;
		map_mesh.render(programID,mvp);
	
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
							speed = mspeed;
							break;
						case SDLK_s:
							speed = -mspeed;
							break;
						case SDLK_q:
							sspeed = -mspeed;
							break;
						case SDLK_d:
							sspeed = mspeed;
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
						case SDLK_s:
							speed = 0;
							break;
						case SDLK_q:
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
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}



	GLint Result = GL_FALSE;
	int InfoLogLength;



	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

