#include "Wand.h"


Wand::Wand()
{
}

float* Wand::getPosition() {
	Fposition[0] = Position[0]; 
	Fposition[1] = Position[1]; 
	Fposition[2] = Position[2]; 
	return Fposition;
}

void Wand::getVelocity(float* vec) {

	float rotZX[16] = { -1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.0f };
	float v[4] = { Velocity[0], Velocity[1], Velocity[2], 1.0f };
	float vFinal[4];
	linAlg::vectorMatrixMult(rotZX, v, vFinal);

	vec[0] = vFinal[0]; vec[1] = vFinal[1]; vec[2] = vFinal[2];
}

float* Wand::getDirection() {
	//TranslateM[3] = Position[0];
	//TranslateM[7] = Position[1];
	//TranslateM[11] = Position[2];

	//linAlg::matrixMult(TranslateM, OrientationM, Transform);

	float v[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	float dirr[4];
	linAlg::vectorMatrixMult(OrientationM, v, dirr);
	//Direction[0] = Direction[0] - Position[0];
	//Direction[1] = Direction[1] - Position[1];
	//Direction[2] = Direction[2] - Position[2];
	//linAlg::normVec(Direction);
	Direction[0] = dirr[0];// +Position[0];
	Direction[1] = dirr[1];// + Position[1];
	Direction[2] = dirr[2];// + Position[2];
	return Direction;
}

Wand::~Wand()
{
}
