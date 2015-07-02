#pragma once
#include "Entity.h"

class TrackingRange :
	public Entity
{
public:
	TrackingRange()
	{
		vao = 0;
		vertexbuffer = 0;
		indexbuffer = 0;
		vertexarray = NULL;
		indexarray = NULL;
		nverts = 0;
		nquads = 0;
	};

	TrackingRange(float x, float y, float z, float dX, float dY, float dZ);
	~TrackingRange(void);

	void createBox(float xSize, float ySize, float zSize);

	void render();

private:
	GLuint vao;          // Vertex array object, the main handle for geometry
	int nverts; // Number of vertices in the vertex array
	int nquads;  // Number of triangles in the index array (may be zero)
	GLuint vertexbuffer; // Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;  // Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
	GLfloat *vertexarray; // Vertex array on interleaved format: x y z nx ny nz s t
	GLuint *indexarray;   // Element index array
};