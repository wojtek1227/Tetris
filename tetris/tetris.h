#ifndef TETRIS_H
#define TETRIS_H

#include "stdint.h"

#define GYRO_DELAY 300
#define BUTTON_DELAY 20
#define GAME_DELAY 400

//TetrisInit
void TetrisInit(void);

//Use gyro data
void TetrisGyro(float y_axis_data);

//Rotate if button was pressed
void TetrisButton(void);
	
//Game function
void TetrisGame(void);


//static
//void DrawSquare(uint16_t x, uint16_t y, uint32_t color_index);
//void EraseSquare(uint8_t x, uint8_t y);

#endif
