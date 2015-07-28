#pragma once
#include "Brush.h"

class Circle : public Brush
{
public:
	Circle();
	Circle(float pX, float pY, float pZ, float r);
	~Circle();

	void render();

};

