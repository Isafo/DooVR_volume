#pragma once
#include "Brush.h"
#include "DynamicMesh.h"
#include "Wand.h"
#include "MatrixStack.h"

class Tool
{

public: 
	Tool();
	~Tool();
	
	virtual void render(MatrixStack* MVstack, GLint locationMV) = 0;

	virtual void firstSelect(DynamicMesh* mesh, Wand* wand) = 0;
	virtual void moveVertices(DynamicMesh* mesh, Wand* wand) = 0;
	virtual void deSelect() = 0;
protected:
	Brush* toolBrush;

	int* currentlySelectedVertices;
	int currentlySelectedSize;

	const int MAX_SELECTED = 10000;
};

