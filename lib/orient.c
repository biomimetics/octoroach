// Contents of this file are copyright Andrew Pullin, 2013

//orient.c

#include<math.h>
#include <string.h>

static float orient[3][3];

void orientSetup(){
	orient[0][0] = 1;
	orient[1][0] = 0;
	orient[2][0] = 0;

	orient[0][1] = 0;
	orient[1][1] = 1;
	orient[2][1] = 0;

	orient[0][2] = 0;
	orient[1][2] = 0;
	orient[2][2] = 1;
}

void orientUpdate(float* omega)
{
	float r = omega[0];
	float p = omega[1];
	float y = omega[2];
	float cp = cos(p);
	float sp = sin(p);
	float cr = cos(r);
	float sr = sin(r);
	float cy = cos(y);
	float sy = sin(y);

	float rot[3][3];
	rot[0][0] = cp*cy;
	rot[0][1] = -(cp*sy);
	rot[0][2] = sp;

	rot[1][0] = cy*sp*sr + cr*sy;
	rot[1][1] = cr*cy - sp*sr*sy;
	rot[1][2] = -(cp*sr);

	rot[2][0] = -(cr*cy*sp) + sr*sy;
	rot[2][1] = cy*sr + cr*sp*sy;
	rot[2][2] = cp*cr;

	int i = 0; int j = 0; int m = 0;

	float norient[3][3];

	float deltaT = 1.0 / 150.0;

	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			for(m = 0; m< 3;m++){
				norient[i][j] += rot[i][m]*orient[m][j];
			}
			norient[i][j] *= deltaT;
		}
	}

	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			orient[i][j] = norient[i][j];
		}
	}
	
};

void orientGetOrZ(float* dst){
	dst[0] = orient[0][2];
	dst[1] = orient[1][2];
	dst[2] = orient[2][2];
}
