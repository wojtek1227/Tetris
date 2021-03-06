#ifndef GYRO_THREAD_H
#define GYRO_THREAD_H

#include "../gyroscope/stm32f429i_discovery_gyroscope.h"
#include "cmsis_os2.h"

#pragma anon_unions

//typedef struct{
//	float x;
//	float y;
//	float z;
//}Axes_t;

typedef union {
	float XYZ[3];
	struct
		{
			float x;
			float y;
			float z;
		};
}AngularRates_t;

void GyroThread(void* params);

#endif
