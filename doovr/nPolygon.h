#pragma once
#include "Utilities.h"

#include <iostream>
#include <math.h>


class nPolygon
{
  public:
	  nPolygon();
	  ~nPolygon();

	float* getPosition(){ return position; }
	void setPosition(float* p) { position[0] = p[0]; position[1] = p[1]; position[2] = p[2]; }
	float* getOrientation(){ return orientation; }
	void setOrientation(float* o) {
		orientation[0] = o[0]; orientation[1] = o[1]; orientation[2] = o[2]; orientation[3] = o[3];
		orientation[4] = o[4]; orientation[5] = o[5]; orientation[6] = o[6]; orientation[7] = o[7];
		orientation[8] = o[8]; orientation[9] = o[9]; orientation[10] = o[10]; orientation[11] = o[11];
		orientation[12] = o[12]; orientation[13] = o[13]; orientation[14] = o[14]; orientation[15] = o[15];
	}

	virtual void render() = 0;

  protected:
	float position[3];
	float orientation[16];

	GLuint vao;				// Vertex array object, the main handle for geometry
	int nverts;				// Number of vertices in the vertex array
	int nlines;				// Number of lines in the index array (may be zero)
	GLuint vertexbuffer;	// Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;		// Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
	GLfloat *vertexarray;	// Vertex array on interleaved format: x y z nx ny nz s t
	GLuint *indexarray;		// Element index array
};

