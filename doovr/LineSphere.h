#pragma once
#include "Brush.h"

class LineSphere : public Brush
{
public:
	LineSphere(float pX, float pY, float pZ, float r);
	~LineSphere();

	void render();
};

