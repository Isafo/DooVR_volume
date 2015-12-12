#pragma once
#include "nPolygon.h"

class Square : public nPolygon
{
public:
	//! draws a square with the center in (pX, pY, pZ) and dimensions dX, dY
	Square(float pX, float pY, float pZ, float dX, float dY);
	~Square();

	void render();

private:
	void clean();

};

