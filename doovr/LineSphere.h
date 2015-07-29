#pragma once
#include "Brush.h"

class LineSphere : public Brush
{
public:
	//! draws a sphere consisting of 3 circles with origin in (pX, pY, pZ) and radius r;
	LineSphere(float pX, float pY, float pZ, float r);
	~LineSphere();

	void render();
};

