#pragma once

#include "Entity.h"
#include "Utilities.h"

class MenuItem : public Entity {
  public:
	MenuItem() {
		vao = 0;
		vertexbuffer = 0;
		indexbuffer = 0;
		vertexarray = NULL;
		indexarray = NULL;
		nverts = 0;
		ntris = 0;
		dim[0] = 0;
		dim[1] = 0;
	};

	~MenuItem(void);

	MenuItem(float x, float y, float z, float dX, float dZ, int tX, int tY, int tXLength, int tYLength);
	MenuItem::MenuItem(float x, float y, float z, float dX, float dZ);
	void render();

	//! returns the width and depth of the menu item
	float* getDim() { return dim; };
	bool getState() { return active;  };
	void setState(bool state) { active = state; };

  private:
	GLuint vao;				// Vertex array object, the main handle for geometry
	int nverts;				// Number of vertices in the vertex array
	int ntris;				// Number of triangles in the index array (may be zero)
	GLuint vertexbuffer;	// Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;		// Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
	GLfloat *vertexarray;	// Vertex array on interleaved format: x y z nx ny nz s t
	GLuint *indexarray;		// Element index array

	float dim[2];
	bool active = false;

	void clean();
};

