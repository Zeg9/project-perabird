/*
 * Mesh.h
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

class Mesh
{
	public:
		Mesh(unsigned int vc);
		~Mesh();
		void updateBuffers();
		void render(GLuint programID, glm::mat4 mvp);
		
		unsigned int vertices_count;
		GLfloat *vertices; // array of size vertices_count*3
		GLfloat *uvs; // array of size vertices_count*2
		GLuint vertexbuffer;
		GLuint uvbuffer;
		GLuint texture;
		glm::vec4 color;
};
