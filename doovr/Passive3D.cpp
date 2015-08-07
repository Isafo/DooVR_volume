#include "Passive3D.h"
#include "linAlg.h"

using namespace wand3d;

Passive3D::Passive3D()
{
	//variables------------------------
	lastPosition[0] = Position[0] = 0.0; lastPosition[1] = Position[1] = 0.0; lastPosition[2] = Position[2] = 0.0;
	Velocity[0] = Fposition[0] = 0.0f; Velocity[1] = Fposition[1] = 0.0f; Velocity[2] = Fposition[2] = 0.0f;

	Transform[0] = 1.0f; Transform[1] = 0.0f; Transform[2] = 0.0f; Transform[3] = 0.0f;
	Transform[4] = 0.0f; Transform[5] = 1.0f; Transform[6] = 0.0f; Transform[7] = 0.0f;
	Transform[8] = 0.0f; Transform[9] = 0.0f; Transform[10] = 1.0f; Transform[11] = 0.0f;
	Transform[12] = 0.0f; Transform[13] = 0.0f; Transform[14] = 0.0f; Transform[15] = 1.0f;

	for (int i = 0; i < 16; i++)
		OrientationM[i] = TranslateM[i] = Transform[i];

	//connection----------------------
	try {
		wand = new Wand3d("COM3"); // usb port
	}
	catch (Wand3dSerialException error) {
		std::cout << error.what() << std::endl;
	}
	
	/*Network newNet;
	
	newNet.setSensorPosition(0, 0.15, 0.0, 0.082);
	newNet.setSensorPosition(1, -0.15, 0.0, 0.082);
	newNet.setSensorPosition(2, -0.15, 0.0, -0.082);
	newNet.setSensorPosition(3, 0.15, 0.0, -0.082);



	double rot[9] = { -1.f, 0.f, 0.f,
					   0.f, 0.f, 1.f,
					   0.f, 1.f, 0.f };

	double rotZ[9] = { 1.f, 0.f, 0.f,
					   0.f, 0.f, -1.f,
					   0.f, 1.f, 0.f };



	newNet.setSensorRotation(0, rot);
	newNet.setSensorRotation(1, rotZ);
	newNet.setSensorRotation(2, rotZ);
	newNet.setSensorRotation(3, rot);

	wand->setNetwork(newNet);*/

	Wand3dObserver* observer = this;
	wand->addObserver(observer);
	
	wand->start();
}

Passive3D::~Passive3D() {
	wand->removeObserver(this);
	delete wand;
}

void Passive3D::wand3dCallback(WandData data) {

	float Orient[16];

	//lastPosition[0] = Position[0];
	//lastPosition[1] = Position[1];
	//lastPosition[2] = Position[2];

	float dirr[4];
	float dirrResult[4];

	Position[0] = -data.position[0];
	Position[1] = data.position[2] - 0.27f;
	Position[2] = data.position[1] - 0.25f;

	utils::getGLRotMatrix(data, Orient);
	float rotZX[16] = { -1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.0f };
	linAlg::matrixMult(rotZX, Orient, OrientationM);

	Velocity[0] = data.velocity[0];
	Velocity[1] = data.velocity[1];
	Velocity[2] = data.velocity[2];


	dirr[0] = data.orientation[0];
	dirr[1] = data.orientation[1];
	dirr[2] = data.orientation[2];
	dirr[3] = 1.0f;
	linAlg::vectorMatrixMult(rotZX, dirr, dirrResult);

	Direction[0] = dirrResult[0];
	Direction[1] = dirrResult[1];
	Direction[2] = dirrResult[2];
}

void Passive3D::calibrate()
{
	wand->reCalibrate();
}

void Passive3D::setPosition(double* t) {
	// Change the coordinatesystem to match Oculus x->x, y->z, z->-y
	Position[0] = (float) -t[0];
	Position[1] = (float) t[2] - 0.27f;
	Position[2] = (float) t[1] - 0.25f;

	//wandPosition[0] = (float)t[0];
	//wandPosition[1] = (float)t[1];
	//wandPosition[2] = (float)t[2];

}

void Passive3D::setOrientation(double* o) {

	std::copy(o, o + 16, OrientationM);
}

void Passive3D::setTransform(float* T) {
	std::copy(T, T + 16, Transform);
}