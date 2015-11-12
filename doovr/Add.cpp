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

	float wPos[4]; float nwPos[4];
	float wDirr[4]; float nwDirr[4];
	float s, d = 0.0;
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
	octList.reserve(50);
	int olCounter = 0;
	int tmpI;
	int olStart;

	//<- check if we are inside root of octree
	for (int i = 0; i < 3; i++){
		if (nwPos[i] < -currentOct->halfDim){
			s = nwPos[i] + currentOct->halfDim;
			d += s*s;
		}
		else if (nwPos[i] > currentOct->halfDim){
			s = nwPos[i] - currentOct->halfDim;
			d += s*s;
		}
	}//->
	if (d <= radius*radius){//<- collision check rootcube -
		while (currentOct->depth < 6){//<-- reaching depth 5 --
			
			if (currentOct->child[0] == nullptr){

				d = currentOct->halfDim / 2.0f;
				octPos[0] = currentOct->pos[0]; octPos[2] = currentOct->pos[2]; octPos[2] = currentOct->pos[2];
				tmpI = currentOct->depth + 1;

				currentOct->child[0] = new Octant(tmpI, octPos[0] - d, octPos[0] - d, octPos[0] + d, d);
				currentOct->child[1] = new Octant(tmpI, octPos[0] + d, octPos[0] - d, octPos[0] + d, d);
				currentOct->child[2] = new Octant(tmpI, octPos[0] - d, octPos[0] - d, octPos[0] - d, d);
				currentOct->child[3] = new Octant(tmpI, octPos[0] + d, octPos[0] - d, octPos[0] - d, d);
				currentOct->child[4] = new Octant(tmpI, octPos[0] - d, octPos[0] + d, octPos[0] + d, d);
				currentOct->child[5] = new Octant(tmpI, octPos[0] + d, octPos[0] + d, octPos[0] + d, d);
				currentOct->child[6] = new Octant(tmpI, octPos[0] - d, octPos[0] + d, octPos[0] - d, d);
				currentOct->child[7] = new Octant(tmpI, octPos[0] + d, octPos[0] + d, octPos[0] - d, d);

			}

			for (int i = 0; i < 8; i++){//<--- collision check cubechildren ---
				for (int i = 0; i < 3; i++){
					if (nwPos[i] < -currentOct->halfDim){
						s = nwPos[i] + currentOct->halfDim;
						d += s*s;
					}
					else if (nwPos[i] > currentOct->halfDim){
						s = nwPos[i] - currentOct->halfDim;
						d += s*s;
					}
				}
				if (d <= radius*radius){
					octList.push_back(currentOct->child[i]);
				}

			}// --->
			currentOct = octList[olCounter];
			olCounter++;
		}// -->

		//<-- change selected iso values --
		olCounter--;
		olStart = olCounter;
		tmpI = octList.size();
		while (olCounter < tmpI){
			currentOct = octList[olCounter];

			tmpPos[0] = currentOct->pos[0] + (currentOct->pos[0] > nwPos[0] ? currentOct->halfDim : -currentOct->halfDim);
			tmpPos[1] = currentOct->pos[1] + (currentOct->pos[1] > nwPos[1] ? currentOct->halfDim : -currentOct->halfDim);
			tmpPos[2] = currentOct->pos[2] + (currentOct->pos[2] > nwPos[2] ? currentOct->halfDim : -currentOct->halfDim);

			linAlg::calculateVec(tmpPos, nwPos, tmpVec);
			if (linAlg::vecLength(tmpVec) < radius) {//<--- check if cube is entirely inside sphere --	
				if (!currentOct->detailed)
					currentOct->data[0][0][0] = 255;
				else{
					currentOct->deAllocateData6();
					currentOct->data[0][0][0] = 255;
				}
			}// --->
			else{//<--- cube was not entirely inside sphere
				if (!currentOct->detailed){
					if (currentOct->data[0][0][0] != 255)
						currentOct->allocateData6();
				}
				else{ //<---- search detailed data and change it
					tmpPos[0] = currentOct->pos[0] - currentOct->halfDim + 0.001f;
					tmpPos[1] = currentOct->pos[1] - currentOct->halfDim + 0.001f;
					tmpPos[2] = currentOct->pos[2] - currentOct->halfDim + 0.001f;
					for (int i = 0; i < 16; i++){
						for (int j = 0; j < 16; j++){
							for (int k = 0; i < 16; k++){
								linAlg::calculateVec(tmpPos, nwPos, tmpVec);

								if (linAlg::vecLength(tmpVec) < radius)//check if point is inside sphere
									currentOct->data[i][j][k] = 255;

								tmpPos[0] += 0.001f; tmpPos[1] += 0.001f; tmpPos[2] += 0.001f;
							}
						}
					}
				}// ---->					
			}// --->
		}// -->

		olCounter = olStart;
		while (olCounter < tmpI){ //<-- march trough selected cubes and generate triangles

		}


	}// ->



}

