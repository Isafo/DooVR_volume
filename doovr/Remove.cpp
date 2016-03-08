/* Copyright (C) DooVR - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Olle Grahn <ollgr444@student.liu.se> and Isabelle Forsman <isafo268@student.liu.se>, July 2015 - Mars 2016
*/

#include "Remove.h"
#include "LineSphere.h"
#include "linAlg.h"

#include <vector>
#include <algorithm>


Remove::Remove()
{
	radius = 0.01f;
	toolBrush = new LineSphere(0.0f, 0.0f, 0.0f, 1.0f);
	octList.reserve(5000);
	octantStack.reserve(100);
}


Remove::~Remove()
{

}


void Remove::render(MatrixStack* MVstack, GLint locationMV)
{
	MVstack->push();
	MVstack->scale(radius);
	MVstack->translate(toolBrush->getPosition());
	glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack->getCurrentMatrix());
	toolBrush->render();
	MVstack->pop();
}

void Remove::changeScalarData(DynamicMesh* _mesh, Wand* _wand, Octree* _ot) {
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

	double wMaxRadius = radius + _ot->voxDiagonalLength;
	double wMinRadius = radius - _ot->voxDiagonalLength;
	float tmpVecLength;

	Octant* currentOct = _ot->root;
	Octant* childOct;
	Octant* tmpOct;
	Octant* oNeighbor[7];
	float nPos[7][3];
	float fDim;
	float hDim;
	float addDim;
	float cornerPos[3];

	octList.clear();
	octantStack.clear();
	octantStackElement tempStackElm;

	int emptyVStackInitSize = _mesh->emptyVStack.size();
	int emptyTStackInitSize = _mesh->emptyTStack.size();
	int tmpStackSize;

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
	if (d > wMaxRadius*wMaxRadius) {
		return;
	}
	else{
		tempStackElm.octant = currentOct;
		octantStack.push_back(tempStackElm);
	}


	while (!octantStack.empty()){

		if (octantStack.back().index >= 8){
			if (octantStack.back().deallocationBool == 2){
				tmpStackSize = octantStack.size();
				octantStack.back().octant->checkHomogeneity(octantStack);

				if (tmpStackSize == octantStack.size())
					octantStack.pop_back();
			}
			else {
				octantStack.pop_back();
			}

			continue;
		}


		childOct = octantStack.back().octant->child[octantStack.back().index];

		++octantStack.back().index;

		tmpPos[0] = nwPos[0] - childOct->pos[0];
		tmpPos[1] = nwPos[1] - childOct->pos[1];
		tmpPos[2] = nwPos[2] - childOct->pos[2];
		d = 0.0f;

		childDim = childOct->halfDim;

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
		//check if tool and octant collide
		if (d <= wMaxRadius*wMaxRadius) {
			//find corner furthest away from sphere center
			tmpVec[0] = (0.0f > tmpPos[0] ? childDim : -childDim);
			tmpVec[1] = (0.0f > tmpPos[1] ? childDim : -childDim);
			tmpVec[2] = (0.0f > tmpPos[2] ? childDim : -childDim);

			//<--- check if octant is entirely inside sphere --	
			linAlg::calculateVec(tmpVec, tmpPos, tmpVec);
			if (linAlg::vecLength(tmpVec) <= wMinRadius) {

				if (childOct->child[0] != nullptr) {
					if (octantStack.back().deallocationBool != 1)
						octantStack.back().deallocationBool = 2;

					childOct->deAllocate(_mesh);
				}

				childOct->scalarValue = 0;
				childOct->isoBool = false;

				// delete the old vertex data
				if (childOct->vertices[2] != -1){
					_mesh->emptyVStack.push_back(childOct->vertices[2]);
					childOct->vertices[2] = -1;
				}
				if (childOct->vertices[1] != -1){
					_mesh->emptyVStack.push_back(childOct->vertices[1]);
					childOct->vertices[1] = -1;
				}
				if (childOct->vertices[0] != -1){
					_mesh->emptyVStack.push_back(childOct->vertices[0]);
					childOct->vertices[0] = -1;
				}

				// delete the old triangle data
				if (childOct->tCount != 0){
					for (int i = 0; i < childOct->tCount; ++i){
						_mesh->emptyTStack.push_back(childOct->triangles[i]);
					}
					childOct->tCount = 0;
					delete[] childOct->triangles;
					childOct->triangles = nullptr;
				}

			}// --->
			//octant was not entirely inside sphere
			else {
				if (childOct->depth == Octant::MAX_DEPTH) {
					octList.push_back(childOct);
				}
				else {

					if (childOct->child[0] == nullptr){
						if (childOct->isoBool == true){
							childOct->partition();

							octantStack.back().deallocationBool = 1;

							tempStackElm.octant = childOct;
							octantStack.push_back(tempStackElm);
						}
					}
					else {
						octantStack.back().deallocationBool = 1;

						tempStackElm.octant = childOct;
						octantStack.push_back(tempStackElm);
					}
				}
			}
		}
	}

	if (octList.empty())
		return;

	// remake
	//<-- change selected iso values --

	int scalarNR = std::pow(2, 10 - Octant::MAX_DEPTH);
	int hRes = (scalarNR - 1) / 2;
	float dim = octList[0]->halfDim;

	for (auto& tempOct : octList) {
		tmpPos[0] = tempOct->pos[0] + tempOct->halfDim;
		tmpPos[1] = tempOct->pos[1] + tempOct->halfDim;
		tmpPos[2] = tempOct->pos[2] + tempOct->halfDim;
		linAlg::calculateVec(tmpPos, nwPos, tmpVec);

		tmpVecLength = linAlg::vecLength(tmpVec);

		if (tmpVecLength < wMaxRadius) {//check if point is inside sphere
			tempOct->scalarValue = std::min(static_cast<double>(tempOct->scalarValue), (255 * std::max(std::min(((tmpVecLength - radius) + _ot->voxDiagonalLength) / (2 * _ot->voxDiagonalLength), 1.0), 0.0)));
			tempOct->isoBool = (tempOct->scalarValue > 128 ? true : false);
		}
	}// -->

	_mesh->generateMC(&octList);
	//_mesh->updateOGLData(&octList);


	// update removed vertexbuffer data that was not reused
	/*if (_mesh->emptyVStack.size() > emptyVStackInitSize || _mesh->emptyTStack.size() > emptyTStackInitSize) {
		_mesh->updateRemovedOGLData(emptyVStackInitSize, emptyTStackInitSize);
	}*/
}

