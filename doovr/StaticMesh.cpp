#include "StaticMesh.h"


StaticMesh::StaticMesh()
{
}


StaticMesh::~StaticMesh()
{
}


void StaticMesh::render()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (triangleCap)* sizeof(triangle), GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex uN, type, element array buffer offset)
	glBindVertexArray(0);
}

void StaticMesh::createBuffers()
{
	triangle* indexP;
	bufferData * vertexP;
	texCoords tempTex;
	tempTex.u = 0.0f;
	tempTex.v = 0.0f;


	//vertexP = &vertexArray[0];
	//indexP = &triangleArray[0];

	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate two buffer IDs
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &indexbuffer);

	// Activate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Present our vertex coordinates to OpenGL
	glBufferData(GL_ARRAY_BUFFER,
		(nrofVerts)*sizeof(bufferData), NULL, GL_STREAM_DRAW);

	vertexP = (bufferData*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(bufferData) *nrofVerts,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i < nrofVerts; i++) {
		vertexP[i].x = vertexArray[i].x;
		vertexP[i].y = vertexArray[i].y;
		vertexP[i].z = vertexArray[i].z;
		vertexP[i].nx = vertexArray[i].nx;
		vertexP[i].ny = vertexArray[i].ny;
		vertexP[i].nz = vertexArray[i].nz;
		vertexP[i].u = tempTex.u;
		vertexP[i].v = tempTex.v;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // texCoords
	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(bufferData), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(bufferData), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(bufferData), (void*)(6 * sizeof(GLfloat))); // texCoords

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		(nrofTris)*sizeof(triangle), NULL, GL_STREAM_DRAW);

	indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(triangle) * nrofTris,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i < nrofTris; i++) {
		indexP[i].index[0] = triangleArray[i].index[0];
		indexP[i].index[1] = triangleArray[i].index[1];
		indexP[i].index[2] = triangleArray[i].index[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}