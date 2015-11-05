#pragma once
class ScalarField
{
public:
	ScalarField(int rX, int rY, int rZ, int dX, int dY, int dZ);
	~ScalarField();

private:
	int res[3];
	float dim[3];
	unsigned char*** data;
	unsigned char isoValue;

	int edgeTable[256];
	int triTable[256][16];
};

