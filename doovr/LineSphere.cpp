#include "LineSphere.h"


LineSphere::LineSphere(float pX, float pY, float pZ, float r) {
	position[0] = pX;
	position[1] = pY;
	position[2] = pZ;

	const float M_PI = 3.14159265359;
	const int nsegments = 100;

	nverts = 2 * (nsegments + 1);
	nlines = 2 * (nsegments + 1);

	vertexarray = new GLfloat[nverts * 3];		// x y z coordinate
	indexarray = new GLuint[nlines * 2];	// 2 vertecies per line
	
	// first circle >------------------------------------------------------------
	float theta = 2 * M_PI / float(nsegments);

	float x = r;
	float z = 0;

	vertexarray[0] = x + pX;	// x
	vertexarray[1] = pY;		// y
	vertexarray[2] = z + pZ;	// z

	//indexarray[0] = 0;
	//indexarray[1] = 1;

	int j = 1;
	int k = 2;
	int i;
	for (i = 3; (i / 3) < nsegments + 1; i = i + 3, k = k + 2, j++) {
		theta = 2.0f * 3.1415926f * float(j) / float(nsegments); //get the current angle 

		x = r * cosf(theta);
		z = r * sinf(theta);

		vertexarray[i] = x + pX;		// x
		vertexarray[i + 1] = pY;		// y
		vertexarray[i + 2] = z + pZ;	// z

		indexarray[k] = j - 1;
		indexarray[k + 1] = j;
	}
	
	// second circle >------------------------------------------------------------
	theta = 2 * M_PI / float(nsegments);

	z = r;
	float y = 0;

	// first pooint
	vertexarray[i] = pX;			// x
	vertexarray[i + 1] = y + pY;	// y
	vertexarray[i + 2] = z + pZ;	// z

	//indexarray[k] = j;
	//indexarray[k + 1] = j + 1;

	j = j + 2;
	k = k + 2;
	for (i = i + 3; (i / 3) < 2 * (nsegments + 1); i = i + 3, k = k + 2, j++) {
		theta = 2.0f * 3.1415926f * float(j) / float(nsegments); //get the current angle 

		float z = r * cosf(theta);
		float y = r * sinf(theta);

		vertexarray[i] = pX;			// x
		vertexarray[i + 1] = y + pY;	// y
		vertexarray[i + 2] = z + pZ;	// z

		indexarray[k] = j - 1;
		indexarray[k + 1] = j;
	}
	
	indexarray[k - 1] = nsegments + 2;

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


LineSphere::~LineSphere() {
	delete[] vertexarray;
	delete[] indexarray;
}

void LineSphere::render() {
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, 2 * nlines, GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);
}
