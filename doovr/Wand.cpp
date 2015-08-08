#include "Wand.h"


Wand::Wand()
{
}

void Wand::getPosition(float* vec) {
	vec[0] = Position[0]; 
	vec[1] = Position[1];
	vec[2] = Position[2];
}

float* Wand::getOrientation()
{

	float start[3] = { 0.0f, 0.0f, 1.0f };
	float temp[3];
	linAlg::normVec(Direction);
	linAlg::crossProd(temp, Direction, start);
	linAlg::normVec(temp);
	linAlg::rotAxis(temp, acos(linAlg::dotProd(Direction, start)), OrientationM);
	return OrientationM;
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

void Wand::getDirection(float* vec) {

	//TranslateM[3] = Position[0];
	//TranslateM[7] = Position[1];
	//TranslateM[11] = Position[2];

	//linAlg::matrixMult(TranslateM, OrientationM, Transform);

	//float v[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	//float dirr[4];
	//linAlg::vectorMatrixMult(OrientationM, v, dirr);
	//Direction[0] = Direction[0] - Position[0];
	//Direction[1] = Direction[1] - Position[1];
	//Direction[2] = Direction[2] - Position[2];
	//linAlg::normVec(Direction);

	vec[0] = Direction[0];
	vec[1] = Direction[1];
	vec[2] = Direction[2];
}

Wand::~Wand()
{
}
