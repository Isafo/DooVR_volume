#pragma once
#include "Brush.h"

class Square : public Brush
{
public:
	Square(float pX, float pY, float pZ, float dX, float dZ);
	~Square();

	void render();
};

