#include "Add.h"
#include "LineSphere.h"
#include "linAlg.h"

#include <vector>



Add::Add()
{
	radius = 0.01f;
	toolBrush = new LineSphere(0.0f, 0.0f, 0.0f, 1.0f);
	octList.reserve(5000);
}


Add::~Add()
{
}

void Add::render(MatrixStack* MVstack, GLint locationMV)
{
	MVstack->push();
		MVstack->scale(radius);
		MVstack->translate(toolBrush->getPosition());
		glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack->getCurrentMatrix());
		toolBrush->render();
	MVstack->pop();
}

void Add::changeScalarData(DynamicMesh* _mesh, Wand* _wand, Octree* _ot ) {
	//TODO: implement tests that check if child cubes can be deallocated when entire cubes are filled.
	float wPos[4]; float nwPos[4];
	float wDirr[4]; float nwDirr[4];
	float s, d = 0.0f, childDim = 0.0f;
	float octPos[3];
	float tmpPos[3];
	float tmpVec[3];
	bool maxReached = true;

	linAlg::transpose(_mesh->orientation);
	//--< 1.0 | calculated the position and direction of the wand
	_wand->getPosition(wPos);
	wPos[0] = wPos[0] - _mesh->position[0];
	wPos[1] = wPos[1] - _mesh->position[1];
	wPos[2] = wPos[2] - _mesh->position[2];
	wPos[3] = 1.0f;
	linAlg::vectorMatrixMult(_mesh->orientation, wPos, nwPos);
	
	//nwPos[0] = (nwPos[0] + 1.0f) * 500; nwPos[1] = (nwPos[1] + 1.0f) * 500; nwPos[2] = (nwPos[2] + 1.0f) * 500;

	_wand->getDirection(wDirr);
	linAlg::normVec(wDirr);
	wDirr[3] = 1.0f;
	linAlg::vectorMatrixMult(_mesh->orientation, wDirr, nwDirr);
	linAlg::transpose(_mesh->orientation);

	Octant* currentOct = _ot->root;
	Octant* childOct;
	Octant* tmpOct;
	Octant* oNeighbor[7];
	float nPos[7][3];
	float fDim;
	float hDim;
	float addDim;
	float cornerPos[3];
	//TODO: make this variable depend on MAX_DEPTH
	const float minDim = 0.001953125f;

	int olCounter = 0;
	int tmpI;
	int olStart;
	int fillCheck;
	

	for (int j = 0; j < 3; j++) {
		if (nwPos[j] <  -currentOct->halfDim) {
			s = nwPos[j] + currentOct->halfDim;
			d += s*s;
		}
		else if (nwPos[j] > currentOct->halfDim) {
			s = nwPos[j] - currentOct->halfDim;
			d += s*s;
		}
	}
	if (d > radius*radius) {
		return;
	}


	int emptyVStackInitSize = _mesh->emptyVStack.size();
	int emptyTStackInitSize = _mesh->emptyTStack.size();

	while (currentOct->depth < _ot->root->MAX_DEPTH) {//<-- reaching depth 6 --
			


		//check if children are allocated
		if (currentOct->child[0] == nullptr) {
			currentOct->partition();
		}
		//<--- collision check cubechildren ---
		childDim = currentOct->child[0]->halfDim;
		for (int i = 0; i < 8; i++) {
			childOct = currentOct->child[i];
			tmpPos[0] = nwPos[0] - childOct->pos[0];
			tmpPos[1] = nwPos[1] - childOct->pos[1];
			tmpPos[2] = nwPos[2] - childOct->pos[2];
			d = 0.0f;

			for (int j = 0; j < 3; j++) {
				if (tmpPos[j] <  -childDim) {
					s = tmpPos[j] + childDim;
					d += s*s;
				}
				else if (tmpPos[j] > childDim) {
					s = tmpPos[j] - childDim;
					d += s*s;
				}
			}
			if (d <= radius*radius) {
				//find corner furthest away from sphere center
				tmpVec[0] = (0.0f > tmpPos[0] ? childDim : -childDim);
				tmpVec[1] = (0.0f > tmpPos[1] ? childDim : -childDim);
				tmpVec[2] = (0.0f > tmpPos[2] ? childDim : -childDim);

				//<--- check if cube is entirely inside sphere --	
				linAlg::calculateVec(tmpVec, tmpPos, tmpVec);
				if (linAlg::vecLength(tmpVec) <= radius) {
						
					if (childOct->child[0] != nullptr) {
						childOct->deAllocate(_mesh);
					}
						
					childOct->data = 255;
					childOct->isoBool = true;
				}// --->
				else {
					//Check if cell is already filled \________________________________________________________________________
					hDim = childOct->halfDim;
					fDim = hDim*2;
					addDim = hDim - minDim;
					cornerPos[0] = childOct->pos[0] + addDim; 
					cornerPos[1] = childOct->pos[1] + addDim; 
					cornerPos[2] = childOct->pos[2] + addDim;
					//find neighbours -------------------------------------
					//childOct->pos
					nPos[0][0] = cornerPos[0] - fDim;	nPos[0][1] = cornerPos[1] - fDim;	nPos[0][2] = cornerPos[2] - fDim;
					nPos[1][0] = cornerPos[0];			nPos[1][1] = cornerPos[1] - fDim;	nPos[1][2] = cornerPos[2] - fDim;
					nPos[2][0] = cornerPos[0];			nPos[2][1] = cornerPos[1] - fDim;	nPos[2][2] = cornerPos[2];
					nPos[3][0] = cornerPos[0] - fDim;	nPos[3][1] = cornerPos[1] - fDim;	nPos[3][2] = cornerPos[2];
					nPos[4][0] = cornerPos[0] - fDim;	nPos[4][1] = cornerPos[1];			nPos[4][2] = cornerPos[2] - fDim;
					nPos[5][0] = cornerPos[0];			nPos[5][1] = cornerPos[1];			nPos[5][2] = cornerPos[2] - fDim;
					nPos[6][0] = cornerPos[0] - fDim;	nPos[6][1] = cornerPos[1];			nPos[6][2] = cornerPos[2];

					for (int j = 0; j < 7; j++){
						//TODO: replace temp values of thjs jf
						if ((nPos[j][0] > 0.5f || nPos[j][0] < -0.5f ||
							nPos[j][1] > 0.5f || nPos[j][1] < -0.5f ||
							nPos[j][2] > 0.5f || nPos[j][2] < -0.5f)){
							std::cout << "utanfor";
						}

						tmpOct = childOct->parent;
						while (nPos[j][0] > tmpOct->pos[0] + tmpOct->halfDim || nPos[j][0] < tmpOct->pos[0] - tmpOct->halfDim ||
							nPos[j][1] > tmpOct->pos[1] + tmpOct->halfDim || nPos[j][1] < tmpOct->pos[1] - tmpOct->halfDim ||
							nPos[j][2] > tmpOct->pos[2] + tmpOct->halfDim || nPos[j][2] < tmpOct->pos[2] - tmpOct->halfDim){

							tmpOct = tmpOct->parent;
						}
						while (tmpOct->child[0] != nullptr){
							for (int k = 0; k < 8; k++){
								if (nPos[j][0] < tmpOct->child[k]->pos[0] + tmpOct->child[k]->halfDim && nPos[j][0] > tmpOct->child[k]->pos[0] - tmpOct->child[k]->halfDim &&
									nPos[j][1] < tmpOct->child[k]->pos[1] + tmpOct->child[k]->halfDim && nPos[j][1] > tmpOct->child[k]->pos[1] - tmpOct->child[k]->halfDim &&
									nPos[j][2] < tmpOct->child[k]->pos[2] + tmpOct->child[k]->halfDim && nPos[j][2] > tmpOct->child[k]->pos[2] - tmpOct->child[k]->halfDim){
									tmpOct = tmpOct->child[k];
									break;
								}
							}
						}
						oNeighbor[j] = tmpOct;
					}
					//evaluate cell bools ---------------------------------
					//TODO: måste göra check på ifall childOcts skalär är inom penseln
					//och måste hålla skalärerna uppdaterade på all djup ifall detta ska
					//fungera
					fillCheck = childOct->isoBool;
					for (int j = 0; j < 7; j++)
						fillCheck += oNeighbor[j]->isoBool;
					
					if (fillCheck == 8){
						if (childOct->child[0] != nullptr)
							childOct->deAllocate(_mesh);
					}
					else{
						octList.push_back(childOct);
					}
						
				}
			}
		}// --->
		currentOct = octList[olCounter];
		olCounter++;
	}// -->

	
	//<-- change selected iso values --
	olCounter--;
	olStart = olCounter;
	tmpI = octList.size();

	int scalarNR = std::pow(2, 10 - _ot->root->MAX_DEPTH);
	int hRes = (scalarNR - 1) / 2;
	float dim = octList[olStart]->halfDim;
	

	while (olCounter < tmpI) {
		currentOct = octList[olCounter];

		tmpPos[0] = currentOct->pos[0] + currentOct->halfDim;
		tmpPos[1] = currentOct->pos[1] + currentOct->halfDim;
		tmpPos[2] = currentOct->pos[2] + currentOct->halfDim;
		linAlg::calculateVec(tmpPos, nwPos, tmpVec);

		if (linAlg::vecLength(tmpVec) <= radius) {//check if point is inside sphere
			currentOct->data = 255;
			currentOct->isoBool = true;
		}
		olCounter++;
	}// -->

   	olCounter = olStart;

	_mesh->generateMC(&octList, olStart);
	_mesh->updateOGLData(&octList, olStart);
	

	// update removed vertexbuffer data that was not reused
	if (_mesh->emptyVStack.size() > emptyVStackInitSize || _mesh->emptyTStack.size() > emptyTStackInitSize) {
	//	_mesh->updateRemovedOGLData(emptyVStackInitSize, emptyTStackInitSize);
	}
}

