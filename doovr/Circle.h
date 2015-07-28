#pragma once
#include "Brush.h"

class Circle : public Brush
{
public:
	Circle();
	Circle(float cX, float cY, float r);
	~Circle();

	void render();

};

