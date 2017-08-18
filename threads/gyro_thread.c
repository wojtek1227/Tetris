#include "gyro_thread.h"

void GyroThread(void* params)
{
	
	AngularRates_t* GyroReading = (AngularRates_t*)params;
	uint32_t tick = osKernelGetTickCount();
	
	BSP_GYRO_Init();
	
	while(1)
	{
		BSP_GYRO_GetXYZ(GyroReading->XYZ);
		tick += 1000;
		osDelayUntil(tick);
	}
}
