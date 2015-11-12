#include "Add.h"
#include "LineSphere.h"
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
	float wPos[4]; float nwPos[4];
	float wDirr[4]; float nwDirr[4];
	double s, d = 0.0;


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

	//check if we are inside root of octree
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
	//we are inside root of octree
	if (d <= radius*radius)
	{
		while (currentOct->depth < 5)
		{
			for (int i = 0; i < 8; i++)
			{

			}
		}
	}



	

	


	

}

