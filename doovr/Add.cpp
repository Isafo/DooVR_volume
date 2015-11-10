#include "Add.h"
#include "LineSphere.h"


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

void Add::changeScalarData(Wand* _wand, Octree* _ot, DynamicMesh* _mesh)
{
	float wPos[3]; float nwPos[3];
	float wDirr[3]; float nwDirr[3];


	linAlg::transpose(_mesh->orientation);
	//--< 1.0 | calculated the position and direction of the wand
	_wand->getPosition(wPos);
	wPos[0] = wPos[0] - _mesh->position[0];
	wPos[1] = wPos[1] - _mesh->position[1];
	wPos[2] = wPos[2] - _mesh->position[2];
	wPos[3] = 1.0f;
	linAlg::vectorMatrixMult(_mesh->orientation, wPos, nwPos);

	_wand->getDirection(wDirr);
	linAlg::normVec(wDirr);
	wDirr[3] = 1.0f;
	linAlg::vectorMatrixMult(_mesh->orientation, wDirr, nwDirr);
	linAlg::transpose(_mesh->orientation);



}

