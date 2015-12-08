#include "Add.h"
#include "LineSphere.h"
#include "linAlg.h"

#include <vector>



Add::Add()
{
	radius = 0.01f;
	toolBrush = new LineSphere(0.0f, 0.0f, 0.0f, 1.0f);
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

void Add::changeScalarData(DynamicMesh* _mesh, Wand* _wand, Octree* _ot )
{
	//TODO: started assuming octree depth of 5, changed it to 6 might have caused bugs. check. Also make the handling of depth dynamic
	//TODO: implement tests that check if child cubes can be deallocated when entire cubes are filled.
	//TODO:	use currentOct->fillCount variable in Octant properly
	float wPos[4]; float nwPos[4];
	float wDirr[4]; float nwDirr[4];
	float s, d = 0.0f, childDim = 0.0f;
	float octPos[3];
	float tmpPos[3];
	float tmpVec[3];

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

	std::vector<Octant*> octList;
	octList.reserve(1000);
	int olCounter = 0;
	int tmpI;
	int olStart;
	

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

	while (currentOct->depth < _ot->root->MAX_DEPTH) {//<-- reaching depth 6 --
			
		if (currentOct->child[0] == nullptr) {
			currentOct->partition();
		}

		childDim = currentOct->child[0]->halfDim;
		for (int i = 0; i < 8; i++) {//<--- collision check cubechildren ---
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

				linAlg::calculateVec(tmpVec, tmpPos, tmpVec);
				if (linAlg::vecLength(tmpVec) <= radius) {//<--- check if cube is entirely inside sphere --	
						
					if (childOct->child[0] != nullptr)
						childOct->deAllocate();


					childOct->data = 255;
					childOct->isoBool = true;
				}// --->
				else {
					octList.push_back(childOct);
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
	//_mesh->updateOGLData();
}

