#include "Mesh.h"
#include "time.h"
#include "math.h"
#include <algorithm>
#include <iterator>

using namespace std;
#define M_PI 3.14159265358979323846


Mesh::Mesh(float rad) {
	triangle tempT;
	vertex tempV;
	int tempE1;
	int tempIn1;
	int tempIn2;
	bool success = true;

	float tempP1[3]; float tempP2[3] = {0.0f, 0.0f, 0.0f};
	int tempSize = 0;
	float testLength = 0.0f;

	vector<int> changedVertices;
	changedVertices.reserve(10000);
	vector<int> changedEdges;
	changedEdges.reserve(30000);

	position[0] = 0.0f;
	position[1] = -0.22f;
	position[2] = -0.25f;

	orientation[0] = 1.0f;
	orientation[1] = 0.0f;
	orientation[2] = 0.0f;
	orientation[3] = 0.0f;

	orientation[4] = 0.0f;
	orientation[5] = 1.0f;
	orientation[6] = 0.0f;
	orientation[7] = 0.0f;

	orientation[8] = 0.0f;
	orientation[9] = 0.0f;
	orientation[10] = 1.0f;
	orientation[11] = 0.0f;

	orientation[12] = 0.0f;
	orientation[13] = 0.0f;
	orientation[14] = 0.0f;
	orientation[15] = 1.0f;

	triangle* indexP;
	vertex * vertexP;

	nrofVerts = 6; vertexCap = 6;
	nrofTris = 8; triangleCap = 8;
	nrofEdges = 24; edgeCap = 24;

	for (int i = 0; i < 1000000; i++)
	{
		vertexEPtr[i] = -(i + 1);
	}
	vertexEPtr[1000000 - 1] = 0;
	vertexEPtr[0] = -7;
	for (int i = 0; i < 2000000; i++)
	{
		triEPtr[i] = -(i + 1);
	}
	triEPtr[2000000 - 1] = -1;
	triEPtr[0] = -9;
	for (int i = 0; i < 3000000; i++)
	{
		e[i].nextEdge = -(i + 1);
	}
	e[3000000 - 1].nextEdge = -1;
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
	indexArray[1].index[0] = 1;	indexArray[1].index[1] = 4; indexArray[1].index[2] = 6;

	indexArray[2].index[0] = 1;	indexArray[2].index[1] = 6; indexArray[2].index[2] = 3;

	indexArray[3].index[0] = 1;	indexArray[3].index[1] = 3;	indexArray[3].index[2] = 5;

	indexArray[4].index[0] = 1;	indexArray[4].index[1] = 5;	indexArray[4].index[2] = 4;

	indexArray[5].index[0] = 2;	indexArray[5].index[1] = 6;	indexArray[5].index[2] = 4;

	indexArray[6].index[0] = 2;	indexArray[6].index[1] = 3;	indexArray[6].index[2] = 6;

	indexArray[7].index[0] = 2;	indexArray[7].index[1] = 5; indexArray[7].index[2] = 3;

	indexArray[8].index[0] = 2; indexArray[8].index[1] = 4; indexArray[8].index[2] = 5;
	
	// Bind halfEdges
	//TOP///////////////////////
	//first tri-----------------
	e[1].vertex = 1; e[1].triangle = 1;
	triEPtr[1] = 1;
	
	e[1].nextEdge = 2; e[2].vertex = 4; e[2].triangle = 1;
	e[2].nextEdge = 3; e[3].vertex = 6; e[3].triangle = 1;
	e[3].nextEdge = 1;

	vertexEPtr[1] = 3;

	//triEPtr[0]->nextEdge = new halfEdge; triEPtr[0]->nextEdge->vertex = 3; triEPtr[0]->nextEdge->triangle = 0;
	//triEPtr[0]->nextEdge->nextEdge = new halfEdge; triEPtr[0]->nextEdge->nextEdge->vertex = 5; triEPtr[0]->nextEdge->nextEdge->triangle = 0;
	//triEPtr[0]->nextEdge->nextEdge->nextEdge = triEPtr[0];

	//vertexEPtr[0] = triEPtr[0]->nextEdge->nextEdge;

	//second tri--------------
	e[4].vertex = 1; e[4].triangle = 2;
	triEPtr[2] = 4;

	e[4].nextEdge = 5; e[5].vertex = 6; e[5].triangle = 2;
	e[5].nextEdge = 6; e[6].vertex = 3; e[6].triangle = 2;
	e[6].nextEdge = 4;

	vertexEPtr[6] = 4;


	//tempE1 = new halfEdge;	tempE1->vertex = 0; tempE1->triangle = 1;
	//triEPtr[1] = tempE1;
	
	//triEPtr[1]->nextEdge = new halfEdge; triEPtr[1]->nextEdge->vertex = 5; triEPtr[1]->nextEdge->triangle = 1;
	//triEPtr[1]->nextEdge->nextEdge = new halfEdge; triEPtr[1]->nextEdge->nextEdge->vertex = 2; triEPtr[1]->nextEdge->nextEdge->triangle = 1;
	//triEPtr[1]->nextEdge->nextEdge->nextEdge = triEPtr[1];

	//vertexEPtr[5] = triEPtr[1];

	//third tri----------------
	e[7].vertex = 1; e[7].triangle = 3;
	triEPtr[3] = 7;

	e[7].nextEdge = 8; e[8].vertex = 3; e[8].triangle = 3;
	e[8].nextEdge = 9; e[9].vertex = 5; e[9].triangle = 3;
	e[9].nextEdge = 7;

	vertexEPtr[3] = 7;

	//tempE1 = new halfEdge;	tempE1->vertex = 0; tempE1->triangle = 2;
	//triEPtr[2] = tempE1;

	//triEPtr[2]->nextEdge = new halfEdge; triEPtr[2]->nextEdge->vertex = 2; triEPtr[2]->nextEdge->triangle = 2;
	//triEPtr[2]->nextEdge->nextEdge = new halfEdge; triEPtr[2]->nextEdge->nextEdge->vertex = 4; triEPtr[2]->nextEdge->nextEdge->triangle = 2;
	//triEPtr[2]->nextEdge->nextEdge->nextEdge = triEPtr[2];

	//vertexEPtr[2] = triEPtr[2];

	//fourth tri-----------------
	e[10].vertex = 1; e[10].triangle = 4;
	triEPtr[4] = 10;

	e[10].nextEdge = 11; e[11].vertex = 5; e[11].triangle = 4;
	e[11].nextEdge = 12; e[12].vertex = 4; e[12].triangle = 4;
	e[12].nextEdge = 10;

	vertexEPtr[5] = 10;


	//tempE1 = new halfEdge;	tempE1->vertex = 0; tempE1->triangle = 3;
	//triEPtr[3] = tempE1;

	//triEPtr[3]->nextEdge = new halfEdge; triEPtr[3]->nextEdge->vertex = 4; triEPtr[3]->nextEdge->triangle = 3;
	//triEPtr[3]->nextEdge->nextEdge = new halfEdge; triEPtr[3]->nextEdge->nextEdge->vertex = 3; triEPtr[3]->nextEdge->nextEdge->triangle = 3;
	//triEPtr[3]->nextEdge->nextEdge->nextEdge = triEPtr[3];

	//vertexEPtr[4] = triEPtr[3];

	//BOTTOM///////////////////////////////////
	//fifth tri---------------------------
	e[13].vertex = 2; e[13].triangle = 5;
	triEPtr[5] = 13;

	e[13].nextEdge = 14; e[14].vertex = 6; e[14].triangle = 5;
	e[14].nextEdge = 15; e[15].vertex = 4; e[15].triangle = 5;
	e[15].nextEdge = 13;

	vertexEPtr[2] = 15;


	//tempE1 = new halfEdge;	tempE1->vertex = 1; tempE1->triangle = 4;
	//triEPtr[4] = tempE1;

	//triEPtr[4]->nextEdge = new halfEdge; triEPtr[4]->nextEdge->vertex = 5; triEPtr[4]->nextEdge->triangle = 4;
	//triEPtr[4]->nextEdge->nextEdge = new halfEdge; triEPtr[4]->nextEdge->nextEdge->vertex = 3; triEPtr[4]->nextEdge->nextEdge->triangle = 4;
	//triEPtr[4]->nextEdge->nextEdge->nextEdge = triEPtr[4];

	//vertexEPtr[1] = triEPtr[4]->nextEdge->nextEdge;

	//sixth tri-----------------------
	e[16].vertex = 2; e[16].triangle = 6;
	triEPtr[6] = 16;

	e[16].nextEdge = 17; e[17].vertex = 3; e[17].triangle = 6;
	e[17].nextEdge = 18; e[18].vertex = 6; e[18].triangle = 6;
	e[18].nextEdge = 16;

	//tempE1 = new halfEdge;	tempE1->vertex = 1; tempE1->triangle = 5;
	//triEPtr[5] = tempE1;
	
	//triEPtr[5]->nextEdge = new halfEdge; triEPtr[5]->nextEdge->vertex = 2; triEPtr[5]->nextEdge->triangle = 5;
	//triEPtr[5]->nextEdge->nextEdge = new halfEdge; triEPtr[5]->nextEdge->nextEdge->vertex = 5; triEPtr[5]->nextEdge->nextEdge->triangle = 5;
	//triEPtr[5]->nextEdge->nextEdge->nextEdge = triEPtr[5];

	//seventh tri---------------------
	e[19].vertex = 2; e[19].triangle = 7;
	triEPtr[7] = 19;

	e[19].nextEdge = 20; e[20].vertex = 5; e[20].triangle = 7;
	e[20].nextEdge = 21; e[21].vertex = 3; e[21].triangle = 7;
	e[21].nextEdge = 19;


	//tempE1 = new halfEdge;	tempE1->vertex = 1; tempE1->triangle = 6;
	//triEPtr[6] = tempE1;
	
	//triEPtr[6]->nextEdge = new halfEdge; triEPtr[6]->nextEdge->vertex = 4; triEPtr[6]->nextEdge->triangle = 6;
	//triEPtr[6]->nextEdge->nextEdge = new halfEdge; triEPtr[6]->nextEdge->nextEdge->vertex = 2; triEPtr[6]->nextEdge->nextEdge->triangle = 6;
	//triEPtr[6]->nextEdge->nextEdge->nextEdge = triEPtr[6];

	//seventh tri
	e[22].vertex = 2; e[22].triangle = 8;
	triEPtr[8] = 22;

	e[22].nextEdge = 23; e[23].vertex = 4; e[23].triangle = 8;
	e[23].nextEdge = 24; e[24].vertex = 5; e[24].triangle = 8;
	e[24].nextEdge = 22;

	vertexEPtr[4] = 22;


	//tempE1 = new halfEdge;	tempE1->vertex = 1; tempE1->triangle = 7;
	//triEPtr[7] = tempE1;
	
	//triEPtr[7]->nextEdge = new halfEdge; triEPtr[7]->nextEdge->vertex = 3; triEPtr[7]->nextEdge->triangle = 7;
	//triEPtr[7]->nextEdge->nextEdge = new halfEdge; triEPtr[7]->nextEdge->nextEdge->vertex = 4; triEPtr[7]->nextEdge->nextEdge->triangle = 7;
	//triEPtr[7]->nextEdge->nextEdge->nextEdge = triEPtr[7];

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
	
	//while (success)
	//{
	//	success = false;

	//	tempSize = nrofVerts;
	//	for (int j = 0; j < nrofVerts; j++)
	//	{
	//		tempP1[0] = vertexArray[j].x; tempP1[1] = vertexArray[j].y; tempP1[2] = vertexArray[j].z;
	//		testLength = linAlg::vecLength(tempP1);
	//		if (vertexEPtr[j] > 0 && testLength < rad)
	//		{
	//			success = true;
	//			linAlg::normVec(tempP1);

	//			vertexArray[j].x += tempP1[0] * MIN_LENGTH;
	//			vertexArray[j].y += tempP1[1] * MIN_LENGTH;
	//			vertexArray[j].z += tempP1[2] * MIN_LENGTH;

	//			//vertexEPtr[j]->needsUpdate = true;
	//			//tempE1 = e[e[vertexEPtr[j]].nextEdge].sibling;
	//			/*tempE1 = vertexEPtr[j];
	//			do {
	//				if (vertexArray[e[tempE1].vertex].selected != 1.0f){
	//					changedEdges.push_back(tempE1);
	//				}
	//				tempE1 = e[e[tempE1].nextEdge].sibling;
	//				
	//			} while (tempE1 != vertexEPtr[j]);*/

	//			changedVertices.push_back(j);
	//			vertexArray[j].selected = 1.0f;
	//		}
	//	}
	//	
	//	updateArea(&changedVertices[0], changedVertices.size());
	//	if (tempSize != nrofVerts)
	//		success = true;
	//}
	
	for (int i = 0; i < vertexCap; i++)
	{
		vertexArray[i].selected = 0.0f;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////


	vertexP = &vertexArray[0];
	indexP = &indexArray[0];

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
		(1000000)*sizeof(vertex), NULL, GL_STREAM_DRAW);

	vertexP = (vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(vertex) *1000000,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i < 1000000; i++) {
		vertexP[i].x = vertexArray[i].x;
		vertexP[i].y = vertexArray[i].y;
		vertexP[i].z = vertexArray[i].z;
		vertexP[i].nx = vertexArray[i].nx;
		vertexP[i].ny = vertexArray[i].ny;
		vertexP[i].nz = vertexArray[i].nz;
		vertexP[i].selected = vertexArray[i].selected;
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
		sizeof(vertex), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (void*)(6 * sizeof(GLfloat))); // normals

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		(2000000)*sizeof(triangle), NULL, GL_STREAM_DRAW);

	indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(triangle) * 2000000,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i < 2000000; i++) {
		indexP[i].index[0] = indexArray[i].index[0];
		indexP[i].index[1] = indexArray[i].index[1];
		indexP[i].index[2] = indexArray[i].index[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::~Mesh(void) {

}

void Mesh::select(Wand* wand, float rad) {
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
		vertexArray[sVerts[i].index].selected = 0.0f;
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
			vertexArray[i].selected = ((rad - oLength) / rad);
			// 2.3 >-----------------------
			//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
			for (int j = 0; j < sVertsNR; j++) {
				index2 = sVerts[j].index;
				tempEdge = vertexEPtr[index2];

				do {
					if (vertexArray[e[tempEdge].vertex].selected == 0.0f){
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
							vertexArray[index].selected = ((rad - oLength)/rad);
						}
					}
					tempEdge = e[e[tempEdge].nextEdge].sibling;

				} while (tempEdge != vertexEPtr[index2]);

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
				vertexArray[index].selected = 0.0f;
		}
		sVertsNR = mIndex;
	}
	
}

void Mesh::pull(Wand* wand, float rad) {
	float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	float tempVec1[3]; float tempVec2[3];
	float wPoint[4]; float vPoint[3]; float vPoint2[3];
	int index; int index2;

	vector<int> changedVertices;
	changedVertices.reserve(100);
//	vector<float> changedLengths;
	//changedLengths.reserve(100);
	vector<int> changedEdges;
	changedEdges.reserve(300);
	int changeCount = 0;

	int tempEdge;

	bool success = false;

	float pLength = 0.0f;
	float oLength = 0.0f;
	//float minLength = 0.0f;
	//int minIndex = 0;

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

	//for (int i = 1; i <= nrofVerts; i++) {
	//	vPoint[0] = vertexArray[i].x;
	//	vPoint[1] = vertexArray[i].y;
	//	vPoint[2] = vertexArray[i].z;
	//	tempVec1[0] = vPoint[0] - newWPoint[0];
	//	tempVec1[1] = vPoint[1] - newWPoint[1];
	//	tempVec1[2] = vPoint[2] - newWPoint[2];

	//	pLength = (newDirr[0] * tempVec1[0] + newDirr[1] * tempVec1[1] + newDirr[2] * tempVec1[2]);//                                                 / linAlg::vecLength(newDirr);
	//	Dirr[0] = newDirr[0] * pLength;
	//	Dirr[1] = newDirr[1] * pLength;
	//	Dirr[2] = newDirr[2] * pLength;

	//	linAlg::calculateVec(tempVec1, Dirr, tempVec2);
	//	oLength = linAlg::vecLength(tempVec2);
	//	if (pLength < 0.02f && pLength > 0.0f && oLength < rad)
	//	{
 //  			changedVertices.push_back(i);
	//		vertexArray[i].selected = 0.5f;
	//		//changedLengths.push_back(mLength);
	//		//minLength = mLength;
	//		//minIndex = 0;
	//		//vertexArray[i].selected = 1.0f;
	//		//normVec(tempVec1);
	//		//mLength = 0.002f*(0.05f / (mLength + 0.05f));
	//		//linAlg::normVec(tempVec1);
	//		pLength = 0.001f*((rad - oLength) / (rad))*((rad - oLength) / (rad));
	//		vertexArray[i].x += vertexArray[i].nx * pLength;
	//		vertexArray[i].y += vertexArray[i].ny * pLength;
	//		vertexArray[i].z += vertexArray[i].nz * pLength;

	//		//glfwGetTime

	//	//	vertexArray[i].x = newWPoint[0] + tempVec1[0]  *rad;
	//	//	vertexArray[i].y = newWPoint[1] + tempVec1[1] * rad;
	//	//	vertexArray[i].z = newWPoint[2] + tempVec1[2] * rad;

	//		for (int j = 0; j < changedVertices.size(); j++) {
	//			index2 = changedVertices[j];
	//			tempEdge = vertexEPtr[index2];
	//			
	//			do {
	//				if (vertexArray[e[tempEdge].vertex].selected != 1.0f){
	//					index = e[tempEdge].vertex;
	//					changedEdges.push_back(tempEdge);
	//					//e[e[tempEdge].sibling].needsUpdate = true;
	//					//e[tempEdge].needsUpdate = true;
	//					
 //                      	
	//					vPoint2[0] = vertexArray[index].x;
	//					vPoint2[1] = vertexArray[index].y;
	//					vPoint2[2] = vertexArray[index].z;
	//					tempVec1[0] = vPoint2[0] - newWPoint[0];
	//					tempVec1[1] = vPoint2[1] - newWPoint[1];
	//					tempVec1[2] = vPoint2[2] - newWPoint[2];

	//					//mLength = linAlg::vecLength(tempVec1);
	//					pLength = (newDirr[0] * tempVec1[0] + newDirr[1] * tempVec1[1] + newDirr[2] * tempVec1[2]);// / linAlg::vecLength(newDirr);
	//					Dirr[0] = newDirr[0] * pLength;
	//					Dirr[1] = newDirr[1] * pLength;
	//					Dirr[2] = newDirr[2] * pLength;

	//					linAlg::calculateVec(tempVec1, Dirr, tempVec2);
	//					oLength = linAlg::vecLength(tempVec2);

	//					if (pLength < 0.02f && pLength > 0.00f && vertexArray[index].selected != 0.5f && oLength < rad) {
	//						//linAlg::normVec(tempVec1);
	//					//	vertexArray[index].x = newWPoint[0] + tempVec1[0] * rad;
	//					//	vertexArray[index].y = newWPoint[1] + tempVec1[1] * rad;
	//					//	vertexArray[index].z = newWPoint[2] + tempVec1[2] * rad;
	//						pLength = 0.001f*((rad - oLength) / (rad))*((rad - oLength) / (rad));
	//						vertexArray[index].x += vertexArray[index].nx * pLength;
	//						vertexArray[index].y += vertexArray[index].ny * pLength;
	//						vertexArray[index].z += vertexArray[index].nz * pLength;
	//						
	//						changedVertices.push_back(index);
	//						vertexArray[index].selected = 0.5f;

	//						/*changedLengths.push_back(mLength);
	//						if (mLength < minLength) {
	//							minLength = mLength;
	//							minIndex = changedVertices.size() - 1;
	//						}*/
	//						
	//					}
	//				}
	//				tempEdge = e[e[tempEdge].nextEdge].sibling;

	//			} while (tempEdge != vertexEPtr[index2]);

	//			vertexArray[index2].selected = 1.0f;
	//		}
	//		success = true;
	//		break;
	//	}
	//}
	//
	////changeCount = 0;
	////if (changedVertices.size() > 0)
	//	

	//if (success == true) {

	//	////swap so that the point closest to the point is first in the list---------------------
	//	//index = changedVertices[minIndex];
	//	//changedVertices[minIndex] = changedVertices[0];
	//	//changedVertices[0] = index;

	//	//mLength = changedLengths[minIndex];
	//	//changedLengths[minIndex] = changedLengths[0];
	//	//changedLengths[0] = mLength;
	//	////-------------------
	//	////calculate vector between wand and vertex points
	//	//vPoint[0] = vertexArray[index].x;
	//	//vPoint[1] = vertexArray[index].y;
	//	//vPoint[2] = vertexArray[index].z;
	//	//tempVec1[0] = vPoint[0] - newWPoint[0];
	//	//tempVec1[1] = vPoint[1] - newWPoint[1];
	//	//tempVec1[2] = vPoint[2] - newWPoint[2];
	//	////--------------------
	//	////calculate length between wandPos and projection of closest point on wandDir--------
	//	//mLength = newDir[0] * tempVec1[0] + newDir[1] * tempVec1[1] + newDir[2] * tempVec1[2];
	//	//cout << mLength << endl;
	//	//newDir[0] = -(newDir[0] * mLength);
	//	//newDir[1] = -(newDir[1] * mLength);
	//	//newDir[2] = -(newDir[2] * mLength);
	//	////---------------
	//	////start moving points. closest pointis moved by the entire projection.
	//	////other points are moved proportionally to their distance to wandPos--------------
	//	//vertexArray[index].x += newDir[0];
	//	//vertexArray[index].y += newDir[1];
	//	//vertexArray[index].z += newDir[2];

	//	//for (int i = 1; i < changedVertices.size(); i++)
	//	//{
	//	//	index = changedVertices[i];
	//	//	mLength = (rad - changedLengths[i] * changedLengths[i]) / rad;

	//	//	vertexArray[index].x += newDir[0] * mLength;
	//	//	vertexArray[index].y += newDir[1] * mLength;
	//	//	vertexArray[index].z += newDir[2] * mLength;
	//	//}
	//	//----------------------------

	//	updateArea(&changedVertices[0], changedVertices.size());
	//	updateOGLData();
	//}
}

void Mesh::push(Wand* wand, float rad) {
	float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	float tempVec1[3]; float tempVec2[3];
	float wPoint[4]; float vPoint[3]; float vPoint2[3];
	int index; int index2; int minIndex;

	vector<int> changedVertices;
	changedVertices.reserve(100);
	vector<float> changedLengths;
	changedLengths.reserve(100);
	vector<int> changedEdges;
	changedEdges.reserve(300);
	int changeCount = 0;

	int tempEdge;

	bool success = false;

	float pLength = 0.0f;
	float oLength = 0.0f;
	float mLength = 0.0f;

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

	//for (int i = 1; i <= nrofVerts; i++) {
	//	vPoint[0] = vertexArray[i].x;
	//	vPoint[1] = vertexArray[i].y;
	//	vPoint[2] = vertexArray[i].z;
	//	tempVec1[0] = vPoint[0] - newWPoint[0];
	//	tempVec1[1] = vPoint[1] - newWPoint[1];
	//	tempVec1[2] = vPoint[2] - newWPoint[2];

	//	pLength = (newDirr[0] * tempVec1[0] + newDirr[1] * tempVec1[1] + newDirr[2] * tempVec1[2]);//                                                 / linAlg::vecLength(newDirr);
	//	Dirr[0] = newDirr[0] * pLength;
	//	Dirr[1] = newDirr[1] * pLength;
	//	Dirr[2] = newDirr[2] * pLength;

	//	linAlg::calculateVec(tempVec1, Dirr, tempVec2);
	//	oLength = linAlg::vecLength(tempVec2);
	//	if (pLength < 0.02f && pLength > 0.0f && oLength < rad)
	//	{
	//		changedVertices.push_back(i);
	//		vertexArray[i].selected = 0.5f;
	//		mLength = oLength;
	//		minIndex = 0;

	//		//linAlg::normVec(tempVec1);
	//		//pLength = 0.001f*((rad - oLength) / (rad))*((rad - oLength) / (rad));

	//		//vertexArray[i].x = newWPoint[0] + tempVec2[0];// *pLength;
	//		//vertexArray[i].y = newWPoint[1] + tempVec2[1];// *pLength;
	//		//vertexArray[i].z = newWPoint[2] + tempVec2[2];// *pLength;

	//		for (int j = 0; j < changedVertices.size(); j++) {
	//			index2 = changedVertices[j];
	//			tempEdge = vertexEPtr[index2];

	//			do {
	//				if (vertexArray[e[tempEdge].vertex].selected != 1.0f){
	//					index = e[tempEdge].vertex;
	//					changedEdges.push_back(tempEdge);

	//					vPoint2[0] = vertexArray[index].x;
	//					vPoint2[1] = vertexArray[index].y;
	//					vPoint2[2] = vertexArray[index].z;
	//					tempVec1[0] = vPoint2[0] - newWPoint[0];
	//					tempVec1[1] = vPoint2[1] - newWPoint[1];
	//					tempVec1[2] = vPoint2[2] - newWPoint[2];

	//					pLength = (newDirr[0] * tempVec1[0] + newDirr[1] * tempVec1[1] + newDirr[2] * tempVec1[2]);// / linAlg::vecLength(newDirr);
	//					Dirr[0] = newDirr[0] * pLength;
	//					Dirr[1] = newDirr[1] * pLength;
	//					Dirr[2] = newDirr[2] * pLength;

	//					linAlg::calculateVec(tempVec1, Dirr, tempVec2);
	//					oLength = linAlg::vecLength(tempVec2);

	//					if (pLength < 0.025f && pLength > 0.00f && vertexArray[index].selected != 0.5f && oLength < rad) {

	//						//vertexArray[index].x = newWPoint[0] + tempVec2[0];// *pLength;
	//						//vertexArray[index].y = newWPoint[1] + tempVec2[1];// *pLength;
	//						//vertexArray[index].z = newWPoint[2] + tempVec2[2];// *pLength;
	//						changedVertices.push_back(index);
	//						vertexArray[index].selected = 0.5f;

	//						if (oLength < mLength) {
	//							mLength = oLength;
	//							minIndex = changedVertices.size() - 1;
	//						}
	//					}
	//				}
	//				tempEdge = e[e[tempEdge].nextEdge].sibling;

	//			} while (tempEdge != vertexEPtr[index2]);

	//			vertexArray[index2].selected = 1.0f;
	//		}
	//		success = true;
	//		break;
	//	}
	//}

	////swap so that the point closest to the point is first in the list---------------------
	//index = changedVertices[minIndex];
	//changedVertices[minIndex] = changedVertices[0];
	//changedVertices[0] = index;

	//tempVec1[0] = vertexArray[index].nx; tempVec1[1] = vertexArray[index].ny; tempVec1[2] = vertexArray[index].nz;

	//for (int i = 1; i < changedVertices.size(); i++)
	//{
	//	
	//}

	//if (success == true) {
	//	//updateArea(&changedVertices[0], changedVertices.size());
	//	//updateOGLData();
	//}
}

void Mesh::drag(Wand* wand, float rad) {
	float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	float tempVec1[3]; float tempVec2[3];
	float wPoint[4]; float vPoint[3]; float vPoint2[3];
	int index; int index2;

	vector<int> changedVertices;
	changedVertices.reserve(100);

	vector<int> changedEdges;
	changedEdges.reserve(300);
	int changeCount = 0;

	int tempEdge;

	bool success = false;

	float pLength = 0.0f;
	float oLength = 0.0f;

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

	for (int i = 1; i <= vertexCap; i++) {
		vPoint[0] = vertexArray[i].x;
		vPoint[1] = vertexArray[i].y;
		vPoint[2] = vertexArray[i].z;
		tempVec1[0] = vPoint[0] - newWPoint[0];
		tempVec1[1] = vPoint[1] - newWPoint[1];
		tempVec1[2] = vPoint[2] - newWPoint[2];

		pLength = (newDirr[0] * tempVec1[0] + newDirr[1] * tempVec1[1] + newDirr[2] * tempVec1[2]);//                                                 / linAlg::vecLength(newDirr);
		Dirr[0] = newDirr[0] * pLength;
		Dirr[1] = newDirr[1] * pLength;
		Dirr[2] = newDirr[2] * pLength;

		linAlg::calculateVec(tempVec1, Dirr, tempVec2);
		oLength = linAlg::vecLength(tempVec2);
		if (pLength < 0.02f && pLength > 0.0f && oLength < rad)
		{
			changedVertices.push_back(i);
			vertexArray[i].selected = 0.5f;

			//linAlg::normVec(tempVec1);
			//pLength = 0.001f*((rad - oLength) / (rad))*((rad - oLength) / (rad));
			tempVec2[0] = vertexArray[i].nx; tempVec2[2] = vertexArray[i].nz; tempVec2[2] = vertexArray[i].nz;
			pLength = linAlg::lengthToSurface(tempVec1, tempVec2, rad);

			vertexArray[i].x = newWPoint[0] + tempVec2[0] * pLength;
			vertexArray[i].y = newWPoint[1] + tempVec2[1] * pLength;
			vertexArray[i].z = newWPoint[2] + tempVec2[2] * pLength;

			for (int j = 0; j < changedVertices.size(); j++) {
				index2 = changedVertices[j];
				tempEdge = vertexEPtr[index2];

				do {
					if (vertexArray[e[tempEdge].vertex].selected != 1.0f){
						index = e[tempEdge].vertex;
						changedEdges.push_back(tempEdge);

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

						if (pLength < 0.025f && pLength > 0.00f && vertexArray[index].selected != 0.5f && oLength < rad) {

							tempVec2[0] = vertexArray[index].nx; tempVec2[2] = vertexArray[index].nz; tempVec2[2] = vertexArray[index].nz;
							pLength = linAlg::lengthToSurface(tempVec1, tempVec2, rad);

							vertexArray[index].x = newWPoint[0] + tempVec2[0] * pLength;
							vertexArray[index].y = newWPoint[1] + tempVec2[1] * pLength;
							vertexArray[index].z = newWPoint[2] + tempVec2[2] * pLength;

							changedVertices.push_back(index);
							vertexArray[index].selected = 0.5f;
						}
					}
					tempEdge = e[e[tempEdge].nextEdge].sibling;

				} while (tempEdge != vertexEPtr[index2]);

				vertexArray[index2].selected = 1.0f;
			}
			success = true;
			break;
		}
	}

	if (success == true) {
		//updateArea(&changedVertices[0], changedVertices.size());
		updateOGLData();
	}
}

void Mesh::markUp(Wand* wand , float rad) {
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
		vertexArray[sVerts[i].index].selected = 0.0f;
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
			vertexArray[i].selected = 1;

			sVertsNR++;
			// 2.3 >-----------------------
			//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
			for (int j = 0; j < sVertsNR; j++) {
				index2 = sVerts[j].index;
				tempEdge = vertexEPtr[index2];

				do {
					if (vertexArray[e[tempEdge].vertex].selected == 0.0f){
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
							vertexArray[index].selected = 1;

							sVertsNR++;
						}
					}
					tempEdge = e[e[tempEdge].nextEdge].sibling;

				} while (tempEdge != vertexEPtr[index2]);

				//vertexArray[index2].selected += 1.0f;
			}
			// 2.4 >---------------------
			success = true;
			break;
		}
	}
	// 2.0 >----------------------

}

void Mesh::updateOGLData()
{

	triangle* indexP;
	vertex* vertexP;
	vertexP = &vertexArray[0];

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	vertexP = (vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(vertex)*(vertexCap),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i <= vertexCap; i++) {
		vertexP[i].x = vertexArray[i].x;
		vertexP[i].y = vertexArray[i].y;
		vertexP[i].z = vertexArray[i].z;
		vertexP[i].nx = vertexArray[i].nx;
		vertexP[i].ny = vertexArray[i].ny;
		vertexP[i].nz = vertexArray[i].nz;
		vertexP[i].selected = vertexArray[i].selected;
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
		sizeof(vertex), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (void*)(6 * sizeof(GLfloat))); // selected

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	//vector<triangle> tempList;
	//tempList.reserve(indexArray.size());
	//indexP = &tempList[0];


	//glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	//	sizeof(triangle)*indexArray.size(), &indexArray, GL_STREAM_DRAW);

	// Present our vertex <indices to OpenGL
	indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(triangle) * (triangleCap),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 1; i <= triangleCap; i++) {
		indexP[i].index[0] = indexArray[i].index[0];
		indexP[i].index[1] = indexArray[i].index[1];
		indexP[i].index[2] = indexArray[i].index[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::render() {
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, (triangleCap) * sizeof(triangle), GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex uN, type, element array buffer offset)
	glBindVertexArray(0);
}


void Mesh::updateArea(sVert* changeList, int listSize) {

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
	vector<int> edges;
	edges.reserve(12);
	vector<float> edgeLengths;
	edgeLengths.reserve(12);


	//cout << "u" << endl;
    for (int i = 0; i < listSize; i++)
	{
		vert3 = changeList[i].index;
		vertexArray[vert3].selected = -vertexArray[vert3].selected;
		
		if (vertexEPtr[vert3] < 0)
			continue;

		tempEdge = vertexEPtr[vert3];
		vPoint1[0] = vertexArray[vert3].x; vPoint1[1] = vertexArray[vert3].y; vPoint1[2] = vertexArray[vert3].z;

		do {
			if (vertexArray[e[tempEdge].vertex].selected >= 0.0f)
			{
				vert2 = e[tempEdge].vertex;
				//if (vertexArray[vert1].)
				vPoint2[0] = vertexArray[vert2].x; vPoint2[1] = vertexArray[vert2].y; vPoint2[2] = vertexArray[vert2].z;

				linAlg::calculateVec(vPoint2, vPoint1, tempVec1);

				edges.push_back(tempEdge);
				edgeLengths.push_back(linAlg::vecLength(tempVec1));
				//edgeLengths[eNR]= linAlg::vecLength(tempVec1);
				//eNR++;
				//edgeLength = linAlg::vecLength(tempVec1);
				if (e[tempEdge].nextEdge < 0)
					cout << "yo";
			}


			tempEdge = e[e[tempEdge].nextEdge].sibling;
		} while (tempEdge != vertexEPtr[vert3]);

        for (int i = 0; i < edges.size(); i++)
		{
			//this if should be removed
			if (e[edges[i]].nextEdge < 0)
				continue;
		//check if edge is too long
			if (edgeLengths[i] > MAX_LENGTH) {
				//check if edge should be flipped
				//cout << "s" << endl;
				vert1 = e[e[e[edges[i]].nextEdge].nextEdge].vertex;
				vert2 = e[e[e[e[edges[i]].sibling].nextEdge].nextEdge].vertex;

				tempVec2[0] = vertexArray[vert1].x;
				tempVec2[1] = vertexArray[vert1].y;
				tempVec2[2] = vertexArray[vert1].z;

				tempVec3[0] = vertexArray[vert2].x;
				tempVec3[1] = vertexArray[vert2].y;
				tempVec3[2] = vertexArray[vert2].z;
				linAlg::calculateVec(tempVec2, tempVec3, tempNorm1);
				edgeLength2 = linAlg::vecLength(tempNorm1);
					
				if (edgeLength2 < MAX_LENGTH)
				{
					edgeFlip(edges[i]);
					if (edgeLength2 < MIN_LENGTH)
					{
						//cout << "c" << endl;
						edgeCollapse(false, edges[i]);
					}
				}
				//---------------------
				//should not be flipped, should be split
				else
				{
					edgeSplit(vPoint1, tempVec1, edges[i]);
				}
				//----------------------
				//cout << "ss" << endl;
			}
			//----------------------
			// check if edge is to short
			else if (edgeLengths[i] < MIN_LENGTH) {
				//cout << "c" << endl;
				edgeCollapse(false, edges[i]);
				//cout << "cc" << endl;

			}
		}
		//eNR = 0;
		edges.clear();
		edgeLengths.clear();

		tempEdge = vertexEPtr[vert3];
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
		} while (tempEdge != vertexEPtr[vert3]);

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

void Mesh::edgeSplit(float* vPoint, float* vec, int &edge) {

	int tempV;
	int tempT1; int tempT2;
	int tempE; int tempE2;
	static int vert1, vert2, vert3, vert4;
	static float temp[3], temp2[3], temp3[3];
	static float tempNorm1[3], tempNorm2[3];
	static float tempVec1[3], tempVec2[3];

	////TODO: FLIP EDGE, NOT HANDLED PROPERLY BUT SHOULD BE////////////////////////////////////
	//
 //  	vert1 = e[e[e[edge].nextEdge].nextEdge].vertex;
	//vert2 = e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex;

	//temp[0] = vertexArray[vert1].x;
	//temp[1] = vertexArray[vert1].y;
	//temp[2] = vertexArray[vert1].z;

	//temp2[0] = vertexArray[vert2].x;
	//temp2[1] = vertexArray[vert2].y;
	//temp2[2] = vertexArray[vert2].z;
	//linAlg::calculateVec(temp, temp2, temp3);
	//linAlg::vecLength(temp3);
	//
	//if (linAlg::vecLength(temp3) < MAX_LENGTH && edge != e[e[e[e[e[e[edge].nextEdge].sibling].nextEdge].sibling].nextEdge].sibling && edge != e[e[e[e[e[e[edge].sibling].nextEdge].sibling].nextEdge].sibling].nextEdge) 
	////if (false)
	//{
	//	for (int i = 0; i < 3; i++) {
	//		if (indexArray[e[edge].triangle].index[i] == e[edge].vertex)
	//			indexArray[e[edge].triangle].index[i] = vert2;

	//		if (indexArray[e[e[edge].sibling].triangle].index[i] == e[e[edge].sibling].vertex)
	//			indexArray[e[e[edge].sibling].triangle].index[i] = vert1;
	//	}
	//	// rebind triangles
	//	e[e[e[e[edge].sibling].nextEdge].nextEdge].triangle = e[edge].triangle;
	//	e[e[e[edge].nextEdge].nextEdge].triangle = e[e[edge].sibling].triangle;
	//	////
	//	e[e[e[e[edge].sibling].nextEdge].nextEdge].nextEdge = e[edge].nextEdge;
	//	e[e[e[edge].nextEdge].nextEdge].nextEdge = e[e[edge].sibling].nextEdge;
	//	////
	//	
	//	tempE = e[e[edge].nextEdge].nextEdge;
	//	e[e[edge].nextEdge].nextEdge = edge;

	//	tempE2 = e[e[e[edge].sibling].nextEdge].nextEdge;
	//	e[e[e[edge].sibling].nextEdge].nextEdge = e[edge].sibling;

	//	e[e[edge].sibling].nextEdge = tempE;
	//	e[edge].nextEdge = tempE2;

	//	e[edge].vertex = e[e[e[edge].sibling].nextEdge].vertex;
	//	e[e[edge].sibling].vertex = e[e[edge].nextEdge].vertex;

	//	vertexEPtr[e[e[e[edge].nextEdge].nextEdge].vertex] = e[edge].nextEdge;
	//	vertexEPtr[e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex] = e[e[edge].sibling].nextEdge;
	//	//cout << "flip" << endl;
	//	//pass on
	//	//edge = e[edge].nextEdge;
	//}
	//else 
	//{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// create new vertex point
	vert1 = e[edge].vertex;
	vert2 = e[e[edge].sibling].vertex;
	//vert3 = edge->nextEdge->nextEdge->vertex;
	//vert4 = edge->sibling->nextEdge->nextEdge->vertex;
	

	tempV = -vertexEPtr[0];
	vertexCap = max(vertexCap, tempV);

	vertexEPtr[0] = vertexEPtr[tempV];
	vertexArray[tempV].x = (vertexArray[vert1].x + vertexArray[vert2].x) / 2.0f;
	vertexArray[tempV].y = (vertexArray[vert1].y + vertexArray[vert2].y) / 2.0f;
	vertexArray[tempV].z = (vertexArray[vert1].z + vertexArray[vert2].z) / 2.0f;
	
	//tempV.x = (vertexArray[vert1].x + vertexArray[vert2].x) / 2.0f;// + vertexArray[vert3].x + vertexArray[vert4].x) / 4.0f;
	//tempV.y = (vertexArray[vert1].y + vertexArray[vert2].y) / 2.0f;// + vertexArray[vert3].y + vertexArray[vert4].y) / 4.0f;
	//tempV.z = (vertexArray[vert1].z + vertexArray[vert2].z) / 2.0f;// + vertexArray[vert3].z + vertexArray[vert4].z) / 4.0f;

	//vertexArray.push_back(tempV);

	//copy triangles
	tempT1 = -triEPtr[0];
	triEPtr[0] = triEPtr[tempT1];
	indexArray[tempT1].index[0] = indexArray[e[edge].triangle].index[0];
	indexArray[tempT1].index[1] = indexArray[e[edge].triangle].index[1];
	indexArray[tempT1].index[2] = indexArray[e[edge].triangle].index[2];
	//tempT.index[0] = indexArray[edge->triangle].index[0];
	//tempT.index[1] = indexArray[edge->triangle].index[1];
	//tempT.index[2] = indexArray[edge->triangle].index[2];
	//indexArray.push_back(tempT);
	tempT2 = -triEPtr[0];
	triangleCap = max(triangleCap, tempT2);
	triEPtr[0] = triEPtr[tempT2];
	indexArray[tempT2].index[0] = indexArray[e[e[edge].sibling].triangle].index[0];
	indexArray[tempT2].index[1] = indexArray[e[e[edge].sibling].triangle].index[1];
	indexArray[tempT2].index[2] = indexArray[e[e[edge].sibling].triangle].index[2];
	//tempT.index[0] = indexArray[edge->sibling->triangle].index[0];
	//tempT.index[1] = indexArray[edge->sibling->triangle].index[1];
	//tempT.index[2] = indexArray[edge->sibling->triangle].index[2];
	//indexArray.push_back(tempT);

	// rebind old triangles
	for (int i = 0; i < 3; i++) {
		if (indexArray[e[edge].triangle].index[i] == e[edge].vertex)
			indexArray[e[edge].triangle].index[i] = tempV;

		if (indexArray[e[e[edge].sibling].triangle].index[i] == e[edge].vertex)
			indexArray[e[e[edge].sibling].triangle].index[i] = tempV;
	}

	// rebind new triangles
	for (int i = 0; i < 3; i++) {
		if (indexArray[tempT1].index[i] == e[e[edge].sibling].vertex)
			indexArray[tempT1].index[i] = tempV;

		if (indexArray[tempT2].index[i] == e[e[edge].sibling].vertex)
			indexArray[tempT2].index[i] = tempV;
	}
	
	/////////////////////////////////////////////////////////
	/*nextEdge*/tempE2 = tempE = -e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	triEPtr[tempT1] = tempE;
	vertexEPtr[tempV] = tempE;

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
	/*
	triEPtr.push_back(new halfEdge);
	vert1 = triEPtr.size() - 1;
	vertexEPtr.push_back(triEPtr[vert1]);

	triEPtr[vert1]->vertex = edge->vertex;
	triEPtr[vert1]->triangle = vert1;
	triEPtr[vert1]->nextEdge = new halfEdge;

	triEPtr[vert1]->nextEdge->vertex = vertexArray.size() - 1;
	triEPtr[vert1]->nextEdge->triangle = vert1;
	triEPtr[vert1]->nextEdge->sibling = edge->nextEdge->nextEdge;
	triEPtr[vert1]->nextEdge->nextEdge = new halfEdge;

	triEPtr[vert1]->nextEdge->nextEdge->vertex = edge->nextEdge->nextEdge->vertex;
	triEPtr[vert1]->nextEdge->nextEdge->triangle = vert1;
	triEPtr[vert1]->nextEdge->nextEdge->nextEdge = triEPtr[vert1];
	triEPtr[vert1]->nextEdge->nextEdge->sibling = edge->nextEdge->nextEdge->sibling;
	//rebind sibling of existing triangle
	edge->nextEdge->nextEdge->sibling->sibling = triEPtr[vert1]->nextEdge->nextEdge;
	//rebind sibling of old triangle
	edge->nextEdge->nextEdge->sibling = triEPtr[vert1]->nextEdge;
	*/

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
	/*
	triEPtr.push_back(new halfEdge);
	vert1 = triEPtr.size() - 1;

	triEPtr[vert1]->sibling = triEPtr[vert1 - 1];
	//bind sibling of first newEdge
	triEPtr[vert1 - 1]->sibling = triEPtr[vert1];
	//continue
	triEPtr[vert1]->vertex = vertexArray.size() - 1;
	triEPtr[vert1]->triangle = vert1;
	triEPtr[vert1]->nextEdge = new halfEdge;

	triEPtr[vert1]->nextEdge->vertex = edge->vertex;
	triEPtr[vert1]->nextEdge->triangle = vert1;
	triEPtr[vert1]->nextEdge->sibling = edge->sibling->nextEdge->sibling;
	//rebind sibling of existing triangle
	edge->sibling->nextEdge->sibling->sibling = triEPtr[vert1]->nextEdge;
	triEPtr[vert1]->nextEdge->nextEdge = new halfEdge;

	triEPtr[vert1]->nextEdge->nextEdge->vertex = edge->sibling->nextEdge->nextEdge->vertex;
	triEPtr[vert1]->nextEdge->nextEdge->triangle = vert1;
	triEPtr[vert1]->nextEdge->nextEdge->sibling = edge->sibling->nextEdge; // =======
	//rebind sibling of old triangle
	edge->sibling->nextEdge->sibling = triEPtr[vert1]->nextEdge->nextEdge;
	triEPtr[vert1]->nextEdge->nextEdge->nextEdge = triEPtr[vert1];
	*/

	// rebind old edge vertex
	vertexEPtr[e[edge].vertex] = tempE2;
	//	vertexEPtr[edge->vertex] = triEPtr[vert1];

	e[edge].vertex = tempV;
	e[e[e[edge].sibling].nextEdge].vertex = tempV;
	//	edge->vertex = vertexArray.size() - 1;
	//edge->sibling->nextEdge->vertex = vertexArray.size() - 1;

	// Update normal /////////////////////////////////////////////////////////////////////////////
	tempE = vertexEPtr[tempV];
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
	} while (tempE != vertexEPtr[tempV]);

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
	
	//}

	
}

void Mesh::edgeFlip(int &edge)
{
	int tempE; int tempE2;

	int vert1 = e[e[e[edge].nextEdge].nextEdge].vertex;
	int vert2 = e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex;

	//check if the vertices that are about to be connected are not already connected

	//if (edge != e[e[e[e[e[e[edge].nextEdge].sibling].nextEdge].sibling].nextEdge].sibling && edge != e[e[e[e[e[e[edge].sibling].nextEdge].sibling].nextEdge].sibling].nextEdge)
	//	return;

	tempE2 = e[edge].nextEdge;
	tempE = e[e[tempE2].nextEdge].sibling;
	while (tempE != tempE2)
	{
		if (e[tempE].vertex == vert2){
              			return;
		}
		tempE = e[e[tempE].nextEdge].sibling;
	}
	//--------------------

	//perform flip
	for (int i = 0; i < 3; i++) {
		if (indexArray[e[edge].triangle].index[i] == e[edge].vertex)
			indexArray[e[edge].triangle].index[i] = vert2;

		if (indexArray[e[e[edge].sibling].triangle].index[i] == e[e[edge].sibling].vertex)
			indexArray[e[e[edge].sibling].triangle].index[i] = vert1;
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

	vertexEPtr[e[e[e[edge].nextEdge].nextEdge].vertex] = e[edge].nextEdge;
	vertexEPtr[e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex] = e[e[edge].sibling].nextEdge;
	cout << "flip" << endl;
	//pass on
	//edge = e[edge].nextEdge;
}
void Mesh::edgeCollapse(bool recursive, int &edge) {

	int tempE; int tempEnd;
	int tempE2; int tempEnd2;
	static int currVert; 
	static int nVert;
	static int ndVert;
	int ctr = 0;


	//FÖRSTA FÖRSÖKET

	//if (tempE == tempE->nextEdge->nextEdge->sibling->nextEdge->nextEdge->sibling->nextEdge->nextEdge->sibling)
	//if (edge = edge->nextEdge->nextEdge->sibling->nextEdge->sibling->nextEdge->sibling->nextEdge->nextEdge)
	
	//if (edge == e[e[e[e[e[e[e[e[e[edge].nextEdge].sibling].nextEdge].nextEdge].sibling].nextEdge].nextEdge].sibling].nextEdge)
	//{
	//	std::cout << "1" << endl;
 //       tempE = e[e[e[edge].nextEdge].nextEdge].sibling;
	//	edgeCollapse(true, e[e[edge].nextEdge].sibling);
	//	edge = tempE;
	//	std::cout << "11" << endl;
	//}
	////if (tempE2 == tempE2->nextEdge->nextEdge->sibling->nextEdge->nextEdge->sibling->nextEdge->nextEdge->sibling)
	//if (e[edge].sibling == e[e[e[e[e[e[e[e[e[e[edge].sibling].nextEdge].sibling].nextEdge].nextEdge].sibling].nextEdge].nextEdge].sibling].nextEdge)
	//{
	//	std::cout << "2" << endl;
 //       edgeCollapse(true, e[e[e[edge].sibling].nextEdge].sibling);
	//	std::cout << "22" << endl;
	//}
	
	////ANDRA FÖRSÖKET
	currVert = e[e[edge].sibling].vertex;
   	nVert = e[edge].vertex;
	//ndVert = e[e[e[edge].nextEdge].nextEdge].vertex; bool ndSuccess = false;
	//
	//tempE = e[e[edge].sibling].nextEdge;
	//while (tempE != edge)
	//{
	//	if (e[e[tempE].sibling].vertex == currVert){
	//		edgeFlip(tempE); currSuccess = true;
 //         	edgeFlip(edge);
	//		e[e[edge].sibling].triangle;
	//		e[edge].triangle;
	//		cout << "yo1";
	//		return; 
	//	}
	//	else if (e[e[tempE].sibling].vertex == ndVert) {
	//		//edgeFlip(tempE); ndSuccess = true;
	//		//edgeFlip(e[e[edge].nextEdge].nextEdge);
	//		cout << "yo2";
	//		return; 
	//	}
	//	tempE = e[e[tempE].sibling].nextEdge;

	//}
	//if (currSuccess)
	//	edgeFlip(edge);
	//if (ndSuccess)
	//	edgeFlip(e[e[edge].nextEdge].nextEdge);
	//if (currSuccess || ndSuccess)
	//	return;

	//TREDJE FÖRSÖKET
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
		return;

	// rebind edges that point to nVert
	tempE = e[e[e[edge].nextEdge].nextEdge].sibling;
	while (tempE != e[e[edge].sibling].nextEdge)
	{
		e[tempE].vertex = currVert;

		// rebind the triangles containing nVert as index
		for (int i = 0; i < 3; i++) {
			if (indexArray[e[tempE].triangle].index[i] == nVert) {
				indexArray[e[tempE].triangle].index[i] = currVert;
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
	vertexEPtr[currVert] = e[e[edge].nextEdge].sibling;
	vertexEPtr[e[e[e[edge].nextEdge].nextEdge].vertex] = e[e[e[edge].nextEdge].nextEdge].sibling;
	vertexEPtr[e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex] = e[e[e[e[edge].sibling].nextEdge].nextEdge].sibling;


	//reset the removed triangles
	tempE2 = e[edge].triangle;
	
	indexArray[tempE2].index[0] = 0;
	indexArray[tempE2].index[1] = 0;
	indexArray[tempE2].index[2] = 0;
	tempE2 = e[e[edge].sibling].triangle;
	indexArray[tempE2].index[0] = 0;
	indexArray[tempE2].index[1] = 0;
	indexArray[tempE2].index[2] = 0;
	nrofTris = nrofTris - 2;
	                                                                                                                
	// reset the removed vertex
	vertexArray[nVert].x = 0.2f;
	vertexArray[nVert].y = 0.2f;
	vertexArray[nVert].z = 0.2f;
	vertexArray[nVert].nx = 0;
	vertexArray[nVert].ny = 0;
	vertexArray[nVert].nz = 0;
	vertexArray[nVert].selected = 0;
	nrofVerts--;

	// reset edge pointers
	vertexEPtr[nVert] = vertexEPtr[0];
	vertexEPtr[0] = -nVert;
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

	//if (!recursive)
	//	edge = tempE;
}
