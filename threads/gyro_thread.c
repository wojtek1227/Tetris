#include "gyro_thread.h"
#include "../tetris/tetris.h"

void GyroThread(void* params)
{
	
	AngularRates_t GyroReading;
	uint32_t tick = osKernelGetTickCount();
	
	BSP_GYRO_Init();
	
	while(1)
	{
		BSP_GYRO_GetXYZ(GyroReading.XYZ);
		TetrisGyro(GyroReading.y);
		tick += 50;
		osDelayUntil(tick);
	}
}
