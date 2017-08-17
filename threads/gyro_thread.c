#include "gyro_thread.h"

void GyroThread(void* params)
{
	BSP_GYRO_Init();
	AngularRates_t* GyroReading = (AngularRates_t*)params;
	uint32_t tick = osKernelGetTickCount();
	while(1)
	{
		BSP_GYRO_GetXYZ(GyroReading->XYZ);
		tick += 500;
		osDelayUntil(tick);
	}
}
