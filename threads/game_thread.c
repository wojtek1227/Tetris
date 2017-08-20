#include "game_thread.h"
#include "../tetris/tetris.h"

void GameThread(void* params)
{
	uint8_t x,y;
	x = 0;
	y = 0;
	//Init
	GUI_Init();
	AngularRates_t *Gyro = (AngularRates_t *) params;
	uint32_t tick = osKernelGetTickCount();
	//TetrisInit();
	//
	while(1)
	{
//		GUI_Clear();
		GUI_SetBkColor(GUI_BLUE);
//		GUI_SetColor(GUI_RED);
//		GUI_DispString("x: ");
//		GUI_DispFloat(Gyro->x, 10);
//		GUI_DispString("\n");
//		GUI_DispString("y: ");
//		GUI_DispFloat(Gyro->y, 10);
//		GUI_DispString("\n");
//		GUI_DispString("z: ");
//		GUI_DispFloat(Gyro->z, 10);
//		GUI_DispString("\n");
//		GUI_DispString("Hello World!");
		GUI_SetColor(GUI_WHITE);
//		GUI_DrawRect(x,y, x+20, y+20);
//		GUI_SetColor(GUI_RED);
		
//		DrawSquare(x, y, GUI_GREEN);
//		x++;
//		y += (x/12);
//		x %=12;
//		y %=15;
//		GUI_FillRect(0, 260,20,280);
		TetrisGame();
		// Execute all GUI jobs ... Return 0 if nothing was done
		GUI_Exec();
		// Nothing left to do for the moment ... Idle processing
		GUI_X_ExecIdle();
		tick +=200;
		osDelayUntil(tick);
	}
}
