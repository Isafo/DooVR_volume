#include "Draw.h"


#define EPSILON 0.000001

Draw::Draw(DynamicMesh* mesh, Wand* wand)
{
	selectedVertices = new int[MAX_SELECTED]; selectedSize = 0;
	previouslySelectedVertices = new int[MAX_SELECTED]; previouslySelectedSize = 0;

	radius = 0.01f;
	toolBrush = new Circle(0.0f, 0.0f, 0.0f, 1.0f);
	pointer = new Line(0.0f, 0.0f, 0.0f, 1.0f);
	iCircle = new Circle(0.0f, 0.0f, 0.0f, 1.0f);

	lineOffset[0] = 0.0f;
	lineOffset[1] = 0.0f;
	lineOffset[2] = 1.0f;

	mVertexArray = mesh->vertexArray;
	mVInfoArray = mesh->vInfoArray;
	mEdgeArray = mesh->e;
	mPosition = mesh->position;
	mOrientation = mesh->orientation;
	mMAX_LENGTH = mesh->MAX_LENGTH;

	zVec[0] = 0.0f;  zVec[1] = 0.0f; zVec[2] = 1.0f;
}


Draw::~Draw()
{
	delete selectedVertices;
	delete previouslySelectedVertices;
	delete toolBrush;
	delete pointer;
	delete iCircle;
}

void Draw::renderIntersection(MatrixStack* MVstack, GLint locationMV)
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

void Draw::render(MatrixStack* MVstack, GLint locationMV)
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


void Draw::firstSelect(DynamicMesh* mesh, Wand* wand)
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
			selectedVertices[selectedSize] = i; selectedSize++;
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
		for (int i = 0; i < selectedSize; i++) {
			index2 = selectedVertices[i];
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
			for (int i = 0; i < selectedSize; i++)
			{
				mVInfoArray[selectedVertices[i]].selected = 0.0f;
			}
			selectedVertices[0] = mIndex;
			selectedSize = 1;
			mVInfoArray[mIndex].selected = 4.0f;

			intersection.nxyz[0] = mVertexArray[mIndex].nxyz[0];
			intersection.nxyz[1] = mVertexArray[mIndex].nxyz[1];
			intersection.nxyz[2] = mVertexArray[mIndex].nxyz[2];

			vPoint = intersection.xyz;
			vNorm = intersection.nxyz;

			// --< 2.something
			//sIt = sHead->next;
			//vInfoArray[sIt->index].selected = 4.0f;
			// 2.3 >-----------------------
			//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
			for (int i = 0; i < selectedSize; i++) {
				index2 = selectedVertices[i];
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

						if (pLength < radius / 2.0f && pLength > -radius / 2.0f && oLength < radius + 0.002) {
							//tempSVert = new sVert; sTail->next->next = tempSVert; sTail->next = tempSVert; tempSVert->next = sTail;
							//tempSVert->index = index;
							selectedVertices[selectedSize] = index; selectedSize++;

							mVInfoArray[index].selected = 4.0f;
						}
					}
					tempEdge = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;

				} while (tempEdge != mVInfoArray[index2].edgePtr);

				//	sIt = sIt->next;
			}
			for (int i = 0; i < selectedSize; i++)
			{
				mVInfoArray[selectedVertices[i]].selected = 0.0f;
			}
		}
	}
}


void Draw::moveVertices(DynamicMesh* mesh, Wand* wand)
{
	float wPoint[4]; float newWPoint[4]; float Dirr[4]; float newDirr[4];
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

	int mIndex; float mLength;

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


	/*if (previouslySelectedSize == 0 && selectedSize == 0)
	{
		firstSelect(mesh, wand);
		usedList = previouslySelectedVertices; usedSize = previouslySelectedSize;
		emptyList = selectedVertices; emptySize = selectedSize;
	}
	else if (selectedSize == 0)
	{
		usedList = previouslySelectedVertices; usedSize = previouslySelectedSize;
		emptyList = selectedVertices; emptySize = selectedSize;
	}
	else
	{
		usedList = selectedVertices; usedSize = selectedSize;
		emptyList = previouslySelectedVertices; emptySize = previouslySelectedSize;
	}*/


	//sIt = sHead;
	// 1.0 >--------------------------
	////--< 2.0 | start searching through vertexarray for points that are within the brush
	//for (int i = 0; i < usedSize; i++) {
	//	//--< 2.1 | calculate vector between vertexposition and wandposition

	//	linAlg::calculateVec(mVertexArray[usedList[i]].xyz, newWPoint, eVec1);

	//	// 2.1 >---------------------
	//	//--< 2.2 | calculate orthogonal and parallel distance between this vector and wandDirection
	//	pLength = linAlg::dotProd(newDirr, eVec1);;//                                                 / linAlg::vecLength(newDirr);
	//	Dirr[0] = newDirr[0] * pLength;
	//	Dirr[1] = newDirr[1] * pLength;
	//	Dirr[2] = newDirr[2] * pLength;

	//	linAlg::calculateVec(eVec1, Dirr, eVec2);
	//	oLength = linAlg::vecLength(eVec2);
	//	// 2.2 >----------------------
	//	if (pLength > 0.0f && oLength < mMAX_LENGTH)
	//	{
	//		emptyList[emptySize] = usedList[i]; emptySize++;
	//		success = true;
	//	}
	//	else
	//	{
	//		mVInfoArray[usedList[i]].selected = (vInfoArray[HVerts[i].index].selected - floor(vInfoArray[HVerts[i].index].selected));
	//	}
	//}
	//sTail->next = sIt;
	// 2.0 >----------------------



	for (int i = 0; i < selectedSize; i++) {
		index2 = selectedVertices[i];

		if (mVInfoArray[index2].edgePtr > 0)
			tempEdge = mVInfoArray[index2].edgePtr;
		else
			continue;

		do {
			tempE = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;
			if (mVInfoArray[mEdgeArray[tempEdge].vertex].selected < 2.0f)
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
								lengthVec[0] = newDirr[0] * t;
								lengthVec[1] = newDirr[1] * t;
								lengthVec[2] = newDirr[2] * t;
								tempE = mVInfoArray[index2].edgePtr;
								success = true;
								if (linAlg::vecLength(lengthVec) < linAlg::vecLength(intersection.nxyz)){
									mIndex = index2;
									intersection.xyz[0] = newWPoint[0] + lengthVec[0];
									intersection.xyz[1] = newWPoint[1] + lengthVec[1];
									intersection.xyz[2] = newWPoint[2] + lengthVec[2];
									intersection.nxyz[0] = lengthVec[0];
									intersection.nxyz[1] = lengthVec[1];
									intersection.nxyz[2] = lengthVec[2];
									//linAlg::crossProd(vNorm, eVec2, eVec1);
									//linAlg::normVec(vNorm);
								}
							}
						}
					}
				}
			}

			tempEdge = tempE;
		} while (tempEdge != mVInfoArray[index2].edgePtr);

		mVInfoArray[index2].selected += 1.0f;

	}
	//sTail->next = sIt;

	//for (int i = 0; i < emptySize; i++)
	//{
	//	mVInfoArray[emptyList[i]].selected = 0.0f;
	//}
	//emptySize = 0;

	//CNR = 0;
	if (success)
	{
		vPoint = intersection.xyz;
		vNorm = mVertexArray[mIndex].nxyz;
		//vNorm = newDirr;
		intersection.nxyz[0] = vNorm[0];
		intersection.nxyz[1] = vNorm[1];
		intersection.nxyz[2] = vNorm[2];
		//emptyList[0] = mIndex;
		//emptySize = 1;

		// --< 2.something
		tempSize = selectedSize;
		selectedSize = 0;
		// 2.3 >-----------------------
		//sIt = sHead;
		//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
		for (int i = 0; i < tempSize; i++) {
			index2 = selectedVertices[i];

			linAlg::calculateVec(mVertexArray[index2].xyz, vPoint, eVec1);

			pLength = linAlg::dotProd(vNorm, eVec1);// / linAlg::vecLength(newDirr);
			Dirr[0] = vNorm[0] * pLength;
			Dirr[1] = vNorm[1] * pLength;
			Dirr[2] = vNorm[2] * pLength;

			linAlg::calculateVec(eVec1, Dirr, eVec2);
			oLength = linAlg::vecLength(eVec2);

			if (pLength < radius / 2.0f && pLength > -radius / 2.0f && oLength < radius + 0.01) {
				selectedVertices[selectedSize] = index2; selectedSize++;
				if (oLength < radius)
				{
					vNorm2 = mVertexArray[index2].nxyz;
					//u = ((pow(radius, 2) - pow(oLength, 2)) / pow(radius, 2));
					u = 1.0f;
					t = 0.003f*u;
					v = mVInfoArray[index2].selected - floor(mVInfoArray[index2].selected);
					if (v > 0.0f)
						v = v;
					if (t > v )
					{
						u = t - v;
						vPoint1 = mVertexArray[index2].xyz;
						vPoint1[0] += vNorm2[0] * u;
						vPoint1[1] += vNorm2[1] * u;
						vPoint1[2] += vNorm2[2] * u;
						mVInfoArray[index2].selected = t;
					}
				}
				mVInfoArray[index2].selected += 2.0f;
				
			}
			else{
				mVInfoArray[index2].selected = 0.0f;
				previouslySelectedVertices[previouslySelectedSize] = index2; previouslySelectedSize++;
			}
		}


		for (int i = 0; i < selectedSize; i++)
		{
			index2 = selectedVertices[i];
			//vPoint = mVertexArray[index2].xyz;

			tempEdge = mVInfoArray[index2].edgePtr;
			do {
				index = mEdgeArray[tempEdge].vertex;

				if (mVInfoArray[index].selected == 0.0f){

					linAlg::calculateVec(mVertexArray[index].xyz, vPoint, eVec1);

					pLength = linAlg::dotProd(vNorm, eVec1);// / linAlg::vecLength(newDirr);
					Dirr[0] = vNorm[0] * pLength;
					Dirr[1] = vNorm[1] * pLength;
					Dirr[2] = vNorm[2] * pLength;

					linAlg::calculateVec(eVec1, Dirr, eVec2);
					oLength = linAlg::vecLength(eVec2);

					if (pLength < radius / 2.0f && pLength > -radius / 2.0f && oLength < radius) {

						vNorm2 = mVertexArray[index].nxyz;
						//u = ((pow(radius, 2) - pow(oLength, 2)) / pow(radius, 2));
						u = 1.0f;
						t = 0.003f*u;

						//u = t - v;

						vPoint1 = mVertexArray[index].xyz;
						vPoint1[0] += vNorm2[0] * t;
						vPoint1[1] += vNorm2[1] * t;
						vPoint1[2] += vNorm2[2] * t;

						selectedVertices[selectedSize] = index; selectedSize++;
						mVInfoArray[index].selected = 2 + t;
					}
				}

				tempEdge = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;
			} while (tempEdge != mVInfoArray[index2].edgePtr);
		}

		mesh->updateNormals(selectedVertices, selectedSize);
		mesh->updateArea(previouslySelectedVertices, previouslySelectedSize);
		previouslySelectedSize = 0;
	}
	

}
void Draw::deSelect()
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