#pragma once
#include "nPolygon.h"
#include "DynamicMesh.h"
#include "Wand.h"
#include "MatrixStack.h"

class Tool
{

public: 
	Tool();
	~Tool();
	
	virtual void render(MatrixStack* MVstack, GLint locationMV) = 0;
	virtual void renderIntersection(MatrixStack* MVstack, GLint locationMV) = 0;

	virtual void firstSelect(DynamicMesh* mesh, Wand* wand) = 0;
	virtual void moveVertices(DynamicMesh* mesh, Wand* wand) = 0;
	virtual void deSelect() = 0;

	virtual void setRadius(float rad) = 0;

	//virtual void changeToolSize(bool sign) = 0;
protected:

	//Polygon* toolBrush;
	nPolygon* toolBrush;
	int* selectedVertices;
	int selectedSize;

	const int MAX_SELECTED = 10000;
};

