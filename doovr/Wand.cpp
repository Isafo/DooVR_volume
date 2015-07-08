#include "Wand.h"


Wand::Wand()
{
}

float* Wand::getPosition()
{
	Fposition[0] = Position[0]; 
	Fposition[1] = Position[1]; 
	Fposition[2] = Position[2]; 
	return Fposition;
}
float* Wand::getVelocity()
{
	Velocity[0] = Position[0] - lastPosition[0];
	Velocity[1] = Position[1] - lastPosition[1];
	Velocity[2] = Position[2] - lastPosition[2];
	return Velocity;
}
float* Wand::getDirection()
{
	//TranslateM[3] = Position[0];
	//TranslateM[7] = Position[1];
	//TranslateM[11] = Position[2];

	//linAlg::matrixMult(TranslateM, OrientationM, Transform);

	float v[4] = { 0.0f, 0.0f, 0.2f, 1.0f };
	float dirr[4];
	linAlg::vectorMatrixMult(OrientationM, v, dirr);
	//Direction[0] = Direction[0] - Position[0];
	//Direction[1] = Direction[1] - Position[1];
	//Direction[2] = Direction[2] - Position[2];

	//linAlg::normVec(Direction);
	Direction[0] = dirr[0] + Position[0];
	Direction[1] = dirr[1] + Position[1];
	Direction[2] = dirr[2] + Position[2];
	return Direction;

}

Wand::~Wand()
{
}
