#ifndef GYRO_THREAD_H
#define GYRO_THREAD_H

#include "../gyroscope/stm32f429i_discovery_gyroscope.h"
#include "cmsis_os2.h"


typedef struct{
	float x;
	float y;
	float z;
}Axes_t;

typedef union {
	float XYZ[3];
	Axes_t Axes;
}AngularRates_t;

void GyroThread(void* params);

#endif
