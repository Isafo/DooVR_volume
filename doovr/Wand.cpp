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

	float v[3] = { 0.0f, 0.0f, 1.0f };
	linAlg::vectorMatrixMult(OrientationM, v, Direction);
	//Direction[0] = Direction[0] - Position[0];
	//Direction[1] = Direction[1] - Position[1];
	//Direction[2] = Direction[2] - Position[2];

	//linAlg::normVec(Direction);

	return Direction;

}

Wand::~Wand()
{
}
