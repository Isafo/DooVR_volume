#include "Drag.h"


#define EPSILON 0.000001

Drag::Drag(DynamicMesh* mesh, Wand* wand)
{
	selectedVertices = new int[MAX_SELECTED]; selectedSize = 0;
	previouslySelectedVertices = new int[MAX_SELECTED]; previouslySelectedSize = 0;

	radius = 0.01f;
	toolBrush = new Circle(0.0f, 0.0f, 0.0f, 1.0f);
	pointer = new Line(0.0f, 0.0f, 0.0f, 0.1f);
	iCircle = new Circle(0.0f, 0.0f, 0.0f, 1.0f);

	lineOffset[0] = 0.0f;
	lineOffset[1] = 0.0f;
	lineOffset[2] = 0.1f;

	mVertexArray = mesh->vertexArray;
	mVInfoArray = mesh->vInfoArray;
	mEdgeArray = mesh->e;
	mPosition = mesh->position;
	mOrientation = mesh->orientation;
	mMAX_LENGTH = mesh->MAX_LENGTH;

	linAlg::transpose(mOrientation);
	//--< 1.0 | calculated the position and direction of the wand
	wand->getPosition(wPoint);
	wPoint[0] = wPoint[0] - mPosition[0];
	wPoint[1] = wPoint[1] - mPosition[1];
	wPoint[2] = wPoint[2] - mPosition[2];
	wPoint[3] = 1.0f;
	linAlg::vectorMatrixMult(mOrientation, wPoint, newWPoint);

	wand->getDirection(Dirr);
	linAlg::normVec(Dirr);
	Dirr[3] = 1.0f;
	linAlg::vectorMatrixMult(mOrientation, Dirr, newDirr);
	linAlg::transpose(mOrientation);
	mIndex = -1;

	zVec[0] = 0.0f;  zVec[1] = 0.0f; zVec[2] = 1.0f;
}


Drag::~Drag()
{
	delete selectedVertices;
	delete previouslySelectedVertices;
	delete toolBrush;
	delete pointer;
	delete iCircle;
}

void Drag::renderIntersection(MatrixStack* MVstack, GLint locationMV)
{

	linAlg::crossProd(tempVec, intersection.nxyz, zVec);
	linAlg::normVec(tempVec);
	linAlg::rotAxis(tempVec, acos(linAlg::dotProd(intersection.nxyz, zVec)), iTransform);

	//iCircle->setOrientation(iTransform);
	//iCircle->setPosition(intersection.xyz);

	MVstack->push();

	MVstack->translate(intersection.xyz);
	MVstack->multiply(iTransform);
	MVstack->scale(radius);
	glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack->getCurrentMatrix());
	iCircle->render();
	MVstack->pop();
}

void Drag::render(MatrixStack* MVstack, GLint locationMV)
{

	MVstack->push();
	MVstack->translate(lineOffset);
	glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack->getCurrentMatrix());
	pointer->render();
	MVstack->pop();

	MVstack->push();
	MVstack->scale(radius);
	MVstack->translate(toolBrush->getPosition());
	glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack->getCurrentMatrix());
	toolBrush->render();
	MVstack->pop();
}


void Drag::firstSelect(DynamicMesh* mesh, Wand* wand)
{
	float wPoint[4]; float newWPoint[4]; float Dirr[4]; float newDirr[4];
	float eVec1[3]; float eVec2[3];
	float P[3]; float Q[3]; float T[3];
	float* vPoint; float* vPoint2; float vNorm[3]; float* vNorm2;
	int index; int index2;
	float* ptrVec;

	int tempEdge; int tempE;

	bool success = false; bool triB = false;

	float pLength = 0.0f; float invP = 0.0f; float u = 0.0f; float v = 0.0f; float t = 0.0f;
	float oLength = 0.0f;

	float mLength;

	intersection.nxyz[0] = 100.f; intersection.nxyz[1] = 100.f; intersection.nxyz[2] = 100.f;


	linAlg::transpose(mOrientation);
	//--< 1.0 | calculated the position and direction of the wand
	wand->getPosition(wPoint);
	wPoint[0] = wPoint[0] - mPosition[0];
	wPoint[1] = wPoint[1] - mPosition[1];
	wPoint[2] = wPoint[2] - mPosition[2];
	wPoint[3] = 1.0f;
	linAlg::vectorMatrixMult(mOrientation, wPoint, newWPoint);

	wand->getDirection(Dirr);
	linAlg::normVec(Dirr);
	Dirr[3] = 1.0f;
	linAlg::vectorMatrixMult(mOrientation, Dirr, newDirr);
	linAlg::transpose(mOrientation);

	// 1.0 >--------------------------
	//--< 2.0 | start searching through vertexarray for points that are within the brush
	for (int i = 0; i <= mesh->vertexCap; i++) {
		//--< 2.1 | calculate vector between vertexposition and wandposition

		linAlg::calculateVec(mVertexArray[i].xyz, newWPoint, eVec1);

		// 2.1 >---------------------
		//--< 2.2 | calculate orthogonal and parallel distance between this vector and wandDirection
		pLength = linAlg::dotProd(newDirr, eVec1);;//                                                 / linAlg::vecLength(newDirr);
		Dirr[0] = newDirr[0] * pLength;
		Dirr[1] = newDirr[1] * pLength;
		Dirr[2] = newDirr[2] * pLength;

		linAlg::calculateVec(eVec1, Dirr, eVec2);
		oLength = linAlg::vecLength(eVec2);
		// 2.2 >----------------------
		if (pLength > 0.0f && oLength < mMAX_LENGTH)
		{
			selectedVertices[selectedSize] = i; selectedSize++;
			success = true;
		}
	}
	// 2.0 >----------------------

	if (!success) 
	{
		deSelect();
		mesh->updateOGLData();
		return;
	}

	success = false;
	//===========================================================================================
	//sIt = sHead;
	for (int i = 0; i < selectedSize; i++) {
		index2 = selectedVertices[i];
		tempEdge = mVInfoArray[index2].edgePtr;

		do {
			tempE = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;

			if (mVInfoArray[mEdgeArray[tempEdge].vertex].selected != 1.0f)
			{
				linAlg::calculateVec(mVertexArray[mEdgeArray[tempEdge].vertex].xyz, mVertexArray[index2].xyz, eVec1);
				linAlg::calculateVec(mVertexArray[mEdgeArray[tempE].vertex].xyz, mVertexArray[index2].xyz, eVec2);
				linAlg::crossProd(P, newDirr, eVec2);

				pLength = linAlg::dotProd(eVec1, P);
				if (pLength < -EPSILON || pLength > EPSILON)
				{
					invP = 1.f / pLength;
					linAlg::calculateVec(newWPoint, mVertexArray[index2].xyz, T);

					u = linAlg::dotProd(T, P) * invP;
					if (u > 0.0f && u < 1.0f)
					{
						linAlg::crossProd(Q, T, eVec1);

						v = linAlg::dotProd(newDirr, Q)*invP;

						if (v > 0.0f && u + v < 1.0f)
						{
							t = linAlg::dotProd(eVec2, Q)*invP;
							if (t > EPSILON && t < 0.1f )
							{
								//sIt->next->index = e[tempEdge].triangle;
								tempVec[0] = newDirr[0] * t;
								tempVec[1] = newDirr[1] * t;
								tempVec[2] = newDirr[2] * t;
								tempE = mVInfoArray[index2].edgePtr;
								success = true;
								if (linAlg::vecLength(tempVec) < linAlg::vecLength(intersection.nxyz)){
									mIndex = index2;
									intersection.xyz[0] = newWPoint[0] + tempVec[0];
									intersection.xyz[1] = newWPoint[1] + tempVec[1];
									intersection.xyz[2] = newWPoint[2] + tempVec[2];
									intersection.nxyz[0] = tempVec[0];
									intersection.nxyz[1] = tempVec[1];
									intersection.nxyz[2] = tempVec[2];
									linAlg::crossProd(vNorm, eVec2, eVec1);
									linAlg::normVec(vNorm);
								}
							}
						}
					}
				}
			}
			tempEdge = tempE;

		} while (tempEdge != mVInfoArray[index2].edgePtr);

		mVInfoArray[index2].selected = 1.0f;
	}


	if (!success)
	{
		deSelect();
		mesh->updateOGLData();
		return;
	}
	
	success = false;
	//================================================================
	//for (int i = 0; i < selectedSize; i++)
	//{
	//	mVInfoArray[selectedVertices[i]].selected = 0.0f;
	//}
	selectedVertices[0] = mIndex;
	selectedSize = 1;
	mVInfoArray[mIndex].selected = 3.0f;

	midLength = linAlg::vecLength(intersection.nxyz);
	midPoint[0] = intersection.xyz[0];
	midPoint[1] = intersection.xyz[1];
	midPoint[2] = intersection.xyz[2];

	intersection.nxyz[0] = mVertexArray[mIndex].nxyz[0];
	intersection.nxyz[1] = mVertexArray[mIndex].nxyz[1];
	intersection.nxyz[2] = mVertexArray[mIndex].nxyz[2];

	vPoint = intersection.xyz;
	//vNorm = intersection.nxyz;

	spherePos[0] = vPoint[0] - vNorm[0] * radius;
	spherePos[1] = vPoint[1] - vNorm[1] * radius;
	spherePos[2] = vPoint[2] - vNorm[2] * radius;

	// --< 2.something
	// 2.3 >-----------------------
	//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
	for (int i = 0; i < selectedSize; i++) {
		index2 = selectedVertices[i];
		tempEdge = mVInfoArray[index2].edgePtr;

		do {
			if (mVInfoArray[mEdgeArray[tempEdge].vertex].selected != 4.0f){
				index = mEdgeArray[tempEdge].vertex;

				linAlg::calculateVec(mVertexArray[index].xyz, vPoint, eVec1);

				pLength = linAlg::dotProd(vNorm, eVec1);// / linAlg::vecLength(newDirr);
				Dirr[0] = vNorm[0] * pLength;
				Dirr[1] = vNorm[1] * pLength;
				Dirr[2] = vNorm[2] * pLength;

				linAlg::calculateVec(eVec1, Dirr, eVec2);
				oLength = linAlg::vecLength(eVec2);

				if (pLength < radius / 2.0f && pLength > -radius / 2.0f && oLength < radius) {
					selectedVertices[selectedSize] = index; selectedSize++;

					mVInfoArray[index].selected = 4.0f;
				}
			}
			tempEdge = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;

		} while (tempEdge != mVInfoArray[index2].edgePtr);

	}
/*		for (int i = 0; i < selectedSize; i++)
	{
		mVInfoArray[selectedVertices[i]].selected = 0.0f;
	}*/
	
	
	mesh->updateOGLData();
}


void Drag::moveVertices(DynamicMesh* mesh, Wand* wand, float dT)
{
	deSelect();

	float eVec1[3]; float eVec2[3];
	float P[3]; float Q[3]; float T[3]; float lengthVec[3];
	float* vPoint; float* vPoint2; float* vPoint1; float* vNorm; float* vNorm2;
	int index; int index2;
	float* ptrVec;

	float tempVec1[3];
	int tempEdge; int tempE;

	int tempSize;

	bool success = false; bool triB = false;

	float pLength = 0.0f; float invP = 0.0f; float u = 0.0f; float v = 0.0f; float t = 0.0f;
	float oLength = 0.0f;

	float mLength;

	int* usedList; int* emptyList;
	int usedSize; int emptySize;


	intersection.nxyz[0] = 100.f; intersection.nxyz[1] = 100.f; intersection.nxyz[2] = 100.f;

	linAlg::transpose(mOrientation);
	//--< 1.0 | calculated the position and direction of the wand
	wand->getPosition(wPoint);
	wPoint[0] = wPoint[0] - mPosition[0];
	wPoint[1] = wPoint[1] - mPosition[1];
	wPoint[2] = wPoint[2] - mPosition[2];
	wPoint[3] = 1.0f;
	linAlg::vectorMatrixMult(mOrientation, wPoint, newWPoint);

	wand->getDirection(Dirr);
	linAlg::normVec(Dirr);
	Dirr[3] = 1.0f;
	linAlg::vectorMatrixMult(mOrientation, Dirr, newDirr);
	linAlg::transpose(mOrientation);

	vPoint = mVertexArray[mIndex].xyz;
	vNorm = mVertexArray[mIndex].nxyz;
	//vNorm = newDirr;

	P[0] = newWPoint[0] + newDirr[0] * midLength;
	P[1] = newWPoint[1] + newDirr[1] * midLength;
	P[2] = newWPoint[2] + newDirr[2] * midLength;

	linAlg::calculateVec(P, spherePos, T);
	linAlg::normVec(T);
	linAlg::calculateVec(P, midPoint, Q);

	u = linAlg::dotProd(T, Q) / linAlg::vecLength(T);
	T[0] = T[0] * u;
	T[1] = T[1] * u;
	T[2] = T[2] * u;

	spherePos[0] += T[0];
	spherePos[1] += T[1];
	spherePos[2] += T[2];
	//T[0] = Q[0]; T[1] = Q[1]; T[2] = Q[2];
	//linAlg::normVec(T);
	//vNorm = T;

	//u = linAlg::dotProd(vNorm, Q);
	midPoint[0] = P[0];
	midPoint[1] = P[1];
	midPoint[2] = P[2];

	tempSize = selectedSize;
	selectedSize = 0;
	// 2.3 >-----------------------
	//sIt = sHead;
	//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
	for (int i = 0; i <= mesh->vertexCap; i++) {

		vPoint = mVertexArray[i].xyz;
		linAlg::calculateVec(vPoint, spherePos, tempVec1);

		mLength = linAlg::vecLength(tempVec1);

		if (mLength <  radius)
		{
			vNorm = mVertexArray[i].nxyz;
			selectedVertices[selectedSize] = i; selectedSize++;

			mVInfoArray[i].selected = 3.0f;
			linAlg::normVec(tempVec1);
			mVertexArray[i].xyz[0] = spherePos[0] + tempVec1[0] * radius;
			mVertexArray[i].xyz[1] = spherePos[1] + tempVec1[1] * radius;
			mVertexArray[i].xyz[2] = spherePos[2] + tempVec1[2] * radius;
			break;

		}
	}

	for (int j = 0; j < selectedSize; j++) {
		index2 = selectedVertices[j];
		tempEdge = mVInfoArray[index2].edgePtr;

		do {
			if (mVInfoArray[mEdgeArray[tempEdge].vertex].selected < 3.0f){
				index = mEdgeArray[tempEdge].vertex;

				vPoint2 = mVertexArray[index].xyz;
				linAlg::calculateVec(vPoint2, spherePos, tempVec1);

				mLength = linAlg::vecLength(tempVec1);

				if (mLength < radius) {
					selectedVertices[selectedSize] = index; selectedSize++;
					mVInfoArray[index].selected = 3.0f;

					vNorm = mVertexArray[index].nxyz;

					linAlg::normVec(tempVec1);
					mVertexArray[index].xyz[0] = spherePos[0] + tempVec1[0] * radius;
					mVertexArray[index].xyz[1] = spherePos[1] + tempVec1[1] * radius;
					mVertexArray[index].xyz[2] = spherePos[2] + tempVec1[2] * radius;
				}
			}
			tempEdge = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;

		} while (tempEdge != mVInfoArray[index2].edgePtr);

	}

	mesh->updateArea(selectedVertices, selectedSize);
	//mesh->updateArea(previouslySelectedVertices, previouslySelectedSize);
	//previouslySelectedSize = 0;
	mesh->updateOGLData();

}
void Drag::deSelect()
{
	for (int i = 0; i < previouslySelectedSize; i++)
	{
		mVInfoArray[previouslySelectedVertices[i]].selected = 0.0f;
	}
	previouslySelectedSize = 0;

	for (int i = 0; i < selectedSize; i++)
	{
		mVInfoArray[selectedVertices[i]].selected = 0.0f;
	}
	selectedSize = 0;
}