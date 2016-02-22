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

	octList.clear();
	std::vector<std::pair<Octant*, unsigned char>> octantStack;

	int emptyVStackInitSize = _mesh->emptyVStack.size();
	int emptyTStackInitSize = _mesh->emptyTStack.size();

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
	else{
		octantStack.push_back(std::make_pair(currentOct, 0));
	}


	while (true){
		if (octantStack.back().first->child[0] != nullptr)
		currentOct = octantStack.back().first->child[octantStack.back().second];
	}

	// remake
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

