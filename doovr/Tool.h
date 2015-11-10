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

	virtual void setRadius(float rad) = 0;
	virtual void setStrength(float str) = 0;
protected:

	//Polygon* toolBrush;
	nPolygon* toolBrush;

	float radius;
	float strength;
};

