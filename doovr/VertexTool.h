#pragma once
#include "Tool.h"


class VertexTool : public Tool
{

public: 
	VertexTool();
	virtual ~VertexTool();
	
	virtual void render(MatrixStack* MVstack, GLint locationMV) = 0;
	virtual void renderIntersection(MatrixStack* MVstack, GLint locationMV) = 0;

	virtual void findIntersection(DynamicMesh* mesh, Wand* wand, int triIndex) = 0;
	virtual void firstSelect(DynamicMesh* mesh, Wand* wand) = 0;
	virtual void moveVertices(DynamicMesh* mesh, Wand* wand, float dT) = 0;
	virtual void deSelect() = 0;

	virtual void setRadius(float rad) = 0;
	virtual void setStrength(float str) = 0;

	virtual void getIntersection(float* intersectionP, float* intersectionN) {};
	//virtual void changeToolSize(bool sign) = 0;
protected:
	vertex intersection;

	int* selectedVertices;
	int selectedSize;

	const int MAX_SELECTED = 100000;
};

