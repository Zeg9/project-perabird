#ifndef SDLGLUTILS_H
#define SDLGLUTILS_H

#include <GL/gl.h>
#include <SDL/SDL.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// Taken from sdl documentation ;)
Uint32 SDL_GetPixel(SDL_Surface *surface, int x, int y);

// These ones are from the opengl tutorial on siteduzero.com
GLuint loadTexture(const char * filename);
int takeScreenshot(const char * filename);

#endif //SDLGLUTILS_H
