#include "tetris.h"
#include "GUI.h"
#include "cmsis_os2.h"
#include "stm32f4xx_hal_rng.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f429xx.h"
#include "string.h"

#define BACKGROUND_COLOR GUI_BLUE

#define NUMBER_OF_BLOCKS 8
#define NUMBER_OF_POSITIONS 4
#define COLLISION_DETECTED 1

#define FIELD_X_SIZE 12
#define FIELD_Y_SIZE 15
#define FIELD_X_FULL_SIZE 13
#define FIELD_Y_FULL_SIZE 16

#define SQUARE_SIZE 20
#define SQUARE_FRAME 1
#define SQUARE_FRAME_COLOR GUI_WHITE

#define COLOR_NUMBER 8

#define MOVE_BLOCK_VALUE 3

#define FALL_TIME 5

#define X_MOVE_WAIT 3

//Types
typedef struct
{
	uint16_t x;
	uint16_t y;
	uint8_t type;
	uint8_t position;
	uint8_t color_num;
}Block_t;

/*
 *Colors
 */
static const uint32_t colors[COLOR_NUMBER] = 
{
	0, // indicates empty square
	GUI_CYAN,
	GUI_GRAY,
	GUI_ORANGE,
	GUI_MAGENTA,
	GUI_GREEN,
	GUI_YELLOW,
	GUI_RED
};

static RNG_HandleTypeDef RNGHandle = {(RNG_TypeDef*)RNG_BASE,0,0,0};
/*
 *Block types:
 * No block
 * I type block
 * reversed L type block
 * L type block
 * Square block
 * S type block
 * T type block
 * reversed S type block
 */

/*
 * Blocks are coded using 16bits
 * exmaple
 * 0000
 * 1111
 * 0000
 * 0000, gives us 0x0f00 
 *
 * 0010
 * 0010
 * 0010
 * 0010, gives us 0x4444
 */
 
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
	__HAL_RCC_RNG_CLK_ENABLE();
}


static const uint16_t block_type[NUMBER_OF_BLOCKS][NUMBER_OF_POSITIONS] = 
{
	{0, 0, 0, 0}, // No block
	{0x0F00, 0x8888, 0x00F0, 0x8888},// I
	{0x44C0, 0x08E0, 0x0C88, 0x0E20},// reversed L
	{0x88C0, 0x0E80, 0x0C44, 0x02E0},// L
	{0x0CC0, 0x0CC0, 0x0CC0, 0x0CC0},// square
	{0x06C0, 0x08C4, 0x06C0, 0x08C4},// S
	{0x0E40, 0x04C4, 0x04E0, 0x08C8},// T
	{0x0C60, 0x04C8, 0x0C60, 0x04C8}// reversed S
};


//Global variables
static uint32_t field[FIELD_Y_FULL_SIZE][FIELD_X_FULL_SIZE] = {{0}};
Block_t current_block;
int16_t y_pos;
float y;
static int16_t fall_time = 0;
static uint8_t move_time = 0;
//Private functions

static void FieldInit(void)
{
	for(uint8_t y = 0; y < FIELD_Y_FULL_SIZE; y++)
	{
		
		for(uint8_t x = 0; x < FIELD_X_FULL_SIZE; x++)
		{
			field[y][x] = 0;
			field[FIELD_Y_SIZE][x] = 0x01;
		}
		field[y][FIELD_X_SIZE] = 0x01;
	}
}
/*
 *Draws Square, 20x20, on field
 */

static void DrawSquare(uint16_t x, uint16_t y, uint32_t color_index)
{
	if ((x < FIELD_X_SIZE) && (y < FIELD_Y_SIZE))
	{
		uint32_t prev_color = GUI_GetColor();
		
		x *= SQUARE_SIZE;
		y *= SQUARE_SIZE;
		GUI_SetColor(SQUARE_FRAME_COLOR);
		GUI_DrawRect(x, y, x + SQUARE_SIZE - 1, y + SQUARE_SIZE - 1);
		GUI_SetColor(color_index);
		GUI_FillRect(x + SQUARE_FRAME, y + SQUARE_FRAME, x + SQUARE_SIZE - SQUARE_FRAME - 1, y + SQUARE_SIZE - SQUARE_FRAME - 1);
		
		GUI_SetColor(prev_color);
	}
}

/*
 *Erases square from field, 
 */
static void EraseSquare(uint16_t x, uint16_t y)
{
	uint32_t prev_color = GUI_GetColor();
	if ((x < FIELD_X_SIZE) && (y < FIELD_Y_SIZE))
	{
		GUI_SetColor(GUI_GetBkColor());
		x *= SQUARE_SIZE;
		y *= SQUARE_SIZE;
		GUI_FillRect(x, y, x + SQUARE_SIZE - 1, y + SQUARE_SIZE - 1);
		GUI_SetColor(prev_color);
	}
}

static void UpdateScreen(void)
{
	for(uint16_t y = 0; y < FIELD_Y_SIZE; y++)
	{
		for(uint16_t x = 0; x < FIELD_X_SIZE; x++)
		{
			if (field[y][x] == 0)
			{
				EraseSquare(x, y);
			}
			else
			{
				DrawSquare(x, y, field[y][x]);
			}
		}
	}
}

static uint8_t BlockCollision(const Block_t* block)
{
	uint16_t tmp = 0x8000;
	
	for(uint8_t i = 0; i < 4; i++)
	{
		for(uint8_t j = 0; j < 4; j++)
		{
			if ((block_type[block->type][block->position] & tmp) && (field[block->y + i][block->x + j] != 0))
			{
				return 1;
			}
			tmp >>= 1;
		}
	}
	return 0;
}

static uint8_t BlockCreate(Block_t* block)
{
	uint32_t random_type;
	uint32_t random_position;
	uint32_t random_color_num;
	HAL_RNG_GenerateRandomNumber(&RNGHandle,&random_type);
	HAL_RNG_GenerateRandomNumber(&RNGHandle,&random_position);
	HAL_RNG_GenerateRandomNumber(&RNGHandle,&random_color_num);
	random_type = (random_type % (NUMBER_OF_BLOCKS - 1)) + 1;
	random_position = (random_position % NUMBER_OF_POSITIONS);
	random_color_num = (random_color_num % (COLOR_NUMBER - 1)) + 1;
	block->x = 4;
	block->y = 0;
	block->type = random_type;
	block->position = random_position;
	block->color_num = random_color_num;
	//Collision detection
	return BlockCollision(block);
}

static void BlockAdd(const Block_t* block)
{
	uint16_t tmp = 0x8000;
	
	for(uint16_t i = 0; i < 4; i++)
	{
		for(uint16_t j = 0; j < 4; j++)
		{
			if (block_type[block->type][block->position] & tmp)
			{
				field[block->y + i][block->x + j] = colors[block->color_num];
			}
			tmp >>= 1;
		}
	}
}	

static void BlockDelete(Block_t *block)
{
	uint16_t tmp = 0x8000;
	if (block->type == 0)
	{
		return;
	}
	
	for(uint16_t i = 0; i < 4; i++)
	{
		for(uint16_t j = 0; j < 4; j++)
		{
			if (block_type[block->type][block->position] & tmp)
			{
				field[block->y + i][block->x + j] = 0;
			}
			tmp >>= 1;
		}
	}
}

static void BlockMove()
{
	if (y_pos > MOVE_BLOCK_VALUE)// zakres <-3,3> sprawdza sie calkiem dobrze, ale to tez kwestia wlasnych preferencji 
	{
		if (move_time == 0)
		{
			BlockDelete(&current_block);
			current_block.x < FIELD_X_SIZE-1 ? current_block.x++ : current_block.x;
			if (BlockCollision(&current_block))
			{
				current_block.x--;
			}
			move_time = 1;
		}
		BlockAdd(&current_block);
	}
	else if(y_pos < -(MOVE_BLOCK_VALUE))
	{
		if (move_time == 0)
		{
			BlockDelete(&current_block);
			current_block.x > 0 ? current_block.x-- : current_block.x; 
			if (BlockCollision(&current_block))
			{
				current_block.x++;
			}
			BlockAdd(&current_block);	
			move_time = 1;
			//y_pos = -5;
		}
	}
	if (move_time == X_MOVE_WAIT)
	{
		move_time = 0;
	}
	else if (move_time != 0)
	{
		move_time++;
	}
	
}

static void ClearLine(uint8_t line)
{
	for(uint8_t y = line; y > 0; y--)
	{
		memcpy(field[y], field[y - 1], FIELD_X_SIZE * 4);
	}
	memset(field[0], 0, FIELD_X_SIZE * 4);
}

static uint8_t CheckLine(void)
{
	uint8_t cleared_lines = 0;
	for(uint8_t y = 0; y < FIELD_Y_SIZE; y++)
	{
		for(uint8_t x = 0; x < FIELD_X_SIZE; x++)
		{
			if(field[y][x] == 0)
			{
				break;
			}
			if(x == (FIELD_X_SIZE - 1))
			{
				ClearLine(y);
				cleared_lines++;
			}
		}
	}
	return cleared_lines;
}


//Public functions
void TetrisInit(void)
{
	HAL_RNG_MspInit(&RNGHandle);
	HAL_RNG_Init(&RNGHandle);
	GUI_Init();
	FieldInit();
	BlockCreate(&current_block);
	//field[10][10] = GUI_MAGENTA;
	//BlockAdd(&current_block);
	GUI_SetBkColor(BACKGROUND_COLOR);
	UpdateScreen();
	
	GUI_Exec();
}

//Use gyro data
void TetrisGyro(float y_axis_data)
{
	//static float y_pos;
	//y_pos += y_axis_data/114.285F;
	if ((y_axis_data/100.0F) > 200.0F)
	{
		y = y_axis_data;
		y_pos += 1;
	}
	if ((y_axis_data/100.0F) < (-200.0F))
	{	
		y = y_axis_data;
		y_pos -= 1;
	}
	if (y_pos > 10 || y_pos < -10)
	{
		y_pos = 0;
	}
}

//Rotate if button was pressed
void TetrisButton(void)
{
	BlockDelete(&current_block);
	if (current_block.position == NUMBER_OF_POSITIONS - 1)
	{
		current_block.position = 0;
		if (BlockCollision(&current_block))
		{
			current_block.position = NUMBER_OF_POSITIONS - 1;
		}
	}
	else
	{
		current_block.position++;
		if (BlockCollision(&current_block))
		{
			current_block.position--;
		}
	}
	BlockAdd(&current_block);

}
	
//Game function
void TetrisGame(void)
{
	static uint8_t score;
	GUI_GotoXY(0,300);
	GUI_DispString("Ypos: ");
	GUI_DispFloat(y_pos, 10);
	GUI_DispString("\nScore: ");
	GUI_DispDec(score, 3);
	
	BlockDelete(&current_block);
	if(fall_time == 0)
	{
		
		current_block.y++;
	}
	fall_time++;
	fall_time %= FALL_TIME;
	if (BlockCollision(&current_block) == COLLISION_DETECTED)
	{
		current_block.y--;
		BlockAdd(&current_block);
		score += CheckLine();
		// if we have a collision after create a new block - it's game over
		if(BlockCreate(&current_block) == COLLISION_DETECTED)
		{
			BlockAdd(&current_block);
			UpdateScreen();
			GUI_Exec();
			while(1)
			{
				char score_string[19] = "Your score is: ";					
				sprintf(&score_string[15], "%d", score);
				GUI_SetFont(&GUI_FontComic18B_ASCII);
				GUI_DispStringAt(score_string, 60,140);
				GUI_DispStringAt("GAME OVER !!!", 65,160);
				GUI_DispStringAt("Click Reset to restart game !", 20,180);
			};
		}
	}
	BlockAdd(&current_block);
	BlockMove();
	UpdateScreen();
}


