#pragma once
#include "nPolygon.h"

class Circle : public nPolygon
{
public:
	Circle();
	Circle(float pX, float pY, float pZ, float r);
	~Circle();

	void render();

private:
	void clean();

};

