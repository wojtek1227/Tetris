#include "game_thread.h"

void GameThread(void* params)
{
	//Init
	GUI_Init();
	AngularRates_t *Gyro = (AngularRates_t *) params;
	uint32_t tick = osKernelGetTickCount();
	//
	while(1)
	{
		GUI_Clear();
		GUI_DispString("x: ");
		GUI_DispFloat(Gyro->x, 10);
		GUI_DispString("\n");
		GUI_DispString("y: ");
		GUI_DispFloat(Gyro->y, 10);
		GUI_DispString("\n");
		GUI_DispString("z: ");
		GUI_DispFloat(Gyro->z, 10);
		GUI_DispString("\n");
		GUI_DispString("Hello World!");
		// Execute all GUI jobs ... Return 0 if nothing was done
		GUI_Exec();
		// Nothing left to do for the moment ... Idle processing
		GUI_X_ExecIdle();
		tick += 300;
		osDelayUntil(tick);
	}
}
