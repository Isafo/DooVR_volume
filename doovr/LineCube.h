#pragma once
#include "nPolygon.h"
class LineCube : public nPolygon
{
public:
	//! draws a cube, using lines, with the center in (pX, pY, pZ) and dimensions dX, dY, dZ
	LineCube(float pX, float pY, float pZ, float dX, float dY, float dZ);
	~LineCube();

	void render() override;

private:
	void clean();
};

