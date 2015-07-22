#include "DynamicMesh.h"
#include "time.h"
#include "math.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <fstream>
#include <string>

#define M_PI 3.14159265358979323846

DynamicMesh::DynamicMesh(float rad) {
	vertexArray = new vertex[MAX_NR_OF_VERTICES];
	vInfoArray = new vInfo[MAX_NR_OF_VERTICES];

	triangleArray = new triangle[MAX_NR_OF_TRIANGLES];
	triEPtr = new int[MAX_NR_OF_TRIANGLES];

	e = new halfEdge[MAX_NR_OF_EDGES];

	sVert tempSV;

	float tempP1[3]; float tempP2[3] = {0.0f, 0.0f, 0.0f};
	float stepRad = MIN_LENGTH;

	std::vector<sVert> changedVertices;
	changedVertices.reserve(10000);

	position[0] = 0.0f; position[1] = -0.22f; position[2] = -0.25f;

	orientation[0] = 1.0f; orientation[1] = 0.0f; orientation[2] = 0.0f; orientation[3] = 0.0f;
	orientation[4] = 0.0f; orientation[5] = 1.0f; orientation[6] = 0.0f; orientation[7] = 0.0f;
	orientation[8] = 0.0f; orientation[9] = 0.0f; orientation[10] = 1.0f; orientation[11] = 0.0f;
	orientation[12] = 0.0f; orientation[13] = 0.0f; orientation[14] = 0.0f; orientation[15] = 1.0f;

	nrofVerts = 6; vertexCap = 6;
	nrofTris = 8; triangleCap = 8;
	nrofEdges = 24; edgeCap = 24;

	//create queue for vertices
	for (int i = 0; i < MAX_NR_OF_VERTICES; i++)
	{
		vInfoArray[i].edgePtr = -(i + 1);
	}
	vInfoArray[MAX_NR_OF_VERTICES - 1].edgePtr = 0;
	vInfoArray[0].edgePtr = -7;
	//create queue for Triangles
	for (int i = 0; i < MAX_NR_OF_TRIANGLES; i++)
	{
		triEPtr[i] = -(i + 1);
	}
	triEPtr[MAX_NR_OF_TRIANGLES - 1] = -1;
	triEPtr[0] = -9;
	//create queue for Edges
	for (int i = 0; i < MAX_NR_OF_EDGES; i++)
	{
		e[i].nextEdge = -(i + 1);
	}
	e[MAX_NR_OF_EDGES - 1].nextEdge = -1;
	e[0].nextEdge = -25;

	// place vertecies
	// Y 0
	vertexArray[1].x = 0.0f;	vertexArray[1].y = MAX_LENGTH / 2.0f;	vertexArray[1].z = 0.0f; /*normal*/	vertexArray[1].nx = 0.0f;	vertexArray[1].ny = 1;	vertexArray[1].nz = 0.0f;
	vertexArray[2].x = 0.0f;	vertexArray[2].y = -MAX_LENGTH / 2.0f;	vertexArray[2].z = 0.0f; /*normal*/	vertexArray[2].nx = 0.0f;	vertexArray[2].ny = -1;	vertexArray[2].nz = 0.0f;

	// X 2
	vertexArray[3].x = MAX_LENGTH / 2.0f;	vertexArray[3].y = 0.0f;	vertexArray[3].z = 0.0f; /*normal*/	vertexArray[3].nx = 1;	vertexArray[3].ny = 0.0f;	vertexArray[3].nz = 0.0f;
	vertexArray[4].x = -MAX_LENGTH / 2.0f;	vertexArray[4].y = 0.0f;	vertexArray[4].z = 0.0f; /*normal*/	vertexArray[4].nx = -1;	vertexArray[4].ny = 0.0f;	vertexArray[4].nz = 0.0f;

	// Z 4
	vertexArray[5].x = 0.0f;	vertexArray[5].y = 0.0f;	vertexArray[5].z = -MAX_LENGTH / 2.0f; /*normal*/	vertexArray[5].nx = 0.0f;	vertexArray[5].ny = 0.0f;	vertexArray[5].nz = -1;
	vertexArray[6].x = 0.0f;	vertexArray[6].y = 0.0f;	vertexArray[6].z = MAX_LENGTH / 2.0f; /*normal*/	vertexArray[6].nx = 0.0f;	vertexArray[6].ny = 0.0f;	vertexArray[6].nz = 1;

	// bind triangles
	triangleArray[1].index[0] = 1;	triangleArray[1].index[1] = 4; triangleArray[1].index[2] = 6;
	triangleArray[2].index[0] = 1;	triangleArray[2].index[1] = 6; triangleArray[2].index[2] = 3;
	triangleArray[3].index[0] = 1;	triangleArray[3].index[1] = 3;	triangleArray[3].index[2] = 5;
	triangleArray[4].index[0] = 1;	triangleArray[4].index[1] = 5;	triangleArray[4].index[2] = 4;
	triangleArray[5].index[0] = 2;	triangleArray[5].index[1] = 6;	triangleArray[5].index[2] = 4;
	triangleArray[6].index[0] = 2;	triangleArray[6].index[1] = 3;	triangleArray[6].index[2] = 6;
	triangleArray[7].index[0] = 2;	triangleArray[7].index[1] = 5; triangleArray[7].index[2] = 3;
	triangleArray[8].index[0] = 2; triangleArray[8].index[1] = 4; triangleArray[8].index[2] = 5;
	
	// Bind halfEdges
	//TOP///////////////////////
	//first tri-----------------
	e[1].vertex = 1; e[1].triangle = 1;
	triEPtr[1] = 1;
	
	e[1].nextEdge = 2; e[2].vertex = 4; e[2].triangle = 1;
	e[2].nextEdge = 3; e[3].vertex = 6; e[3].triangle = 1;
	e[3].nextEdge = 1;

	vInfoArray[1].edgePtr = 3;

	//second tri--------------
	e[4].vertex = 1; e[4].triangle = 2;
	triEPtr[2] = 4;

	e[4].nextEdge = 5; e[5].vertex = 6; e[5].triangle = 2;
	e[5].nextEdge = 6; e[6].vertex = 3; e[6].triangle = 2;
	e[6].nextEdge = 4;

	vInfoArray[6].edgePtr = 4;

	//third tri----------------
	e[7].vertex = 1; e[7].triangle = 3;
	triEPtr[3] = 7;

	e[7].nextEdge = 8; e[8].vertex = 3; e[8].triangle = 3;
	e[8].nextEdge = 9; e[9].vertex = 5; e[9].triangle = 3;
	e[9].nextEdge = 7;

	vInfoArray[3].edgePtr = 7;

	//fourth tri-----------------
	e[10].vertex = 1; e[10].triangle = 4;
	triEPtr[4] = 10;

	e[10].nextEdge = 11; e[11].vertex = 5; e[11].triangle = 4;
	e[11].nextEdge = 12; e[12].vertex = 4; e[12].triangle = 4;
	e[12].nextEdge = 10;

	vInfoArray[5].edgePtr = 10;

	//BOTTOM///////////////////////////////////
	//fifth tri---------------------------
	e[13].vertex = 2; e[13].triangle = 5;
	triEPtr[5] = 13;

	e[13].nextEdge = 14; e[14].vertex = 6; e[14].triangle = 5;
	e[14].nextEdge = 15; e[15].vertex = 4; e[15].triangle = 5;
	e[15].nextEdge = 13;

	vInfoArray[2].edgePtr = 15;

	//sixth tri-----------------------
	e[16].vertex = 2; e[16].triangle = 6;
	triEPtr[6] = 16;

	e[16].nextEdge = 17; e[17].vertex = 3; e[17].triangle = 6;
	e[17].nextEdge = 18; e[18].vertex = 6; e[18].triangle = 6;
	e[18].nextEdge = 16;

	//seventh tri---------------------
	e[19].vertex = 2; e[19].triangle = 7;
	triEPtr[7] = 19;

	e[19].nextEdge = 20; e[20].vertex = 5; e[20].triangle = 7;
	e[20].nextEdge = 21; e[21].vertex = 3; e[21].triangle = 7;
	e[21].nextEdge = 19;

	//seventh tri
	e[22].vertex = 2; e[22].triangle = 8;
	triEPtr[8] = 22;

	e[22].nextEdge = 23; e[23].vertex = 4; e[23].triangle = 8;
	e[23].nextEdge = 24; e[24].vertex = 5; e[24].triangle = 8;
	e[24].nextEdge = 22;

	vInfoArray[4].edgePtr = 22;

	//TOP SIBLINGS
	e[triEPtr[1]].sibling = e[e[triEPtr[4]].nextEdge].nextEdge;
	e[e[e[triEPtr[4]].nextEdge].nextEdge].sibling = triEPtr[1];

	e[triEPtr[4]].sibling = e[e[triEPtr[3]].nextEdge].nextEdge;
	e[e[e[triEPtr[3]].nextEdge].nextEdge].sibling = triEPtr[4];

	e[triEPtr[3]].sibling = e[e[triEPtr[2]].nextEdge].nextEdge;
	e[e[e[triEPtr[2]].nextEdge].nextEdge].sibling = triEPtr[3];

	e[triEPtr[2]].sibling = e[e[triEPtr[1]].nextEdge].nextEdge;
	e[e[e[triEPtr[1]].nextEdge].nextEdge].sibling = triEPtr[2];

	//BOTTOM SIBLINGS
	e[triEPtr[5]].sibling = e[e[triEPtr[6]].nextEdge].nextEdge;
	e[e[e[triEPtr[6]].nextEdge].nextEdge].sibling = triEPtr[5];

	e[triEPtr[6]].sibling = e[e[triEPtr[7]].nextEdge].nextEdge;
	e[e[e[triEPtr[7]].nextEdge].nextEdge].sibling = triEPtr[6];

	e[triEPtr[7]].sibling = e[e[triEPtr[8]].nextEdge].nextEdge;
	e[e[e[triEPtr[8]].nextEdge].nextEdge].sibling = triEPtr[7];

	e[triEPtr[8]].sibling = e[e[triEPtr[5]].nextEdge].nextEdge;
	e[e[e[triEPtr[5]].nextEdge].nextEdge].sibling = triEPtr[8];

	//MIDDLE SIBLINGS
	e[e[triEPtr[1]].nextEdge].sibling = e[triEPtr[5]].nextEdge;
	e[e[triEPtr[5]].nextEdge].sibling = e[triEPtr[1]].nextEdge;
	
	e[e[triEPtr[2]].nextEdge].sibling = e[triEPtr[6]].nextEdge;
	e[e[triEPtr[6]].nextEdge].sibling = e[triEPtr[2]].nextEdge;

	e[e[triEPtr[3]].nextEdge].sibling = e[triEPtr[7]].nextEdge;
	e[e[triEPtr[7]].nextEdge].sibling = e[triEPtr[3]].nextEdge;

	e[e[triEPtr[4]].nextEdge].sibling = e[triEPtr[8]].nextEdge;
	e[e[triEPtr[8]].nextEdge].sibling = e[triEPtr[4]].nextEdge;

	// create sphere by subdivision
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	
	while (stepRad < rad)
	{
		for (int j = 1; j < vertexCap; j++)
		{
			tempP1[0] = vertexArray[j].x; tempP1[1] = vertexArray[j].y; tempP1[2] = vertexArray[j].z;
			if (vInfoArray[j].edgePtr > 0)
			{
				linAlg::normVec(tempP1);

				vertexArray[j].x = tempP1[0] * stepRad;
				vertexArray[j].y = tempP1[1] * stepRad;
				vertexArray[j].z = tempP1[2] * stepRad;

				tempSV.index = j;
				changedVertices.push_back(tempSV);
				vInfoArray[j].selected = 1.0f;
			}
		}
		updateArea(&changedVertices[0], changedVertices.size());
		changedVertices.clear();
		
		stepRad += MIN_LENGTH;
	}
	stepRad = rad;
	for (int i = 1; i < 60; i++)
	{
		for (int j = 0; j < vertexCap; j++)
		{
			tempP1[0] = vertexArray[j].x; tempP1[1] = vertexArray[j].y; tempP1[2] = vertexArray[j].z;
			if (vInfoArray[j].edgePtr > 0)
			{
				linAlg::normVec(tempP1);

				vertexArray[j].x = tempP1[0] * stepRad;
				vertexArray[j].y = tempP1[1] * stepRad;
				vertexArray[j].z = tempP1[2] * stepRad;

				tempSV.index = j;
				changedVertices.push_back(tempSV);
				vInfoArray[j].selected = 1.0f;
			}
		}
		updateArea(&changedVertices[0], changedVertices.size());
		changedVertices.clear();
	}
	
	
	for (int i = 0; i < vertexCap; i++)
	{
		vInfoArray[i].selected = 0.0f;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	createBuffers();
	
}

DynamicMesh::~DynamicMesh(void) {

}

DynamicMesh::DynamicMesh(std::string fileName) {
	vertexArray = new vertex[MAX_NR_OF_VERTICES];
	vInfoArray = new vInfo[MAX_NR_OF_VERTICES];

	triangleArray = new triangle[MAX_NR_OF_TRIANGLES];
	triEPtr = new int[MAX_NR_OF_TRIANGLES];

	e = new halfEdge[MAX_NR_OF_EDGES];

	// read mesh from file
	std::cout << "loading mesh..." << std::endl;

	std::ifstream file("../savedFiles/" + fileName, std::ios::in | std::ios::binary);
	if (file.is_open()) {

		int bitCount = 0;

		// read the size of the arrays
		file.seekg(0);
		file.read((char*)&vertexCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&triangleCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&edgeCap, sizeof(int));
		bitCount += sizeof(int);

		// read mesh position
		file.seekg(bitCount);
		file.read((char*)&position[0], sizeof(float) * 3);
		bitCount += sizeof(float) * 3;

		// read mesh orientation
		file.seekg(bitCount);
		file.read((char*)&orientation[0], sizeof(float) * 16);
		bitCount += sizeof(float) * 16;

		// read vertecies
		file.seekg(bitCount);
		file.read((char*)&nrofVerts, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&vertexArray[0], sizeof(vertex) * (vertexCap + 1));
		bitCount += sizeof(vertex) * (vertexCap + 1);

		// read triangles
		file.seekg(bitCount);
		file.read((char*)&nrofTris, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&triangleArray[0], sizeof(triangle) * (triangleCap + 1));
		bitCount += sizeof(triangle) * (triangleCap + 1);

		// read halfEdges
		file.seekg(bitCount);
		file.read((char*)&e[0], sizeof(halfEdge) * (edgeCap + 1));
		bitCount += sizeof(halfEdge) * (edgeCap + 1);

		// read vInfoArray
		file.seekg(bitCount);
		file.read((char*)&vInfoArray[0], sizeof(vInfo) * (vertexCap + 1));
		bitCount += sizeof(vInfo) * (vertexCap + 1);

		// read triEdgePtr
		file.seekg(bitCount);
		file.read((char*)&triEPtr[0], sizeof(int) * (triangleCap + 1));


		file.close();

		std::cout << "load complete" << std::endl;
	}
	else {
		std::cout << "could not open file" << std::endl;
	}

	//create queue for vertices
	for (int i = vertexCap + 1; i < MAX_NR_OF_VERTICES; i++)
	{
		vInfoArray[i].edgePtr = -(i + 1);
	}
	vInfoArray[MAX_NR_OF_VERTICES - 1].edgePtr = 0;
	//create queue for Triangles
	for (int i = triangleCap + 1; i < MAX_NR_OF_TRIANGLES; i++)
	{
		triEPtr[i] = -(i + 1);
	}
	triEPtr[MAX_NR_OF_TRIANGLES - 1] = -1;
	//create queue for Edges
	for (int i = edgeCap + 1; i < MAX_NR_OF_EDGES; i++)
	{
		e[i].nextEdge = -(i + 1);
	}
	e[MAX_NR_OF_EDGES - 1].nextEdge = -1;

	createBuffers();
}



void DynamicMesh::createBuffers() {
	triangle* indexP;
	dBufferData * vertexP;

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
		(MAX_NR_OF_VERTICES)*sizeof(dBufferData), NULL, GL_STREAM_DRAW);

	vertexP = (dBufferData*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(dBufferData) *MAX_NR_OF_VERTICES,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i < MAX_NR_OF_VERTICES; i++) {
		vertexP[i].x = vertexArray[i].x;
		vertexP[i].y = vertexArray[i].y;
		vertexP[i].z = vertexArray[i].z;
		vertexP[i].nx = vertexArray[i].nx;
		vertexP[i].ny = vertexArray[i].ny;
		vertexP[i].nz = vertexArray[i].nz;
		vertexP[i].selected = vInfoArray[i].selected;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // Selected
	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)(6 * sizeof(GLfloat))); // normals

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		(MAX_NR_OF_TRIANGLES)*sizeof(triangle), NULL, GL_STREAM_DRAW);

	indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(triangle) * MAX_NR_OF_TRIANGLES,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i < MAX_NR_OF_TRIANGLES; i++) {
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

void DynamicMesh::updateOGLData()
{
	triangle* indexP;
	dBufferData* vertexP;

	//KAN BEHÖVAS VETTE
	//vertexP = &vertexArray[0];

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	vertexP = (dBufferData*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(dBufferData)*(vertexCap),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i <= vertexCap; i++) {
		vertexP[i].x = vertexArray[i].x;
		vertexP[i].y = vertexArray[i].y;
		vertexP[i].z = vertexArray[i].z;
		vertexP[i].nx = vertexArray[i].nx;
		vertexP[i].ny = vertexArray[i].ny;
		vertexP[i].nz = vertexArray[i].nz;
		vertexP[i].selected = vInfoArray[i].selected;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // selected

	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)(6 * sizeof(GLfloat))); // selected

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	// Present our vertex <indices to OpenGL
	indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(triangle) * (triangleCap),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i <= triangleCap; i++) {
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

void DynamicMesh::render() {
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, (triangleCap)* sizeof(triangle), GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex uN, type, element array buffer offset)
	glBindVertexArray(0);
}

void DynamicMesh::load(std::string fileName) {
	// read mesh from file
	std::cout << "loading mesh..." << std::endl;

	std::ifstream file("../savedFiles/" + fileName, std::ios::in | std::ios::binary);

	if (file.is_open()) {

		int bitCount = 0;

		// read the size of the arrays
		file.seekg(0);
		file.read((char*)&vertexCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&triangleCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&edgeCap, sizeof(int));
		bitCount += sizeof(int);

		// read mesh position
		file.seekg(bitCount);
		file.read((char*)&position[0], sizeof(float) * 3);
		bitCount += sizeof(float) * 3;

		// read mesh orientation
		file.seekg(bitCount);
		file.read((char*)&orientation[0], sizeof(float) * 16);
		bitCount += sizeof(float) * 16;

		// read vertecies
		file.seekg(bitCount);
		file.read((char*)&nrofVerts, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&vertexArray[0], sizeof(vertex) * (vertexCap + 1));
		bitCount += sizeof(vertex) * (vertexCap + 1);

		// read triangles
		file.seekg(bitCount);
		file.read((char*)&nrofTris, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&triangleArray[0], sizeof(triangle) * (triangleCap + 1));
		bitCount += sizeof(triangle) * (triangleCap + 1);

		// read halfEdges
		file.seekg(bitCount);
		file.read((char*)&e[0], sizeof(halfEdge) * edgeCap);
		bitCount += sizeof(halfEdge) * (edgeCap + 1);

		// read vInfoArray
		file.seekg(bitCount);
		file.read((char*)&vInfoArray[0], sizeof(vInfo) * (vertexCap + 1));
		bitCount += sizeof(vInfo) * (vertexCap + 1);

		// read triEdgePtr
		file.seekg(bitCount);
		file.read((char*)&triEPtr[0], sizeof(int) * (triangleCap + 1));


		file.close();

		std::cout << "load complete" << std::endl;
	}
	else {
		std::cout << "could not open file" << std::endl;
	}
}

void DynamicMesh::save() {

	std::cout << "saving..." << std::endl;

	// get date and time
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);

	int bitCount = 0;

	strftime(buf, sizeof(buf), "%Y-%m-%d_%X", &tstruct);

	// replace : with - to be able to save file
	buf[13] = '-';
	buf[16] = '-';

	std::string fileName(buf);


	std::fstream file;
	file.open("../savedFiles/" + fileName + ".bin", std::ios::out | std::ios::binary);

	if (file.is_open()) {

		// write the size of the different arrays to the file
		file.seekp(0);
		file.write((char*)&vertexCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&triangleCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&edgeCap, sizeof(int));
		bitCount += sizeof(int);

		// write position and orientation
		file.seekp(bitCount);
		file.write((char*)&position[0], sizeof(float) * 3);
		bitCount += sizeof(float) * 3;

		file.seekp(bitCount);
		file.write((char*)&orientation[0], sizeof(float) * 16);
		bitCount += sizeof(float) * 16;

		// write verticies
		file.seekp(bitCount);
		file.write((char*)&nrofVerts, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&vertexArray[0], sizeof(vertex) * (vertexCap + 1));
		bitCount += sizeof(vertex) * (vertexCap + 1);

		// write triangles
		file.seekp(bitCount);
		file.write((char*)&nrofTris, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&triangleArray[0], sizeof(triangle) * (triangleCap + 1));
		bitCount += sizeof(triangle) * (triangleCap + 1);


		// write edges
		file.seekp(bitCount);
		file.write((char*)&e[0], sizeof(halfEdge) * (edgeCap + 1));
		bitCount += sizeof(halfEdge) * (edgeCap + 1);

		// write vertex edge pointers
		file.seekp(bitCount);
		file.write((char*)&vInfoArray[0], sizeof(vInfo) * (vertexCap + 1));
		bitCount += sizeof(vInfo) * (vertexCap + 1);

		// write triangle edge pointers
		file.seekp(bitCount);
		file.write((char*)&triEPtr[0], sizeof(int) * (triangleCap + 1));

		file.close();

		std::cout << "save complete" << std::endl;

	}
	else {
		std::cout << "could not open file for saving" << std::endl;
	}
}


void DynamicMesh::select(Wand* wand, float rad) {
	float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	float tempVec1[3]; float tempVec2[3];
	float wPoint[4]; float vPoint[3]; float vPoint2[3];
	int index; int index2;

	int tempEdge;

	bool success = false;

	float pLength = 0.0f;
	float oLength = 0.0f;
	
	int mIndex; float mLength;

	for (int i = 0; i < sVertsNR; i++)
	{
		vInfoArray[sVerts[i].index].selected = 0.0f;
	}
	sVertsNR = 0;
	//--< 1.0 | calculated the position and direction of the wand
	wPoint[0] = wand->getPosition()[0] - position[0];
	wPoint[1] = wand->getPosition()[1] - position[1];
	wPoint[2] = wand->getPosition()[2] - position[2];
	wPoint[3] = 1.0f;
	linAlg::vectorMatrixMult(orientation, wPoint, newWPoint);

	Dirr[0] = wand->getDirection()[0];
	Dirr[1] = wand->getDirection()[1];
	Dirr[2] = wand->getDirection()[2];
	Dirr[3] = 1.0f;
	linAlg::vectorMatrixMult(orientation, Dirr, newDirr);
	// 1.0 >--------------------------
	//--< 2.0 | start searching through vertexarray for points that are within the brush
	for (int i = 1; i <= vertexCap; i++) {
		//--< 2.1 | calculate vector between vertexposition and wandposition
		vPoint[0] = vertexArray[i].x;
		vPoint[1] = vertexArray[i].y;
		vPoint[2] = vertexArray[i].z;
		tempVec1[0] = vPoint[0] - newWPoint[0];
		tempVec1[1] = vPoint[1] - newWPoint[1];
		tempVec1[2] = vPoint[2] - newWPoint[2];
		// 2.1 >---------------------
		//--< 2.2 | calculate orthogonal and parallel distance between this vector and wandDirection
		pLength = (newDirr[0] * tempVec1[0] + newDirr[1] * tempVec1[1] + newDirr[2] * tempVec1[2]);//                                                 / linAlg::vecLength(newDirr);
		Dirr[0] = newDirr[0] * pLength;
		Dirr[1] = newDirr[1] * pLength;
		Dirr[2] = newDirr[2] * pLength;

		linAlg::calculateVec(tempVec1, Dirr, tempVec2);
		oLength = linAlg::vecLength(tempVec2);
		// 2.2 >----------------------
		if (pLength < 0.05f && pLength > 0.0f && oLength < rad)
		{
			//--< 2.3 | add the found vertex to list of selected vertices and mark it as selected 
			//changedVertices.push_back(i);
			sVerts[sVertsNR].index = i;
			mIndex = sVertsNR;
			mLength = oLength;
			sVertsNR++;
			
			//sVerts[nrofsVerts].vec[0] = tempVec1[0];
			//sVerts[nrofsVerts].vec[1] = tempVec1[1];
			//sVerts[nrofsVerts].vec[2] = tempVec1[2];
			//sVerts[nrofsVerts].length = oLength;
			vInfoArray[i].selected = ((rad - oLength) / rad);
			// 2.3 >-----------------------
			//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
			for (int j = 0; j < sVertsNR; j++) {
				index2 = sVerts[j].index;
				tempEdge = vInfoArray[index2].edgePtr;

				do {
					if (vInfoArray[e[tempEdge].vertex].selected == 0.0f){
						index = e[tempEdge].vertex;
						//changedEdges.push_back(tempEdge);
						//sEdges[nrofsEdges] = tempEdge;
						//nrofsEdges++;
						vPoint2[0] = vertexArray[index].x;
						vPoint2[1] = vertexArray[index].y;
						vPoint2[2] = vertexArray[index].z;
						tempVec1[0] = vPoint2[0] - newWPoint[0];
						tempVec1[1] = vPoint2[1] - newWPoint[1];
						tempVec1[2] = vPoint2[2] - newWPoint[2];

						pLength = (newDirr[0] * tempVec1[0] + newDirr[1] * tempVec1[1] + newDirr[2] * tempVec1[2]);// / linAlg::vecLength(newDirr);
						Dirr[0] = newDirr[0] * pLength;
						Dirr[1] = newDirr[1] * pLength;
						Dirr[2] = newDirr[2] * pLength;

						linAlg::calculateVec(tempVec1, Dirr, tempVec2);
						oLength = linAlg::vecLength(tempVec2);

						if (pLength < 0.05f && pLength > 0.00f && oLength < rad) {

							//changedVertices.push_back(index);
							sVerts[sVertsNR].index = index;
							
							if (oLength < mLength)
							{
								mIndex = sVertsNR;
								mLength = oLength;
							}
							sVertsNR++;
							//sVerts[nrofsVerts].vec[0] = tempVec1[0];
							//sVerts[nrofsVerts].vec[1] = tempVec1[1];
							//sVerts[nrofsVerts].vec[2] = tempVec1[2];
							//sVerts[nrofsVerts].length = oLength;
							vInfoArray[index].selected = ((rad - oLength)/rad);
						}
					}
					tempEdge = e[e[tempEdge].nextEdge].sibling;

				} while (tempEdge != vInfoArray[index2].edgePtr);

				//vertexArray[index2].selected += 1.0f;
			}
			// 2.4 >---------------------
			success = true;
			break;
		}
	}
	// 2.0 >----------------------

	if (success == true) {

		index = sVerts[mIndex].index;
		//sVerts[mIndex].index = sVerts[0].index;
		//sVerts[0].index = index;

		//start using variable as counter
		mIndex = 0;

		vPoint[0] = vertexArray[index].x; vPoint[1] = vertexArray[index].y; vPoint[2] = vertexArray[index].z;

		for (int i = 0; i < sVertsNR; i++)
		{
			index = sVerts[i].index;
			vPoint2[0] = vertexArray[index].x; vPoint2[1] = vertexArray[index].y; vPoint2[2] = vertexArray[index].z;
			linAlg::calculateVec(vPoint2, vPoint, tempVec1);
			if (linAlg::vecLength(tempVec1) < rad)
			{
				sVerts[mIndex].index = sVerts[i].index;
				sVerts[mIndex].vec[0] = sVerts[i].vec[0];
				sVerts[mIndex].vec[1] = sVerts[i].vec[1];
				sVerts[mIndex].vec[2] = sVerts[i].vec[2];
				sVerts[mIndex].length = sVerts[i].length;
				mIndex++;
			}
			else
				vInfoArray[index].selected = 0.0f;
		}
		sVertsNR = mIndex;
	}
}

void DynamicMesh::pull(Wand* wand, float rad) {
	float wPoint[4]; float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	int index;

	wPoint[0] = wand->getPosition()[0] - position[0];
	wPoint[1] = wand->getPosition()[1] - position[1];
	wPoint[2] = wand->getPosition()[2] - position[2];
	wPoint[3] = 1.0f;
	linAlg::vectorMatrixMult(orientation, wPoint, newWPoint);

	Dirr[0] = wand->getDirection()[0];
	Dirr[1] = wand->getDirection()[1];
	Dirr[2] = wand->getDirection()[2];
	Dirr[3] = 1.0f;
	linAlg::vectorMatrixMult(orientation, Dirr, newDirr);

	for (int i = 0; i < sVertsNR; i++)
	{
		index = sVerts[i].index;
		vertexArray[index].x += vertexArray[index].nx*0.0001f;
		vertexArray[index].y += vertexArray[index].ny*0.0001f;
		vertexArray[index].z += vertexArray[index].nz*0.0001f;
	}
	updateArea(sVerts, sVertsNR);
}

void DynamicMesh::push(Wand* wand, float rad) {
	float wPoint[4]; float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	int index;

	wPoint[0] = wand->getPosition()[0] - position[0];
	wPoint[1] = wand->getPosition()[1] - position[1];
	wPoint[2] = wand->getPosition()[2] - position[2];
	wPoint[3] = 1.0f;
	linAlg::vectorMatrixMult(orientation, wPoint, newWPoint);

	Dirr[0] = wand->getDirection()[0];
	Dirr[1] = wand->getDirection()[1];
	Dirr[2] = wand->getDirection()[2];
	Dirr[3] = 1.0f;
	linAlg::vectorMatrixMult(orientation, Dirr, newDirr);

	for (int i = 0; i < sVertsNR; i++)
	{
		index = sVerts[i].index;

   		vertexArray[index].x = newWPoint[0] + sVerts[i].vec[0] * rad;
		vertexArray[index].y = newWPoint[1] + sVerts[i].vec[1] * rad;
		vertexArray[index].z = newWPoint[2] + sVerts[i].vec[2] * rad;
	}

	updateArea(sVerts, sVertsNR);

}

void DynamicMesh::drag(Wand* wand, float rad) {

}

void DynamicMesh::markUp(Wand* wand, float rad) {
	float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	float tempVec1[3]; float tempVec2[3];
	float wPoint[4]; float vPoint[3]; float vPoint2[3];
	int index; int index2;

	int tempEdge;

	bool success = false;

	float pLength = 0.0f;
	float oLength = 0.0f;

	int mIndex; float mLength;

	for (int i = 0; i < sVertsNR; i++)
	{
		vInfoArray[sVerts[i].index].selected = 0.0f;
	}
	sVertsNR = 0;
	//--< 1.0 | calculated the position and direction of the wand
	wPoint[0] = wand->getPosition()[0] - position[0];
	wPoint[1] = wand->getPosition()[1] - position[1];
	wPoint[2] = wand->getPosition()[2] - position[2];
	wPoint[3] = 1.0f;
	linAlg::vectorMatrixMult(orientation, wPoint, newWPoint);

	Dirr[0] = wand->getDirection()[0];
	Dirr[1] = wand->getDirection()[1];
	Dirr[2] = wand->getDirection()[2];
	Dirr[3] = 1.0f;
	linAlg::vectorMatrixMult(orientation, Dirr, newDirr);
	// 1.0 >--------------------------
	//--< 2.0 | start searching through vertexarray for points that are within the brush
	for (int i = 1; i <= vertexCap; i++) {
		//--< 2.1 | calculate vector between vertexposition and wandposition
		vPoint[0] = vertexArray[i].x;
		vPoint[1] = vertexArray[i].y;
		vPoint[2] = vertexArray[i].z;
		tempVec1[0] = vPoint[0] - newWPoint[0];
		tempVec1[1] = vPoint[1] - newWPoint[1];
		tempVec1[2] = vPoint[2] - newWPoint[2];
		// 2.1 >---------------------
		mLength = linAlg::vecLength(tempVec1);
		// 2.2 >----------------------
		if (mLength <  rad)
		{
			//--< 2.3 | add the found vertex to list of selected vertices and mark it as selected 
			//changedVertices.push_back(i);
			sVerts[sVertsNR].index = i;
			//mIndex = sVertsNR;
			//mLength = oLength;
			linAlg::normVec(tempVec1);
			sVerts[sVertsNR].vec[0] = tempVec1[0];
			sVerts[sVertsNR].vec[1] = tempVec1[1];
			sVerts[sVertsNR].vec[2] = tempVec1[2];
			sVerts[sVertsNR].length = mLength;
			vInfoArray[i].selected = 1;

			sVertsNR++;
			// 2.3 >-----------------------
			//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
			for (int j = 0; j < sVertsNR; j++) {
				index2 = sVerts[j].index;
				tempEdge = vInfoArray[index2].edgePtr;

				do {
					if (vInfoArray[e[tempEdge].vertex].selected == 0.0f){
						index = e[tempEdge].vertex;
						//changedEdges.push_back(tempEdge);
						//sEdges[nrofsEdges] = tempEdge;
						//nrofsEdges++;
						vPoint2[0] = vertexArray[index].x;
						vPoint2[1] = vertexArray[index].y;
						vPoint2[2] = vertexArray[index].z;
						tempVec1[0] = vPoint2[0] - newWPoint[0];
						tempVec1[1] = vPoint2[1] - newWPoint[1];
						tempVec1[2] = vPoint2[2] - newWPoint[2];

						mLength = linAlg::vecLength(tempVec1);

						if (mLength < rad) {

							//changedVertices.push_back(index);
							sVerts[sVertsNR].index = index;

							//if (oLength < mLength)
							//{
							//	mIndex = sVertsNR;
							//	mLength = oLength;
							//}
							linAlg::normVec(tempVec1);
							sVerts[sVertsNR].vec[0] = tempVec1[0];
							sVerts[sVertsNR].vec[1] = tempVec1[1];
							sVerts[sVertsNR].vec[2] = tempVec1[2];
							sVerts[sVertsNR].length = mLength;
							vInfoArray[index].selected = 1;

							sVertsNR++;
						}
					}
					tempEdge = e[e[tempEdge].nextEdge].sibling;

				} while (tempEdge != vInfoArray[index2].edgePtr);

				//vertexArray[index2].selected += 1.0f;
			}
			// 2.4 >---------------------
			success = true;
			break;
		}
	}
	// 2.0 >----------------------
}

void DynamicMesh::updateArea(sVert* changeList, int listSize) {

	static float vPoint1[3], vPoint2[3], vPoint3[3], vPoint4[3];
	static float tempVec1[3], tempVec2[3], tempVec3[3];
	static float tempNorm1[3] = { 0.0f, 0.0f, 0.0f };
	static float tempNorm2[3] = { 0.0f, 0.0f, 0.0f };
	int tempEdge; int tempE;
	static int vert1, vert2, vert3; 
	static float edgeLength, edgeLength2;

	//int edges[100];
	//float edgeLengths[100];
	//int eNR = 0;
	std::vector<int> edges;
	edges.reserve(12);
	std::vector<float> edgeLengths;
	edgeLengths.reserve(12);

    for (int i = 0; i < listSize; i++)
	{
		vert3 = changeList[i].index;
		vInfoArray[vert3].selected = -vInfoArray[vert3].selected;
		
		if (vInfoArray[vert3].edgePtr < 0)
			continue;

		tempEdge = e[e[vInfoArray[vert3].edgePtr].nextEdge].sibling;
		vPoint1[0] = vertexArray[vert3].x; vPoint1[1] = vertexArray[vert3].y; vPoint1[2] = vertexArray[vert3].z;

		do {
			if (vInfoArray[e[tempEdge].vertex].selected >= 0.0f)
			{
				tempE = tempEdge;
				tempEdge = e[e[tempEdge].nextEdge].sibling;

				vert2 = e[tempE].vertex;
				vPoint2[0] = vertexArray[vert2].x; vPoint2[1] = vertexArray[vert2].y; vPoint2[2] = vertexArray[vert2].z;

				linAlg::calculateVec(vPoint2, vPoint1, tempVec1);

				edgeLength = linAlg::vecLength(tempVec1);

				//check if edge is too long
				if (edgeLength > MAX_LENGTH) {
					//check if edge should be flipped
					vert1 = e[e[e[tempE].nextEdge].nextEdge].vertex;
					vert2 = e[e[e[e[tempE].sibling].nextEdge].nextEdge].vertex;

					tempVec2[0] = vertexArray[vert1].x; tempVec2[1] = vertexArray[vert1].y; tempVec2[2] = vertexArray[vert1].z;

					tempVec3[0] = vertexArray[vert2].x; tempVec3[1] = vertexArray[vert2].y; tempVec3[2] = vertexArray[vert2].z;
					linAlg::calculateVec(tempVec2, tempVec3, tempNorm1);
					edgeLength2 = linAlg::vecLength(tempNorm1);

					if (edgeLength2 < MAX_LENGTH)
					{
						edgeFlip(tempE);
						if (edgeLength2 < MIN_LENGTH)
						{
							edgeCollapse(false, tempE);
						}
					}
					//---------------------
					//should not be flipped, should be split
					else
					{
						edgeSplit(vPoint1, tempVec1, tempE);
					}
					//----------------------
				}
				//----------------------
				// check if edge is to short
				else if (edgeLength < MIN_LENGTH) {
					edgeCollapse(false, tempE);
				}
			}
			else
				tempEdge = e[e[tempEdge].nextEdge].sibling;

		} while (tempEdge != vInfoArray[vert3].edgePtr);

		//now do it for vertexEPtr aswell---------------------------
		tempE = tempEdge;

		vert2 = e[tempE].vertex;
		vPoint2[0] = vertexArray[vert2].x; vPoint2[1] = vertexArray[vert2].y; vPoint2[2] = vertexArray[vert2].z;

		linAlg::calculateVec(vPoint2, vPoint1, tempVec1);

		edgeLength = linAlg::vecLength(tempVec1);

		//check if edge is too long
		if (edgeLength > MAX_LENGTH) {
			//check if edge should be flipped
			vert1 = e[e[e[tempE].nextEdge].nextEdge].vertex;
			vert2 = e[e[e[e[tempE].sibling].nextEdge].nextEdge].vertex;

			tempVec2[0] = vertexArray[vert1].x; tempVec2[1] = vertexArray[vert1].y; tempVec2[2] = vertexArray[vert1].z;

			tempVec3[0] = vertexArray[vert2].x; tempVec3[1] = vertexArray[vert2].y; tempVec3[2] = vertexArray[vert2].z;
			linAlg::calculateVec(tempVec2, tempVec3, tempNorm1);
			edgeLength2 = linAlg::vecLength(tempNorm1);

			if (edgeLength2 < MAX_LENGTH)
			{
				edgeFlip(tempE);
				if (edgeLength2 < MIN_LENGTH)
				{
					edgeCollapse(false, tempE);
				}
			}
			//---------------------
			//should not be flipped, should be split
			else
			{
				edgeSplit(vPoint1, tempVec1, tempE);
			}
			//----------------------
		}
		//----------------------
		// check if edge is to short
		else if (edgeLength < MIN_LENGTH) {
			edgeCollapse(false, tempE);
		}
	//>-----------------------
       
		tempEdge = vInfoArray[vert3].edgePtr;
		// Update normal /////////////////////////////////////////////////////////////////////////////
		do {
			vert1 = e[tempEdge].vertex;
			vert2 = e[e[e[tempEdge].nextEdge].nextEdge].vertex;

			vPoint2[0] = vertexArray[vert1].x;
			vPoint2[1] = vertexArray[vert1].y;
			vPoint2[2] = vertexArray[vert1].z;

			vPoint3[0] = vertexArray[vert2].x;
			vPoint3[1] = vertexArray[vert2].y;
			vPoint3[2] = vertexArray[vert2].z;

			linAlg::calculateVec(vPoint2, vPoint1, tempVec1);
			linAlg::calculateVec(vPoint3, vPoint1, tempVec2 );

			linAlg::crossProd(tempNorm1, tempVec2, tempVec1);

			linAlg::normVec(tempNorm1);

			tempNorm2[0] += tempNorm1[0];
			tempNorm2[1] += tempNorm1[1];
			tempNorm2[2] += tempNorm1[2];


			tempEdge = e[e[tempEdge].nextEdge].sibling;
		} while (tempEdge != vInfoArray[vert3].edgePtr);

		edgeLength = linAlg::vecLength(tempNorm2);

		tempNorm2[0] = tempNorm2[0] / edgeLength;
		tempNorm2[1] = tempNorm2[1] / edgeLength;
		tempNorm2[2] = tempNorm2[2] / edgeLength;

		//linAlg::normVec(tempNorm2);

        vertexArray[changeList[i].index].nx = tempNorm2[0];
		vertexArray[changeList[i].index].ny = tempNorm2[1];
		vertexArray[changeList[i].index].nz = tempNorm2[2];
	}
}

void DynamicMesh::edgeSplit(float* vPoint, float* vec, int &edge) {

	int tempV;
	int tempT1; int tempT2;
	int tempE; int tempE2;
	static int vert1, vert2, vert3, vert4;
	static float temp[3], temp2[3], temp3[3];
	static float tempNorm1[3], tempNorm2[3];
	static float tempVec1[3], tempVec2[3];

	// create new vertex point
	vert1 = e[edge].vertex;
	vert2 = e[e[edge].sibling].vertex;
	//vert3 = edge->nextEdge->nextEdge->vertex;
	//vert4 = edge->sibling->nextEdge->nextEdge->vertex;

	tempV = -vInfoArray[0].edgePtr;
	vertexCap = max(vertexCap, tempV);

	vInfoArray[0].edgePtr = vInfoArray[tempV].edgePtr;
	vertexArray[tempV].x = (vertexArray[vert1].x + vertexArray[vert2].x) / 2.0f;
	vertexArray[tempV].y = (vertexArray[vert1].y + vertexArray[vert2].y) / 2.0f;
	vertexArray[tempV].z = (vertexArray[vert1].z + vertexArray[vert2].z) / 2.0f;

	//copy triangles
	tempT1 = -triEPtr[0];
	triEPtr[0] = triEPtr[tempT1];
	triangleArray[tempT1].index[0] = triangleArray[e[edge].triangle].index[0];
	triangleArray[tempT1].index[1] = triangleArray[e[edge].triangle].index[1];
	triangleArray[tempT1].index[2] = triangleArray[e[edge].triangle].index[2];

	tempT2 = -triEPtr[0];
	triangleCap = max(triangleCap, tempT2);
	triEPtr[0] = triEPtr[tempT2];
	triangleArray[tempT2].index[0] = triangleArray[e[e[edge].sibling].triangle].index[0];
	triangleArray[tempT2].index[1] = triangleArray[e[e[edge].sibling].triangle].index[1];
	triangleArray[tempT2].index[2] = triangleArray[e[e[edge].sibling].triangle].index[2];

	// rebind old triangles
	for (int i = 0; i < 3; i++) {
		if (triangleArray[e[edge].triangle].index[i] == e[edge].vertex)
			triangleArray[e[edge].triangle].index[i] = tempV;

		if (triangleArray[e[e[edge].sibling].triangle].index[i] == e[edge].vertex)
			triangleArray[e[e[edge].sibling].triangle].index[i] = tempV;
	}

	// rebind new triangles
	for (int i = 0; i < 3; i++) {
		if (triangleArray[tempT1].index[i] == e[e[edge].sibling].vertex)
			triangleArray[tempT1].index[i] = tempV;

		if (triangleArray[tempT2].index[i] == e[e[edge].sibling].vertex)
			triangleArray[tempT2].index[i] = tempV;
	}
	
	/////////////////////////////////////////////////////////
	/*nextEdge*/tempE2 = tempE = -e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	triEPtr[tempT1] = tempE;
	vInfoArray[tempV].edgePtr = tempE;

	e[tempE].vertex = e[edge].vertex;
	e[tempE].triangle = tempT1;
	/*nextEdge*/tempE = e[tempE].nextEdge = -e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	e[tempE].vertex = tempV;
	e[tempE].triangle = tempT1;
	e[tempE].sibling = e[e[edge].nextEdge].nextEdge;
	/*nextEdge*/tempE = e[tempE].nextEdge = -e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	e[tempE].vertex = e[e[e[edge].nextEdge].nextEdge].vertex;
	e[tempE].triangle = tempT1;
	e[tempE].nextEdge = tempE2;
	e[tempE].sibling = e[e[e[edge].nextEdge].nextEdge].sibling;
	//rebind sibling of existing triangle
	e[e[e[e[edge].nextEdge].nextEdge].sibling].sibling = e[e[tempE2].nextEdge].nextEdge;
	//rebind sibling of old triangle
	e[e[e[edge].nextEdge].nextEdge].sibling = e[tempE2].nextEdge;

	/*nextEdge*/tempE2 = tempE = -e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	triEPtr[tempT2] = tempE;

	e[tempE].sibling = triEPtr[tempT1];
	//bind sibling of first newEdge
	e[triEPtr[tempT1]].sibling = tempE;
	//continue
	e[tempE].vertex = tempV;
	e[tempE].triangle = tempT2;
	/*nextedge*/tempE = e[tempE].nextEdge = -e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	e[tempE].vertex = e[edge].vertex;
	e[tempE].triangle = tempT2;
	e[tempE].sibling = e[e[e[edge].sibling].nextEdge].sibling;
	//rebind sibling of existing triangle
	e[e[e[e[edge].sibling].nextEdge].sibling].sibling = e[tempE2].nextEdge;
	/*nextedge*/tempE = e[tempE].nextEdge = -e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	e[tempE].vertex = e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex;
	e[tempE].triangle = tempT2;
	e[tempE].sibling = e[e[edge].sibling].nextEdge;
	//rebind sibling of old triangle
	e[e[e[edge].sibling].nextEdge].sibling = tempE;
	e[tempE].nextEdge = tempE2;
	edgeCap = max(edgeCap, tempE);

	// rebind old edge vertex
	vInfoArray[e[edge].vertex].edgePtr = tempE2;

	e[edge].vertex = tempV;
	e[e[e[edge].sibling].nextEdge].vertex = tempV;

	// Update normal /////////////////////////////////////////////////////////////////////////////
	tempE = vInfoArray[tempV].edgePtr;
	temp[0] = vertexArray[e[e[tempE].sibling].vertex].x;
	temp[1] = vertexArray[e[e[tempE].sibling].vertex].y;
	temp[2] = vertexArray[e[e[tempE].sibling].vertex].z;
	// loop through the edges
	do {
		vert1 = e[tempE].vertex;
		vert2 = e[e[e[tempE].nextEdge].nextEdge].vertex;

		temp2[0] = vertexArray[vert1].x;
		temp2[1] = vertexArray[vert1].y;
		temp2[2] = vertexArray[vert1].z;

		temp3[0] = vertexArray[vert2].x;
		temp3[1] = vertexArray[vert2].y;
		temp3[2] = vertexArray[vert2].z;

		linAlg::calculateVec(temp2, temp, tempVec1 );
		linAlg::calculateVec(temp3, temp, tempVec2);

		linAlg::crossProd(tempNorm1, tempVec2, tempVec1);

		linAlg::normVec(tempNorm1);

		tempNorm2[0] += tempNorm1[0];
		tempNorm2[1] += tempNorm1[1];
		tempNorm2[2] += tempNorm1[2];

		tempE = e[e[tempE].nextEdge].sibling;
	} while (tempE != vInfoArray[tempV].edgePtr);

	static float vecLenght = linAlg::vecLength(tempNorm2);

	tempNorm2[0] = tempNorm2[0] / vecLenght;
	tempNorm2[1] = tempNorm2[1] / vecLenght;
	tempNorm2[2] = tempNorm2[2] / vecLenght;

	//linAlg::normVec(tempNorm2);
	vertexArray[tempV].nx = tempNorm2[0];
	vertexArray[tempV].ny = tempNorm2[1];
	vertexArray[tempV].nz = tempNorm2[2];
	nrofVerts++;
	nrofTris = nrofTris + 2;
	
}

void DynamicMesh::edgeFlip(int &edge)
{
	int tempE; int tempE2;

	int vert1 = e[e[e[edge].nextEdge].nextEdge].vertex;
	int vert2 = e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex;

	tempE2 = e[edge].nextEdge;
	tempE = e[e[tempE2].nextEdge].sibling;
	while (tempE != tempE2)
	{
		if (e[tempE].vertex == vert2){
			//test----------------
			vert1 = e[edge].vertex;
			vert2 = e[e[edge].sibling].vertex;
			float tempVec[3] = { vertexArray[vert1].x - vertexArray[vert2].x, vertexArray[vert1].y - vertexArray[vert2].y, vertexArray[vert1].z - vertexArray[vert2].z };
			linAlg::normVec(tempVec);
			vertexArray[vert1].x -= tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[vert1].y -= tempVec[1] * (MIN_LENGTH / 2.0f); vertexArray[vert1].z -= tempVec[2] * (MIN_LENGTH / 2.0f);
			vertexArray[vert2].x += tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[vert2].y += tempVec[1] * (MIN_LENGTH / 2.0f); vertexArray[vert2].z += tempVec[2] * (MIN_LENGTH / 2.0f);
			//------------------
			return;
		}
		tempE = e[e[tempE].nextEdge].sibling;
	}
	//--------------------
	//perform flip
	for (int i = 0; i < 3; i++) {
		if (triangleArray[e[edge].triangle].index[i] == e[edge].vertex)
			triangleArray[e[edge].triangle].index[i] = vert2;

		if (triangleArray[e[e[edge].sibling].triangle].index[i] == e[e[edge].sibling].vertex)
			triangleArray[e[e[edge].sibling].triangle].index[i] = vert1;
	}
	// rebind triangles
	e[e[e[e[edge].sibling].nextEdge].nextEdge].triangle = e[edge].triangle;
	e[e[e[edge].nextEdge].nextEdge].triangle = e[e[edge].sibling].triangle;
	////
	e[e[e[e[edge].sibling].nextEdge].nextEdge].nextEdge = e[edge].nextEdge;
	e[e[e[edge].nextEdge].nextEdge].nextEdge = e[e[edge].sibling].nextEdge;
	////

	tempE = e[e[edge].nextEdge].nextEdge;
	e[e[edge].nextEdge].nextEdge = edge;

	tempE2 = e[e[e[edge].sibling].nextEdge].nextEdge;
	e[e[e[edge].sibling].nextEdge].nextEdge = e[edge].sibling;

	e[e[edge].sibling].nextEdge = tempE;
	e[edge].nextEdge = tempE2;

	e[edge].vertex = e[e[e[edge].sibling].nextEdge].vertex;
	e[e[edge].sibling].vertex = e[e[edge].nextEdge].vertex;

	vInfoArray[e[e[e[edge].nextEdge].nextEdge].vertex].edgePtr = e[edge].nextEdge;
	vInfoArray[e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex].edgePtr = e[e[edge].sibling].nextEdge;
	//std::cout << "flip" << std::endl;
	//pass on
	//edge = e[edge].nextEdge;
}

void DynamicMesh::edgeCollapse(bool recursive, int &edge) {

	int tempE; int tempEnd;
	int tempE2; int tempEnd2;
	static int currVert; 
	static int nVert;
	static int ndVert;
	int ctr = 0;

	currVert = e[e[edge].sibling].vertex;
   	nVert = e[edge].vertex;
	
	//check for special cases
	tempE = e[e[edge].nextEdge].sibling;
	tempE2 = e[e[e[edge].sibling].nextEdge].sibling;
	do{
		do{
			if (e[tempE2].vertex == e[tempE].vertex)
				ctr++;

			tempE2 = e[e[tempE2].nextEdge].sibling;
		} while (tempE2 != e[edge].sibling);

		tempE2 = e[e[e[edge].sibling].nextEdge].sibling;
		tempE = e[e[tempE].nextEdge].sibling;
	} while (tempE != edge);

	if (ctr != 2)
	{
		//test-----------------
		//float tempVec[3] = { vertexArray[nVert].x - vertexArray[currVert].x, vertexArray[nVert].y - vertexArray[currVert].y, vertexArray[nVert].z - vertexArray[currVert].z };
		//linAlg::normVec(tempVec);
		//vertexArray[nVert].x += tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[nVert].y += tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[nVert].z += tempVec[0] * (MIN_LENGTH / 2.0f);
		//vertexArray[currVert].x -= tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[currVert].y -= tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[currVert].z -= tempVec[0] * (MIN_LENGTH / 2.0f);
		//---------------------
		return;
	}
		
	// rebind edges that point to nVert
	tempE = e[e[e[edge].nextEdge].nextEdge].sibling;
	while (tempE != e[e[edge].sibling].nextEdge)
	{
		e[tempE].vertex = currVert;

		// rebind the triangles containing nVert as index
		for (int i = 0; i < 3; i++) {
			if (triangleArray[e[tempE].triangle].index[i] == nVert) {
				triangleArray[e[tempE].triangle].index[i] = currVert;
				break;
			}
		}
		tempE = e[e[e[tempE].nextEdge].nextEdge].sibling;
	}

	tempE = e[e[edge].nextEdge].sibling;
	

	// rebind edges
	e[e[e[edge].nextEdge].sibling].sibling = e[e[e[edge].nextEdge].nextEdge].sibling;
	e[e[e[e[edge].nextEdge].nextEdge].sibling].sibling = e[e[edge].nextEdge].sibling;

	e[e[e[e[edge].sibling].nextEdge].sibling].sibling = e[e[e[e[edge].sibling].nextEdge].nextEdge].sibling;
	e[e[e[e[e[edge].sibling].nextEdge].nextEdge].sibling].sibling = e[e[e[edge].sibling].nextEdge].sibling;

	// rebind vertex pointer
	vInfoArray[currVert].edgePtr = e[e[edge].nextEdge].sibling;
	vInfoArray[e[e[e[edge].nextEdge].nextEdge].vertex].edgePtr = e[e[e[edge].nextEdge].nextEdge].sibling;
	vInfoArray[e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex].edgePtr = e[e[e[e[edge].sibling].nextEdge].nextEdge].sibling;

	//reset the removed triangles
	tempE2 = e[edge].triangle;
	
	triangleArray[tempE2].index[0] = 0;
	triangleArray[tempE2].index[1] = 0;
	triangleArray[tempE2].index[2] = 0;
	tempE2 = e[e[edge].sibling].triangle;
	triangleArray[tempE2].index[0] = 0;
	triangleArray[tempE2].index[1] = 0;
	triangleArray[tempE2].index[2] = 0;
	nrofTris = nrofTris - 2;
	                                                                                                                
	// reset the removed vertex
	vertexArray[nVert].x = 100.0f;
	vertexArray[nVert].y = 100.0f;
	vertexArray[nVert].z = 100.0f;
	vertexArray[nVert].nx = 0;
	vertexArray[nVert].ny = 0;
	vertexArray[nVert].nz = 0;
	vInfoArray[nVert].selected = 0;
	nrofVerts--;

	// reset edge pointers
	vInfoArray[nVert].edgePtr = vInfoArray[0].edgePtr;
	vInfoArray[0].edgePtr = -nVert;
	//
	triEPtr[e[edge].triangle] = triEPtr[0];
	triEPtr[e[e[edge].sibling].triangle] = -e[edge].triangle;
	triEPtr[0] = -e[e[edge].sibling].triangle;

	// delete the removed edges
	e[e[e[e[edge].sibling].nextEdge].nextEdge].nextEdge = e[0].nextEdge;
	e[e[e[edge].sibling].nextEdge].nextEdge = -e[e[e[edge].sibling].nextEdge].nextEdge;
	e[e[edge].sibling].nextEdge = -e[e[edge].sibling].nextEdge;
	e[e[e[edge].nextEdge].nextEdge].nextEdge = -e[edge].sibling;
	e[e[edge].nextEdge].nextEdge = -e[e[edge].nextEdge].nextEdge;
	e[edge].nextEdge = -e[edge].nextEdge;

	e[0].nextEdge = -edge;
}
