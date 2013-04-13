#ifndef SDLGLUTILS_H
#define SDLGLUTILS_H

#include <GL/glew.h>
#include <SDL/SDL.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// This file (and the .cpp one) contains parts from various websites

// Taken from sdl documentation ;)
Uint32 SDL_GetPixel(SDL_Surface *surface, int x, int y);

// These ones are from the opengl tutorial on siteduzero.com
GLuint loadTexture(const char * filename);
int takeScreenshot(const char * filename);

// From opengl-tutorial.com
GLuint loadShaders(
	const char * vertex_file_path,
	const char * fragment_file_path);

#endif //SDLGLUTILS_H
