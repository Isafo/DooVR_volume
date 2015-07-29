#pragma once
#include "Brush.h"

class Square : public Brush
{
public:
	//! draws a square with the center in (pX, pY, pZ) and dimensions dX, dY
	Square(float pX, float pY, float pZ, float dX, float dY);
	~Square();

	void render();
};

