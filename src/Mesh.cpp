/*
 * Mesh.cpp
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

#include "Mesh.h"
#include <iostream>

Mesh::Mesh(unsigned int vc):vertices_count(vc)
{
	vertices = new GLfloat[vertices_count*3];
	uvs = new GLfloat[vertices_count*2];
	glGenBuffers(1,&vertexbuffer);
	glGenBuffers(1,&uvbuffer);
}
Mesh::~Mesh()
{
	delete[] vertices;
	delete[] uvs;
}

void Mesh::updateBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat)*vertices_count*3, vertices, GL_STATIC_DRAW);
	std::cout << sizeof(vertices) << std::endl;
	
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat)*vertices_count*2, uvs, GL_STATIC_DRAW);
}

void Mesh::render(GLuint programID, glm::mat4 mvp)
{

	GLuint matrixID = glGetUniformLocation(programID, "MVP");
	GLuint textureID = glGetUniformLocation(programID, "textureSampler");
	GLuint vertexPositionID = glGetAttribLocation(programID,"vertexPosition");
	GLuint vertexUVID = glGetAttribLocation(programID,"vertexUV");
	
	glUniformMatrix4fv(matrixID,1,GL_FALSE,&mvp[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureID, 0);
	
	glEnableVertexAttribArray(vertexPositionID);
	glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
	glVertexAttribPointer(vertexPositionID,3,GL_FLOAT,GL_FALSE,0,0);
	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER,uvbuffer);
	glVertexAttribPointer(vertexUVID,2,GL_FLOAT,GL_FALSE,0,0);
	glDrawArrays(GL_TRIANGLES,0,vertices_count);
	glDisableVertexAttribArray(vertexPositionID);
	glDisableVertexAttribArray(vertexUVID);
}

