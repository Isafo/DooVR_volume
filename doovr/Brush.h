#pragma once
#include "Utilities.h"

#include <iostream>
#include <math.h>


class Brush
{
  public:
	Brush();
	~Brush();

	float* getPosition(){ return position; }
	void setPosition(float* p) { position[0] = p[0]; position[1] = p[1]; position[2] = p[2]; }

	virtual void render() = 0;

  protected:
	float position[3];

	GLuint vao;				// Vertex array object, the main handle for geometry
	int nverts;				// Number of vertices in the vertex array
	int nlines;				// Number of lines in the index array (may be zero)
	GLuint vertexbuffer;	// Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;		// Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
	GLfloat *vertexarray;	// Vertex array on interleaved format: x y z nx ny nz s t
	GLuint *indexarray;		// Element index array
};

