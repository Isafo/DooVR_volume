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
	halfEdge* tempE1;
	int tempIn1;
	int tempIn2;
	bool success = true;

	float tempP1[3]; float tempP2[3] = {0.0f, 0.0f, 0.0f};
	int tempSize = 0;
	float testLength = 0.0f;

	vector<int> changedVertices;
	changedVertices.reserve(10000);

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

	float scaleF = 1.0f / (ROWS * 2);

	nrofVerts = 6;
	nrofTris = 8;

	for (int i = 0; i < 1000000; i++)
	{
		vertexEPtr[i] = i + 1;
	}
	vertexEPtr[1000000 - 1] = -1;
	vertexEPtr[0] = 7;
	for (int i = 0; i < 2000000; i++)
	{
		triEPtr[i] = i + 1;
	}
	triEPtr[2000000 - 1] = -1;
	triEPtr[0] = 9;
	for (int i = 0; i < 3000000; i++)
	{
		e[i].nextEdge = i + 1;
	}
	e[3000000 - 1].nextEdge = -1;
	e[0].nextEdge = 25;

	
	// place vertecies
	// Y 0
	vertexArray[1].x = 0.0f;	vertexArray[1].y = MAX_LENGTH / 2.0f;	vertexArray[1].z = 0.0f; /*normal*/	vertexArray[1].nx = 0.0f;	vertexArray[1].ny = MAX_LENGTH / 2.0f;	vertexArray[1].nz = 0.0f;

	vertexArray[2].x = 0.0f;	vertexArray[2].y = -MAX_LENGTH / 2.0f;	vertexArray[2].z = 0.0f; /*normal*/	vertexArray[2].nx = 0.0f;	vertexArray[2].ny = -MAX_LENGTH / 2.0f;	vertexArray[2].nz = 0.0f;

	// X 2
	vertexArray[3].x = MAX_LENGTH / 2.0f;	vertexArray[3].y = 0.0f;	vertexArray[3].z = 0.0f; /*normal*/	vertexArray[3].nx = MAX_LENGTH / 2.0f;	vertexArray[3].ny = 0.0f;	vertexArray[3].nz = 0.0f;

	vertexArray[4].x = -MAX_LENGTH / 2.0f;	vertexArray[4].y = 0.0f;	vertexArray[4].z = 0.0f; /*normal*/	vertexArray[4].nx = -MAX_LENGTH / 2.0f;	vertexArray[4].ny = 0.0f;	vertexArray[4].nz = 0.0f;

	// Z 4
	vertexArray[5].x = 0.0f;	vertexArray[5].y = 0.0f;	vertexArray[5].z = -MAX_LENGTH / 2.0f; /*normal*/	vertexArray[5].nx = 0.0f;	vertexArray[5].ny = 0.0f;	vertexArray[5].nz = -MAX_LENGTH / 2.0f;
	
	vertexArray[6].x = 0.0f;	vertexArray[6].y = 0.0f;	vertexArray[6].z = MAX_LENGTH / 2.0f; /*normal*/	vertexArray[6].nx = 0.0f;	vertexArray[6].ny = 0.0f;	vertexArray[6].nz = MAX_LENGTH / 2.0f;

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
	/*
	while (success)
	{
		
		success = false;
		tempSize = vertexArray.size() / 2;
		tempP2[0] = vertexArray[tempSize].x;
		tempP2[1] = vertexArray[tempSize].y;
		tempP2[2] = vertexArray[tempSize].z;

		for (int j = 0; j < vertexArray.size(); j++)
		{
			tempP1[0] = vertexArray[j].x; tempP1[1] = vertexArray[j].y; tempP1[2] = vertexArray[j].z;
			testLength = linAlg::vecLength(tempP1);
			if (vertexEPtr[j] != nullptr && testLength < rad)
			{
				success = true;
				linAlg::normVec(tempP1);

				vertexArray[j].x += tempP1[0] * MIN_LENGTH;
				vertexArray[j].y += tempP1[1] * MIN_LENGTH;
				vertexArray[j].z += tempP1[2] * MIN_LENGTH;

				vertexEPtr[j]->needsUpdate = true;
				tempE1 = vertexEPtr[j]->nextEdge->sibling;
				while (tempE1 != vertexEPtr[j]) {
					tempE1->needsUpdate = true;
					tempE1 = tempE1->nextEdge->sibling;
				}

				changedVertices.push_back(j);
			}
		}
		
		updateArea(&changedVertices[0], changedVertices.size());
	}
	*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	vector<triangle> tempList;
	triangle tempTri;

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
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normals

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

void Mesh::sculpt(float* p, float lp[3], float rad, bool but) {
	float newWPoint[4];
	float tempVec1[3]; float tempVec2[3];
	float wPoint[4]; float vPoint[3]; float vPoint2[3];
	int index; int index2;

	vector<int> changedVertices;
	changedVertices.reserve(100);
	int changeCount = 0;

	triangle* indexP;
	vertex* vertexP;
	int tempEdge;

	bool success = false;

	//MOVEMENT BETWEEN LAST FRAME AND THIS FRAME
	float mLength = 0.0f;

	wPoint[0] = p[0] - position[0];
	wPoint[1] = p[1] - position[1];
	wPoint[2] = p[2] - position[2];
	wPoint[3] = 1.0f;

	linAlg::vectorMatrixMult(orientation, wPoint, newWPoint);
	//tempvec = glm::transpose(glm::make_mat4(orientation)) * glm::vec4(wPoint[0], wPoint[1], wPoint[2], 1.0f);
	//wPoint[0] = tempvec.x;
	//wPoint[1] = tempvec.y;
	//wPoint[2] = tempvec.z;

	for (int i = 1; i <= nrofVerts; i++) {
		vPoint[0] = vertexArray[i].x;
		vPoint[1] = vertexArray[i].y;
		vPoint[2] = vertexArray[i].z;
		tempVec1[0] = vPoint[0] - newWPoint[0];
		tempVec1[1] = vPoint[1] - newWPoint[1];
		tempVec1[2] = vPoint[2] - newWPoint[2];

		mLength = linAlg::vecLength(tempVec1);
		if (mLength < rad) {
   			changedVertices.push_back(i);
			//normVec(tempVec1);
			//mLength = 0.002f*(0.05f / (mLength + 0.05f));
			linAlg::normVec(tempVec1);

			//tempVec2[0] = vertexArray[i].nx;
			//tempVec2[1] = vertexArray[i].ny;
			//tempVec2[2] = vertexArray[i].nz;

			//if (linAlg::dotProd(tempVec1, tempVec2) > 0){
				vertexArray[i].x = newWPoint[0] + tempVec1[0]  *rad;
				vertexArray[i].y = newWPoint[1] + tempVec1[1] * rad;
				vertexArray[i].z = newWPoint[2] + tempVec1[2] * rad;
				//vertexArray[i].x = vertexArray[i].x + vertexArray[i].nx * (rad - mLength);
				//vertexArray[i].y = vertexArray[i].y + vertexArray[i].ny * (rad - mLength);
				//vertexArray[i].z = vertexArray[i].z + vertexArray[i].nz * (rad - mLength);
			//}
			//else{
			//	vertexArray[i].x = newWPoint[0] + tempVec1[0] * rad;
			//	vertexArray[i].y = newWPoint[1] + tempVec1[1] * rad;
			//	vertexArray[i].z = newWPoint[2] + tempVec1[2] * rad;
			//}

			for (int j = 0; j < changedVertices.size(); j++) {
				
				index2 = changedVertices[j];
				
				tempEdge = vertexEPtr[index2];
				
				do {
					if (!e[tempEdge].needsUpdate){
						index = e[tempEdge].vertex;

						e[e[tempEdge].sibling].needsUpdate = true;
						e[tempEdge].needsUpdate = true;

						vPoint2[0] = vertexArray[index].x;
						vPoint2[1] = vertexArray[index].y;
						vPoint2[2] = vertexArray[index].z;
						tempVec1[0] = vPoint2[0] - newWPoint[0];
						tempVec1[1] = vPoint2[1] - newWPoint[1];
						tempVec1[2] = vPoint2[2] - newWPoint[2];

						mLength = linAlg::vecLength(tempVec1);

						if (mLength < rad)
						{
							linAlg::normVec(tempVec1);
							vertexArray[index].x = newWPoint[0] + tempVec1[0] * rad;
							vertexArray[index].y = newWPoint[1] + tempVec1[1] * rad;
							vertexArray[index].z = newWPoint[2] + tempVec1[2] * rad;
							
							changedVertices.push_back(index);
						}
					}
					tempEdge = e[e[tempEdge].nextEdge].sibling;

				} while (tempEdge != vertexEPtr[index2]);
			}
			success = true;
			break;
		}
	}

	changeCount = 0;
	if (changedVertices.size() > 0)
		updateArea(&changedVertices[0], changedVertices.size());

	if (success == true) {

		vertexP = &vertexArray[0];


		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		vertexP = (vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(vertex)*nrofVerts,
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

		for (int i = 1; i <= nrofVerts; i++) {
			vertexP[i].x = vertexArray[i].x;
			vertexP[i].y = vertexArray[i].y;
			vertexP[i].z = vertexArray[i].z;
			vertexP[i].nx = vertexArray[i].nx;
			vertexP[i].ny = vertexArray[i].ny;
			vertexP[i].nz = vertexArray[i].nz;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		// Specify how many attribute arrays we have in our VAO
		glEnableVertexAttribArray(0); // Vertex coordinates
		glEnableVertexAttribArray(1); // Normals

		// Specify how OpenGL should interpret the vertex buffer data:
		// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
		// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
		// Type GL_FLOAT
		// Not normalized (GL_FALSE)
		// Stride 8 (interleaved array with 8 floats per vertex)
		// Array buffer offset 0, 3, 6 (offset into first vertex)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			6 * sizeof(GLfloat), (void*)0); // xyz coordinates
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normals

		// Activate the index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

		//vector<triangle> tempList;
		//tempList.reserve(indexArray.size());
		//indexP = &tempList[0];


		//glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		//	sizeof(triangle)*indexArray.size(), &indexArray, GL_STREAM_DRAW);

		// Present our vertex <indices to OpenGL
		indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(triangle) * nrofTris,
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

		for (int i = 1; i <= nrofTris; i++) {
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
}

vertex* Mesh::getVertexList() {
	return &vertexArray[0];
}

triangle* Mesh::getIndexList() {
	return &indexArray[0];
}

void Mesh::render() {
	glBindVertexArray(vao);
	//glColor3f(color.x, color.y, color.z);

	glDrawElements(GL_TRIANGLES, nrofTris * sizeof(triangle), GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex uN, type, element array buffer offset)
	glBindVertexArray(0);
}


void Mesh::updateArea(int* changeList, int listSize) {

	static float vPoint1[3], vPoint2[3], vPoint3[3], vPoint4[3];
	static float tempVec1[3], tempVec2[3], tempVec3[3];
	static float tempNorm1[3] = { 0.0f, 0.0f, 0.0f };
	static float tempNorm2[3] = { 0.0f, 0.0f, 0.0f };
	int tempEdge; halfEdge* tempE;
	static int vert1, vert2; 
	static float edgeLength;

	for (int i = 0; i < listSize; i++) {

		
		vPoint1[0] = vertexArray[changeList[i]].x;
		vPoint1[1] = vertexArray[changeList[i]].y;
		vPoint1[2] = vertexArray[changeList[i]].z;

		 // Retriangulation //////////////////////////////////////////////////////////////////////////////////////////
		 // check if edge needs update
		if (vertexEPtr[changeList[i]] != 0)
			tempEdge = vertexEPtr[changeList[i]];
		else
			continue;

		 do {
			 if (e[tempEdge].needsUpdate == true) {
				 e[tempEdge].needsUpdate = false;
				 e[e[tempEdge].sibling].needsUpdate = false;

				// calculate edge lenght
				 vert1 = e[tempEdge].vertex;

				vPoint2[0] = vertexArray[vert1].x;
				vPoint2[1] = vertexArray[vert1].y;
				vPoint2[2] = vertexArray[vert1].z;
				 
				linAlg::calculateVec(tempVec1, vPoint2, vPoint1);
                edgeLength = linAlg::vecLength(tempVec1);
				
				// check if edge is to long/short
				if (edgeLength < MIN_LENGTH) {
					cout << "yo";
					//tempE = tempEdge
					edgeCollapse(false, tempEdge);
					tempEdge = e[e[e[tempEdge].sibling].nextEdge].nextEdge;
					//edge already incremented, something needs to be done.
				} else if (edgeLength > MAX_LENGTH) {
					edgeSplit(vPoint1, tempVec1, tempEdge);
				}
			 }
			
			 tempEdge = e[e[tempEdge].nextEdge].sibling;

		 } while (e[tempEdge].needsUpdate == true);
			 
		 // Update normal /////////////////////////////////////////////////////////////////////////////
		 if (vertexEPtr[changeList[i]] != 0)
			 tempEdge = vertexEPtr[changeList[i]];
		 // loop through the rest of the edges
		 do {
			 vert1 = e[tempEdge].vertex;
			 vert2 = e[e[e[tempEdge].nextEdge].nextEdge].vertex;

			 vPoint2[0] = vertexArray[vert1].x;
			 vPoint2[1] = vertexArray[vert1].y;
			 vPoint2[2] = vertexArray[vert1].z;

			 vPoint3[0] = vertexArray[vert2].x;
			 vPoint3[1] = vertexArray[vert2].y;
			 vPoint3[2] = vertexArray[vert2].z;

			 linAlg::calculateVec(tempVec1, vPoint2, vPoint1);
			 linAlg::calculateVec(tempVec2, vPoint3, vPoint1);

			 linAlg::crossProd(tempNorm1, tempVec2, tempVec1);

			 linAlg::normVec(tempNorm1);

			 tempNorm2[0] += tempNorm1[0];
			 tempNorm2[1] += tempNorm1[1];
			 tempNorm2[2] += tempNorm1[2];


			 tempEdge = e[e[tempEdge].nextEdge].sibling;
		 } while (tempEdge != vertexEPtr[changeList[i]]);

		 edgeLength = linAlg::vecLength(tempNorm2);

		 tempNorm2[0] = tempNorm2[0] / edgeLength;
		 tempNorm2[1] = tempNorm2[1] / edgeLength;
		 tempNorm2[2] = tempNorm2[2] / edgeLength;

		 linAlg::normVec(tempNorm2);

                            		 vertexArray[changeList[i]].nx = tempNorm2[0];
		 vertexArray[changeList[i]].ny = tempNorm2[1];
		 vertexArray[changeList[i]].nz = tempNorm2[2];
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

	//TODO: FLIP EDGE, NOT HANDLED PROPERLY BUT SHOULD BE////////////////////////////////////
	
   	vert1 = e[e[e[edge].nextEdge].nextEdge].vertex;
	vert2 = e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex;

	temp[0] = vertexArray[vert1].x;
	temp[1] = vertexArray[vert1].y;
	temp[2] = vertexArray[vert1].z;

	temp2[0] = vertexArray[vert2].x;
	temp2[1] = vertexArray[vert2].y;
	temp2[2] = vertexArray[vert2].z;
	linAlg::calculateVec(temp3, temp, temp2);
	linAlg::vecLength(temp3);
	///*
	if (linAlg::vecLength(temp3) < MAX_LENGTH && edge != e[e[e[e[e[e[edge].nextEdge].sibling].nextEdge].sibling].nextEdge].sibling && edge != e[e[e[e[e[e[edge].sibling].nextEdge].sibling].nextEdge].sibling].nextEdge) {
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

		//pass on
		edge = e[edge].nextEdge;
	}
	else
	{//*/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// create new vertex point
	vert1 = e[edge].vertex;
	vert2 = e[e[edge].sibling].vertex;
	//vert3 = edge->nextEdge->nextEdge->vertex;
	//vert4 = edge->sibling->nextEdge->nextEdge->vertex;

	tempV = vertexEPtr[0];
	vertexEPtr[0] = vertexEPtr[tempV];
	vertexArray[tempV].x = (vertexArray[vert1].x + vertexArray[vert2].x) / 2.0f;
	vertexArray[tempV].y = (vertexArray[vert1].y + vertexArray[vert2].y) / 2.0f;
	vertexArray[tempV].z = (vertexArray[vert1].z + vertexArray[vert2].z) / 2.0f;

	//tempV.x = (vertexArray[vert1].x + vertexArray[vert2].x) / 2.0f;// + vertexArray[vert3].x + vertexArray[vert4].x) / 4.0f;
	//tempV.y = (vertexArray[vert1].y + vertexArray[vert2].y) / 2.0f;// + vertexArray[vert3].y + vertexArray[vert4].y) / 4.0f;
	//tempV.z = (vertexArray[vert1].z + vertexArray[vert2].z) / 2.0f;// + vertexArray[vert3].z + vertexArray[vert4].z) / 4.0f;

	//vertexArray.push_back(tempV);

	//copy triangles
	tempT1 = triEPtr[0];
	triEPtr[0] = triEPtr[tempT1];
	indexArray[tempT1].index[0] = indexArray[e[edge].triangle].index[0];
	indexArray[tempT1].index[1] = indexArray[e[edge].triangle].index[1];
	indexArray[tempT1].index[2] = indexArray[e[edge].triangle].index[2];
	//tempT.index[0] = indexArray[edge->triangle].index[0];
	//tempT.index[1] = indexArray[edge->triangle].index[1];
	//tempT.index[2] = indexArray[edge->triangle].index[2];
	//indexArray.push_back(tempT);
	tempT2 = triEPtr[0];
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
	tempE2 = tempE = e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	triEPtr[tempT1] = tempE;
	vertexEPtr[tempV] = tempE;

	e[tempE].vertex = e[edge].vertex;
	e[tempE].triangle = tempT1;
	/*nextEdge*/tempE = e[tempE].nextEdge = e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	e[tempE].vertex = tempV;
	e[tempE].triangle = tempT1;
	e[tempE].sibling = e[e[edge].nextEdge].nextEdge;
	/*nextEdge*/tempE = e[tempE].nextEdge = e[0].nextEdge;
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

	tempE2 = tempE = e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	triEPtr[tempT2] = tempE;

	e[tempE].sibling = triEPtr[tempT1];
	//bind sibling of first newEdge
	e[triEPtr[tempT1]].sibling = tempE;
	//continue
	e[tempE].vertex = tempV;
	e[tempE].triangle = tempT2;
	/*nextedge*/tempE = e[tempE].nextEdge = e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	e[tempE].vertex = e[edge].vertex;
	e[tempE].triangle = tempT2;
	e[tempE].sibling = e[e[e[edge].sibling].nextEdge].sibling;
	//rebind sibling of existing triangle
	e[e[e[e[edge].sibling].nextEdge].sibling].sibling = e[tempE2].nextEdge;
	/*nextedge*/tempE = e[tempE].nextEdge = e[0].nextEdge;
	e[0].nextEdge = e[tempE].nextEdge;

	e[tempE].vertex = e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex;
	e[tempE].triangle = tempT2;
	e[tempE].sibling = e[e[edge].sibling].nextEdge;
	//rebind sibling of old triangle
	e[e[e[edge].sibling].nextEdge].sibling = tempE;
	e[tempE].nextEdge = tempE2;
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

		linAlg::calculateVec(tempVec1, temp2, temp);
		linAlg::calculateVec(tempVec2, temp3, temp);

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

	linAlg::normVec(tempNorm2);
	vertexArray[tempV].nx = tempNorm2[0];
	vertexArray[tempV].ny = tempNorm2[1];
	vertexArray[tempV].nz = tempNorm2[2];

	}

	nrofVerts++;
	nrofTris = nrofTris + 2;
}

void Mesh::edgeCollapse(bool recursive, int &edge) {

	int tempE;
	int tempE2;
	static int currVert; 
	static int nVert;
    currVert = e[e[edge].sibling].vertex;
	nVert = e[edge].vertex;
	bool edgeRemoved = false;

	//tempE = edge->nextEdge->sibling;
	//tempE2 = edge->sibling->nextEdge->sibling;

	//if (tempE == tempE->nextEdge->nextEdge->sibling->nextEdge->nextEdge->sibling->nextEdge->nextEdge->sibling)
	//if (edge = edge->nextEdge->nextEdge->sibling->nextEdge->sibling->nextEdge->sibling->nextEdge->nextEdge)
	while (edge == e[e[e[e[e[e[e[e[e[edge].nextEdge].sibling].nextEdge].nextEdge].sibling].nextEdge].nextEdge].sibling].nextEdge)
	{
      	tempE = e[e[e[edge].nextEdge].nextEdge].sibling;
		edgeCollapse(true, e[e[edge].nextEdge].sibling);
		edge = tempE;
	}
	//if (tempE2 == tempE2->nextEdge->nextEdge->sibling->nextEdge->nextEdge->sibling->nextEdge->nextEdge->sibling)
	while (e[edge].sibling == e[e[e[e[e[e[e[e[e[e[edge].sibling].nextEdge].sibling].nextEdge].nextEdge].sibling].nextEdge].nextEdge].sibling].nextEdge)
	{
		edgeCollapse(true, e[e[e[edge].sibling].nextEdge].sibling);
	}

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

	// reset the removed vertex
	vertexArray[nVert].x = -1000;
	vertexArray[nVert].y = -1000;
	vertexArray[nVert].z = -1000;
	vertexArray[nVert].nx = 0;
	vertexArray[nVert].ny = 0;
	vertexArray[nVert].nz = 0;

	// reset edge pointers
	vertexEPtr[nVert] = vertexEPtr[0];
	vertexEPtr[0] = nVert;
	//
	triEPtr[e[edge].triangle] = triEPtr[0];
	triEPtr[e[e[edge].sibling].triangle] = e[edge].triangle;
	triEPtr[0] = e[e[edge].sibling].triangle;

	// delete the removed edges

	e[e[e[e[edge].sibling].nextEdge].nextEdge].nextEdge = e[0].nextEdge;
	e[e[e[edge].nextEdge].nextEdge].nextEdge = e[edge].sibling;
	e[0].nextEdge = edge;

	if (!recursive)
		edge = tempE;
}
