#pragma once
#include "nPolygon.h"

class LineSphere : public nPolygon
{
public:
	//! draws a sphere consisting of 3 circles with origin in (pX, pY, pZ) and radius r;
	LineSphere(float pX, float pY, float pZ, float r);
	~LineSphere();

	void render();
};

