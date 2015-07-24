#include "Wand.h"


Wand::Wand()
{
}

void Wand::getPosition(float* vec) {
	vec[0] = Position[0]; 
	vec[1] = Position[1];
	vec[2] = Position[2];
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
	vec[0] = Direction[0];
	vec[1] = Direction[1];
	vec[2] = Direction[2];
}

Wand::~Wand()
{
}
