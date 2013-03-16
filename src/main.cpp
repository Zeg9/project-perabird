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
#include "utils.h"
#include "sdlglutils.h"
#include "Resources.h"
#include "Mesh.h"
#include "Map.h"

#define deg2rad(x) 0.017453293*x
#define WATER_HEIGHT 8

void letterMesh(Mesh &m, char c)
{
	float x1 = (c%16)/16.0f;
	float y1 = 1-(c/16+1)/16.0f;
	float x2 = x1 + (16.0f/256.0f);
	float y2 = y1 + (16.0f/256.0f);
	GLfloat *uvs = m.uvs;
	uvs[0] = x1;  uvs[1] = y1;
	uvs[2] = x2;  uvs[3] = y1;
	uvs[4] = x1;  uvs[5] = y2;
	uvs[6] = x2;  uvs[7] = y2;
	uvs[8] = x1;  uvs[9] = y2;
	uvs[10] = x2; uvs[11] = y1;
	m.updateBuffers();
}

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	// Vertex array
	GLuint vaID;
	glGenVertexArrays(1,&vaID);
	glBindVertexArray(vaID);
	// Load shaders
	GLuint simple_programID = LoadShaders(
		getPath("shaders/simple/vertex.glsl"),
		getPath("shaders/simple/fragment.glsl")
	);
	GLuint map_programID = LoadShaders(
		getPath("shaders/map/vertex.glsl"),
		getPath("shaders/map/fragment.glsl")
	);
	GLuint water_programID = LoadShaders(
		getPath("shaders/water/vertex.glsl"),
		getPath("shaders/water/fragment.glsl")
	);
	// Load map
	Map map("test");
	glm::vec2 map_size = map.getSize();
	Mesh map_mesh(2*3*map_size.x*map_size.y);  // 2 triangles per pixel
	{
		map_mesh.texture = loadTexture(getPath("grass.png"));
		map_mesh.programID = map_programID;
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
	}
	
	Mesh water_mesh(4*3);
	{
		int w(map_size.x),h(map_size.y),y(WATER_HEIGHT);
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
	
	Mesh letter(2*3); // it is a quad so 2 triangles
	{
		int w(16),h(16);
		letter.texture = loadTexture(getPath("FreeMono15.png"));
		letter.programID = simple_programID;
		GLfloat *vertices = letter.vertices;
		GLfloat *uvs = letter.uvs;
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
		position += direction*glm::vec3(speed);
		glm::vec3 right = glm::vec3(
			sin(deg2rad(rx)-M_PI/2.0f),
			0,
			cos(deg2rad(rx)-M_PI/2.0f)
		);
		position += right*glm::vec3(sspeed);
		if (vspeed > -10)
			vspeed -= mspeed*.5;
		position.y += vspeed;
		if(position.y < map.terrainHeightf(position.x,position.z)+.5)
			position.y = map.terrainHeightf(position.x,position.z)+.5;
		
		glm::mat4 projection = glm::perspective(60.0f,4.0f/3.0f,.1f,100.0f);
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
		view = glm::scale(view,glm::vec3(2.0f));
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
			model = glm::translate(model,glm::vec3(8.0f,0.0f,0.0f));
			mvp = projection*view*model;
		}
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
							vspeed = mspeed*5;
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

