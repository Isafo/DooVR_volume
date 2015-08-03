#include "Push.h"
#include "LineSphere.h"


Push::Push(DynamicMesh* mesh, Wand* wand)
{
	selectedVertices = new int[MAX_SELECTED]; selectedSize = 0;


	radius = 0.01f;
	toolBrush = new LineSphere(0.0f, 0.0f, 0.0f, 1.0f);

	mVertexArray = mesh->vertexArray;
	mVInfoArray = mesh->vInfoArray;
	mEdgeArray = mesh->e;
	mPosition = mesh->position;
	mOrientation = mesh->orientation;
	mMAX_LENGTH = mesh->MAX_LENGTH;

	zVec[0] = 0.0f;  zVec[1] = 0.0f; zVec[2] = 1.0f;

	selectedVertices[0] = 101;
}


Push::~Push()
{
	delete selectedVertices;
	delete toolBrush;
}


void Push::render(MatrixStack* MVstack, GLint locationMV){

	MVstack->push();
		MVstack->scale(radius);
		MVstack->translate(toolBrush->getPosition());
		glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack->getCurrentMatrix());
		toolBrush->render();
	MVstack->pop();
}

void Push::renderIntersection(MatrixStack* MVstack, GLint locationMV) {

}

void Push::firstSelect(DynamicMesh* mesh, Wand* wand)
{
	float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	float tempVec1[3]; float tempVec2[3];
	float wPoint[4]; float* vPoint; float* vPoint2;
	int index; int index2;

	int tempEdge;

	bool success = false;

	float pLength = 0.0f;
	float oLength = 0.0f;

	float mLength;

	for (int i = 0; i < selectedSize; i++)
	{
		mVInfoArray[selectedVertices[i]].selected = 0.0f;
	}
	selectedSize = 0;
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
		vPoint = mVertexArray[i].xyz;
		linAlg::calculateVec(vPoint, newWPoint, tempVec1);
		//tempVec1[0] = vPoint[0] - newWPoint[0];
		//tempVec1[1] = vPoint[1] - newWPoint[1];
		//tempVec1[2] = vPoint[2] - newWPoint[2];
		// 2.1 >---------------------
		mLength = linAlg::vecLength(tempVec1);
		// 2.2 >----------------------
		if (mLength <  radius)
		{
			//--< 2.3 | add the found vertex to list of selected vertices and mark it as selected 
			//changedVertices.push_back(i);
			selectedVertices[selectedSize] = i; selectedSize++;
			//mIndex = sVertsNR;
			//mLength = oLength;

			mVInfoArray[i].selected = 4.0f;

			// 2.3 >-----------------------
			//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
			for (int j = 0; j < selectedSize; j++) {
				index2 = selectedVertices[j];
				tempEdge = mVInfoArray[index2].edgePtr;

				do {
					if (mVInfoArray[mEdgeArray[tempEdge].vertex].selected == 0.0f){
						index = mEdgeArray[tempEdge].vertex;
						//changedEdges.push_back(tempEdge);
						//sEdges[nrofsEdges] = tempEdge;
						//nrofsEdges++;
						vPoint2 = mVertexArray[index].xyz;

						tempVec1[0] = vPoint2[0] - newWPoint[0];
						tempVec1[1] = vPoint2[1] - newWPoint[1];
						tempVec1[2] = vPoint2[2] - newWPoint[2];

						mLength = linAlg::vecLength(tempVec1);

						if (mLength < radius) {
							selectedVertices[selectedSize] = index; selectedSize++;
							mVInfoArray[index].selected = 4.0f;
						}
					}
					tempEdge = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;

				} while (tempEdge != mVInfoArray[index2].edgePtr);

				
			}
			// 2.4 >---------------------
			success = true;
			for (int i = 0; i < selectedSize; i++)
			{
				mVInfoArray[selectedVertices[i]].selected = 0.0f;
			}
			break;
		}
	}
}
void Push::moveVertices(DynamicMesh* mesh, Wand* wand){
	float newWPoint[4];
	float Dirr[4]; float newDirr[4];
	float tempVec1[3]; float tempVec2[3];
	float wPoint[4]; float* vPoint; float* vPoint2;
	int index; int index2;

	int tempEdge;

	bool success = false;

	float pLength = 0.0f;
	float oLength = 0.0f;

	float mLength;

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
	for (int i = 0; i <= selectedSize; i++) {
		//--< 2.1 | calculate vector between vertexposition and wandposition
		vPoint = mVertexArray[selectedVertices[i]].xyz;
		linAlg::calculateVec(vPoint, newWPoint, tempVec1);
		//tempVec1[0] = vPoint[0] - newWPoint[0];
		//tempVec1[1] = vPoint[1] - newWPoint[1];
		//tempVec1[2] = vPoint[2] - newWPoint[2];
		// 2.1 >---------------------
		mLength = linAlg::vecLength(tempVec1);
		// 2.2 >----------------------
		if (mLength <  radius)
		{
			//--< 2.3 | add the found vertex to list of selected vertices and mark it as selected 
			//changedVertices.push_back(i);
			//mIndex = sVertsNR;
			//mLength = oLength;

			mVInfoArray[i].selected = 4.0f;
			selectedVertices[0] = selectedVertices[i];
			selectedSize = 1;
			success = true;
			break;
		}
	}
	if (success == false)
		return;

	// 2.3 >-----------------------
	//--< 2.4 | a first vertex has been found, the rest of the search is done through the surface 
	for (int j = 0; j < selectedSize; j++) {
		index2 = selectedVertices[j];
		tempEdge = mVInfoArray[index2].edgePtr;

		do {
			if (mVInfoArray[mEdgeArray[tempEdge].vertex].selected == 0.0f){
				index = mEdgeArray[tempEdge].vertex;
				//changedEdges.push_back(tempEdge);
				//sEdges[nrofsEdges] = tempEdge;
				//nrofsEdges++;
				vPoint2 = mVertexArray[index].xyz;

				tempVec1[0] = vPoint2[0] - newWPoint[0];
				tempVec1[1] = vPoint2[1] - newWPoint[1];
				tempVec1[2] = vPoint2[2] - newWPoint[2];

				mLength = linAlg::vecLength(tempVec1);

				if (mLength < radius) {
					selectedVertices[selectedSize] = index; selectedSize++;
					mVInfoArray[index].selected = 4.0f;

					linAlg::normVec(tempVec1);
					mVertexArray[index].xyz[0] = newWPoint[0] + tempVec1[0] * radius;
					mVertexArray[index].xyz[1] = newWPoint[1] + tempVec1[1] * radius;
					mVertexArray[index].xyz[2] = newWPoint[2] + tempVec1[2] * radius;
				}
			}
			tempEdge = mEdgeArray[mEdgeArray[tempEdge].nextEdge].sibling;

		} while (tempEdge != mVInfoArray[index2].edgePtr);


	}		

}
void Push::deSelect(){
	
	for (int i = 0; i < selectedSize; i++)
	{
		mVInfoArray[selectedVertices[i]].selected = 0.0f;
	}
	selectedSize = 0;
}