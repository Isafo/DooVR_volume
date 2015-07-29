#pragma once
#include "Brush.h"

class Line : public Brush
{
public:
	//! draws a line starting at position (pX, pY, pZ) going to position (pX, pY, pZ - length)
	Line(float pX, float pY, float pZ, float length);
	~Line();

	void render();
};

