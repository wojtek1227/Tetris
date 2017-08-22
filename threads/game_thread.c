#include "game_thread.h"
#include "../tetris/tetris.h"

void GameThread(void* params)
{
	uint8_t x,y;
	x = 0;
	y = 0;
	//Init
	//GUI_Init();
	TetrisInit();
	AngularRates_t *Gyro = (AngularRates_t *) params;
	uint32_t tick = osKernelGetTickCount();
	//TetrisInit();
	//
	while(1)
	{
		TetrisGame();
		// Execute all GUI jobs ... Return 0 if nothing was done
		GUI_Exec();
		// Nothing left to do for the moment ... Idle processing
		GUI_X_ExecIdle();
		tick +=100;
		osDelayUntil(tick);
	}
}
