#pragma once
#include "Brush.h"
class LineCube : public Brush
{
public:
	LineCube(float pX, float pY, float pZ, float dX, float dY, float dZ);
	~LineCube();

	void render();
};

