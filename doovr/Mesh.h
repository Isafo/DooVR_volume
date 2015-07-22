#pragma once
#include "Utilities.h"

//! Data structure containing the coordinates and normal coordinates of a vertex, aswell as a pointer to an adjacent face
struct vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat nx;
	GLfloat ny;
	GLfloat nz;
};

//! Data structure containing three indices of the vertexArray that make a certain triangle. Points to one edge in the triangle
struct triangle {
	GLuint index[3];
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	float* getPosition(){ return position; };
	float* getOrientation(){ return orientation; };

	void setPosition(float* p) { position[0] = p[0]; position[1] = p[1]; position[2] = p[2]; }
	void setOrientation(float* o) { std::copy(o, o + 16, orientation); }

	virtual void render() = 0;
	virtual void createBuffers() = 0;

protected:

	//array that stres all vertices of the mesh
	vertex* vertexArray;
	int nrofVerts;
	//largest index in the vertexArray where values exist 
	int vertexCap;

	//Array that stores all triangles of the mesh
	triangle* triangleArray;
	int nrofTris;
	//largest index in the indexArray where values exist 
	int triangleCap;

	float position[3];
	float orientation[16];

	GLuint vao;          // Vertex array object, the main handle for geometry
	GLuint vertexbuffer; // Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;  // Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
};

