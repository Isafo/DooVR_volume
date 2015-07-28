#include "LineCube.h"


LineCube::LineCube(float pX, float pY, float pZ, float dX, float dY, float dZ) {
	position[0] = pX;
	position[1] = pY;
	position[2] = pZ;

	nverts = 8;
	nlines = 12;

	vertexarray = new GLfloat[nverts * 3];
	indexarray = new GLuint[nlines * 2];

	//	x-coordinate				y-coordinate					z-coordinate
	//front
	vertexarray[0] = dX / 2;	vertexarray[1] = -dY / 2;		vertexarray[2] = dZ / 2;		// 0
	vertexarray[3] = dX / 2;	vertexarray[4] = dY / 2;		vertexarray[5] = dZ / 2;		// 1
	vertexarray[6] = -dX / 2;	vertexarray[7] = dY / 2;		vertexarray[8] = dZ / 2;		// 2
	vertexarray[9] = -dX / 2;	vertexarray[10] = -dY / 2;		vertexarray[11] = dZ / 2;		// 3

	// back
	vertexarray[12] = dX / 2;	vertexarray[13] = -dY / 2;		vertexarray[14] = -dZ / 2;		// 4
	vertexarray[15] = dX / 2;	vertexarray[16] = dY / 2;		vertexarray[17] = -dZ / 2;		// 5
	vertexarray[18] = -dX / 2;	vertexarray[19] = dY / 2;		vertexarray[20] = -dZ / 2;		// 6
	vertexarray[21] = -dX / 2;	vertexarray[22] = -dY / 2;		vertexarray[23] = -dZ / 2;		// 7

	// front lines
	indexarray[0] = 0; indexarray[1] = 1;
	indexarray[2] = 1; indexarray[3] = 2;
	indexarray[4] = 2; indexarray[5] = 3;
	indexarray[6] = 3; indexarray[7] = 0;

	// right side
	indexarray[8] = 0; indexarray[9] = 4;
	indexarray[10] = 1; indexarray[11] = 5;

	// right side
	indexarray[12] = 2; indexarray[13] = 6;
	indexarray[14] = 3; indexarray[15] = 7;

	// back
	indexarray[16] = 4; indexarray[17] = 5;
	indexarray[18] = 5; indexarray[19] = 6;
	indexarray[20] = 6; indexarray[21] = 7;
	indexarray[22] = 7; indexarray[23] = 4;

	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &(vao));
	glBindVertexArray(vao);

	// Generate two buffer IDs
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &indexbuffer);

	// Activate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Present our vertex coordinates to OpenGL
	glBufferData(GL_ARRAY_BUFFER,
				 3 * nverts * sizeof(GLfloat), vertexarray, GL_STATIC_DRAW);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates

	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 floats (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3 or 6 floats (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
						  3 * sizeof(GLfloat), (void*)0);

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 2 * nlines * sizeof(GLuint), indexarray, GL_STATIC_DRAW);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the index buffer while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


LineCube::~LineCube() {
	delete[] vertexarray;
	delete[] indexarray;
}

void LineCube::render() {
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, 2 * nlines, GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);
}
