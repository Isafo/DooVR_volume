#pragma once
#include "Utilities.h"
#include "linAlg.h"

class Wand
{
public:
	Wand();
	~Wand();

	void getPosition(float* vec) ;
	void getVelocity(float* vec);
	void getDirection(float* vec);
	float* getOrientation() { return OrientationM; }
	float* getTransformMatrix() { return Transform; }

	void setLastPos() { lastPosition[0] = Position[0]; lastPosition[1] = Position[1]; lastPosition[2] = Position[2]; }

	virtual void setPosition(double* t) = 0;
	virtual void setOrientation(double* o) = 0;
	virtual void setTransform(float* T) = 0;


protected:

	double Position[3];
	double lastPosition[3];
	float Fposition[3];

	float Velocity[3];

	float Direction[3];

	float OrientationM[16];
	float TranslateM[16];
	float Transform[16];
};

