#include "ScalarField.h"


ScalarField::ScalarField(int rX, int rY, int rZ, int dX, int dY, int dZ)
{
	//copy res and dim data
	res[0] = rX; res[1] = rY; res[2] = rZ;
	dim[0] = dX; dim[1] = dY; dim[2] = dZ;

	//allocate memory
	data = new unsigned char**[res[0]];
	for (int i = 0; i < res[0]; i++)
		data[i] = new unsigned char*[res[1]];

	for (int i = 0; i < res[0]; i++)
		for (int j = 0; j < res[1]; j++)
			data[i][j] = new unsigned char[res[2]];

	//init values
	for (int i = 0; i < res[0]; i++)
		for (int j = 0; j < res[1]; j++)
			for (int k = 0; k < res[2]; k++)
				data[i][j][k] = (i ) % 256;
	isoValue = 128;

}


ScalarField::~ScalarField()
{
}
