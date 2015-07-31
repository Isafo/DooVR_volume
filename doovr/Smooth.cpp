#include "Smooth.h"
#include "Circle.h"

#define EPSILON 0.000001

Smooth::Smooth(DynamicMesh* mesh, Wand* wand)
{
	currentlySelectedVertices = new int[MAX_SELECTED]; currentlySelectedSize = 0;
	previouslySelectedVertices = new int[MAX_SELECTED]; previouslySelectedSize = 0;

	radius = 0.01f;
	toolBrush = new Circle(0.0f, 0.0f, 0.0f, 1.0f);
	pointer = new Line(0.0f, 0.0f, 0.0f, 1.0f);
	lineOffset[0] = 0.0f;
	lineOffset[1] = 0.0f;
	lineOffset[2] = 1.0f;

	mVertexArray = mesh->vertexArray;
	mVInfoArray = mesh->vInfoArray;
	mEdgeArray = mesh->e;
	mPosition = mesh->position;
	mOrientation = mesh->orientation;
	mMAX_LENGTH = mesh->MAX_LENGTH;
}


Smooth::~Smooth()
{
	delete currentlySelectedVertices;
	delete previouslySelectedVertices;
	delete toolBrush;
	delete pointer;
}


void Smooth::render(MatrixStack* MVstack, GLint locationMV)
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


void Smooth::firstSelect(DynamicMesh* mesh, Wand* wand)
{
	float wPoint[4]; float newWPoint[4]; float Dirr[4]; float newDirr[4];
	float eVec1[3]; float eVec2[3];
	float P[3]; float Q[3]; float T[3];
	float* vPoint; float* vPoint2; float* vNorm; float* vNorm2;
	int index; int index2;
	float* ptrVec;

	int tempEdge; int tempE;

	bool success = false; bool triB = false;

	float pLength = 0.0f; float invP = 0.0f; float u = 0.0f; float v = 0.0f; float t = 0.0f;
	float oLength = 0.0f;

	int mIndex; float mLength;


	//sIt = sHead->next;
	//while (sIt != sTail)
	//{
	//	vInfoArray[sIt->index].selected = 0;
	//	tempSVert = sIt->next;
	//	delete sIt;
	//	sIt = tempSVert;
	//}
	//sTail->next = sHead;
	//sHead->next = sTail;
	/*for (int i = 0; i < HNR; i++)
	{
		vInfoArray[HVerts[i].index].selected = 0.0f;
	}
	HNR = 0;*/
	intersection.nxyz[0] = 100.f; intersection.nxyz[1] = 100.f; intersection.nxyz[2] = 100.f;
	//sIt = sHead->next;

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
	//newDirr[0] = Dirr[0];
	//newDirr[1] = Dirr[1];
	//newDirr[2] = Dirr[2];
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
			//--< 2.3 | add the found vertex to list of selected vertices and mark it as selected 
			//tempSVert = new sVert; sTail->next->next = tempSVert; sTail->next = tempSVert; tempSVert->next = sTail;
			//tempSVert->index = i;
			previouslySelectedVertices[previouslySelectedSize] = i; previouslySelectedSize++;
			/*if (pLength < sMid->vec[1]){
			tempSVert->vec[0] = oLength;
			tempSVert->vec[1] = pLength;
			sMid = tempSVert;
			}*/
			success = true;
		}
	}
	// 2.0 >----------------------

	if (success) {
		success = false;

		//sIt = sHead;
		for (int i = 0; i < previouslySelectedSize; i++) {
			index2 = previouslySelectedVertices[i];
			tempEdge = mVInfoArray[index2].edgePtr;

			do {
				tempE = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;

				if (mVInfoArray[mEdgeArray[tempEdge].vertex].selected < 3.0f)
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
								if (t > EPSILON)
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
									}
								}
							}
						}
					}
				}
				tempEdge = tempE;

			} while (tempEdge != mVInfoArray[index2].edgePtr);

			mVInfoArray[index2].selected = 4.0f;
			//sIt = sIt->next;
		}
		//sTail->next = sIt;

		if (success)
		{
			//sIt = sHead->next;
			//while (sIt != sTail)
			//{
			//	vInfoArray[sIt->index].selected = 0.0f;
			//	tempSVert = sIt->next;
			//	delete sIt;
			//	sIt = tempSVert;
			//}
			//sTail->next = sHead;
			//sHead->next = sTail;
			//
			//tempSVert = new sVert; sTail->next->next = tempSVert; sTail->next = tempSVert; tempSVert->next = sTail;
			//tempSVert->index = sMid.index;
			for (int i = 0; i < previouslySelectedSize; i++)
			{
				mVInfoArray[previouslySelectedVertices[i]].selected = 0.0f;
			}
			previouslySelectedVertices[0] = mIndex;
			previouslySelectedSize = 1;
			mVInfoArray[mIndex].selected = 4.0f;

			vPoint = intersection.xyz;
			vNorm = intersection.nxyz;

			// --< 2.something
			//sIt = sHead->next;
			//vInfoArray[sIt->index].selected = 4.0f;

			// 2.3 >-----------------------
			//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
			for (int i = 0; i < previouslySelectedSize; i++) {
				index2 = previouslySelectedVertices[i];
				tempEdge = mVInfoArray[index2].edgePtr;

				do {
					if (mVInfoArray[mEdgeArray[tempEdge].vertex].selected < 4.0f){
						index = mEdgeArray[tempEdge].vertex;

						linAlg::calculateVec(mVertexArray[index].xyz, vPoint, eVec1);

						pLength = linAlg::dotProd(vNorm, eVec1);// / linAlg::vecLength(newDirr);
						Dirr[0] = vNorm[0] * pLength;
						Dirr[1] = vNorm[1] * pLength;
						Dirr[2] = vNorm[2] * pLength;

						linAlg::calculateVec(eVec1, Dirr, eVec2);
						oLength = linAlg::vecLength(eVec2);

						if (pLength < radius / 2.0f && pLength > -radius / 2.0f && oLength < radius) {
							//tempSVert = new sVert; sTail->next->next = tempSVert; sTail->next = tempSVert; tempSVert->next = sTail;
							//tempSVert->index = index;
							previouslySelectedVertices[previouslySelectedSize] = index; previouslySelectedSize++;
							

							mVInfoArray[index].selected = 4.0f;
						}
					}
					tempEdge = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;

				} while (tempEdge != mVInfoArray[index2].edgePtr);

				//	sIt = sIt->next;
			}
		}
	}
}
void Smooth::moveVertices(DynamicMesh* mesh, Wand* wand)
{

}
void Smooth::deSelect()
{
	for (int i = 0; i < previouslySelectedSize; i++)
	{
		mVInfoArray[previouslySelectedVertices[i]].selected = 0.0f;
	}
	previouslySelectedSize = 0;
}