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

#define getPath(x) "../data/" x // TODO make a resource management file

#define deg2rad(x) 0.017453293*x

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

int main()
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
	
	// TESTING 190 or 209
	SDL_Surface*map_img = IMG_Load(getPath("map_1.png"));
	Uint8 r,g,b;
	SDL_GetRGB(SDL_GetPixel(map_img,10,10),map_img->format,&r,&g,&b);
	std::cout << (r==g && g==b) << ',' << (int)r << std::endl;
	// END TESTING
	
	glClearColor(0,0,0.2,0);
	glEnable(GL_CULL_FACE);
	GLuint vaID;
	glGenVertexArrays(1,&vaID);
	glBindVertexArray(vaID);
	
	static GLfloat mesh[] = {
		0,  1,  0,
		-1, -1, 0,
		1,  -1, 0,
	};
	GLuint vertexbuffer;
	glGenBuffers(1,&vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);
	
	static const GLfloat colors[] = {
		1,0,0,
		0,1,0,
		0,0,1,
	};
	GLuint colorbuffer;
	glGenBuffers(1,&colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	
	static const GLfloat uvs[] = {
		.5, 1,
		0, 0,
		1, 0,
	};
	GLuint uvbuffer;
	glGenBuffers(1,&uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
	
	
	GLuint programID = LoadShaders(
		getPath("vertex.glsl"),
		getPath("fragment.glsl")
	);
	
	GLuint matrixID = glGetUniformLocation(programID, "MVP");
	GLuint vertexPositionID = glGetAttribLocation(programID,"vertexPosition");
	GLuint vertexColorID = glGetAttribLocation(programID,"vertexColor");
	GLuint vertexUVID = glGetAttribLocation(programID,"vertexUV");
	
	GLuint texture = loadTexture(getPath("test.png"));
	
	glm::vec3 position(0,0,3);
	double speed(0),sspeed(0);
	float rx(0), ry(0);
	SDL_Event e; bool done = false;
	while (!done)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		
		glm::vec3 direction(
			cos(deg2rad(ry)) * sin(deg2rad(rx)),
			sin(deg2rad(ry)),
			cos(deg2rad(ry)) * cos(deg2rad(rx))
		);
		position += direction*glm::vec3(speed);
		glm::vec3 right = glm::vec3(
			sin(deg2rad(rx)-M_PI/2.0f),
			0,
			cos(deg2rad(rx)-M_PI/2.0f)
		);
		position += right*glm::vec3(sspeed);
		
		glm::mat4 projection = glm::perspective(60.0f,4.0f/3.0f,.1f,100.0f);
		glm::mat4 view = glm::lookAt(
			position,
			position+direction,
			glm::vec3(0.0f,1.0f,0.0f)
		);
		/*view = glm::rotate(view,ry, glm::vec3(1.0f,0.0f,0.0f));
		view = glm::rotate(view, rx, glm::vec3(0.0f,1.0f,0.0f));
		view = glm::translate(view,-position);*/
		glm::mat4 model;
		glm::mat4 mvp = projection*view*model;
		glUniformMatrix4fv(matrixID,1,GL_FALSE,&mvp[0][0]);
	
		glEnableVertexAttribArray(vertexPositionID);
		glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
		glVertexAttribPointer(vertexPositionID,3,GL_FLOAT,GL_FALSE,0,0);
		glEnableVertexAttribArray(vertexColorID);
		glBindBuffer(GL_ARRAY_BUFFER,colorbuffer);
		glVertexAttribPointer(vertexColorID,3,GL_FLOAT,GL_FALSE,0,0);
		glEnableVertexAttribArray(vertexUVID);
		glBindBuffer(GL_ARRAY_BUFFER,uvbuffer);
		glVertexAttribPointer(vertexUVID,2,GL_FLOAT,GL_FALSE,0,0);
		glDrawArrays(GL_TRIANGLES,0,3);
		glDisableVertexAttribArray(vertexPositionID);
		glDisableVertexAttribArray(vertexColorID);
		glDisableVertexAttribArray(vertexUVID);
	
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
							speed = 1;
							break;
						case SDLK_s:
							speed = -1;
							break;
						case SDLK_q:
							sspeed = -1;
							break;
						case SDLK_d:
							sspeed = 1;
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

