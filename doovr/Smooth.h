#pragma once
#include "Tool.h"
#include "Line.h"
#include "MatrixStack.h"
class Smooth : public Tool
{
public:
	Smooth(DynamicMesh* mesh, Wand* wand);
	~Smooth();

	void render(MatrixStack* MVstack, GLint locationMV);

	void firstSelect(DynamicMesh* mesh, Wand* wand);
	void moveVertices(DynamicMesh* mesh, Wand* wand);
	void deSelect();

private:
	float radius;

	Line* pointer;
	float lineOffset[3];

	vertex intersection;

	int* previouslySelectedVertices;
	int previouslySelectedSize;

	vertex* mVertexArray;
	halfEdge* mEdgeArray;
	vInfo* mVInfoArray;
	float* mPosition;
	float* mOrientation;

	float* wPosition;
	float* wDirection;
	float* wOrientation;

	float mMAX_LENGTH;

	float tempVec[3];

};

