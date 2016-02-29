#pragma once
#include "nPolygon.h"

class Line : public nPolygon
{
public:
	//! draws a line starting at position (pX, pY, pZ) going to position (pX, pY, pZ - length)
	Line(float pX, float pY, float pZ, float length);
	~Line();

	void clean();

	void render() override;
};

