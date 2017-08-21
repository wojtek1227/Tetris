#include "tetris.h"
#include "GUI.h"


#define BACKGROUND_COLOR GUI_BLUE

#define NUMBER_OF_BLOCKS 8
#define NUMBER_OF_POSITIONS 4

#define FIELD_X_SIZE 12
#define FIELD_Y_SIZE 15

#define SQUARE_SIZE 20
#define SQUARE_FRAME 1
#define SQUARE_FRAME_COLOR GUI_WHITE

#define COLOR_NUMBER 8

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
	GUI_YELLOW,
	GUI_GREEN,
	GUI_MAGENTA,
	GUI_RED
};
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
static const uint16_t block_type[NUMBER_OF_BLOCKS][NUMBER_OF_POSITIONS] = 
{
	{0, 0, 0, 0}, // No block
	{0x0F00, 0x4444, 0x00F0, 0x2222},// I
	{0x2260, 0x0470, 0x0644, 0x0E20},// reversed L
	{0x4460, 0x0740, 0x0622, 0x02E0},// L
	{0x0660, 0x0660, 0x0660, 0x0660},// square
	{0x06C0, 0x0462, 0x06C0, 0x0462},// S
	{0x0720, 0x0262, 0x04E0, 0x0464},// T
	{0x0630, 0x0264, 0x0630, 0x0264}// reversed S
};


//Global variables
static uint32_t field[FIELD_Y_SIZE][FIELD_X_SIZE] = {{0}};
Block_t current_block;
int16_t y_pos;
float y;

//Private functions

static void FieldInit(void)
{
	for(uint8_t y = 0; y < FIELD_Y_SIZE; y++)
	{
		for(uint8_t x = 0; x < FIELD_X_SIZE; x++)
		{
			field[y][x] = 0;
		}
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

static uint8_t CollisionDetection(const Block_t* block)
{
	uint16_t tmp = 0x8000;
	for(uint8_t i = 0; i < 4; i++)
	{
		for(uint8_t j = 0; j < 4; j++)
		{
			if ((block_type[block->type][block->position] & tmp) && (field[block->y + i][block->x + i] != 0))
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
	uint32_t random = 2;
	//ADD rng hal
	block->x = 4;
	block->y = 0;
	block->type = random % 7 + 1;
	block->position = random % 4;
	block->color_num = 6;
	//Collision detection
	if (CollisionDetection(block))
	{
		return 1;
	}
	return 0;
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
		}
	}
	return 0;
}
//Public functions
void TetrisInit(void)
{
	GUI_Init();
	FieldInit();
	//field[10][10] = GUI_MAGENTA;
	GUI_SetBkColor(BACKGROUND_COLOR);
	UpdateScreen();
	
	GUI_Exec();
}

//Use gyro data
void TetrisGyro(float y_axis_data)
{
	//static float y_pos;
	//y_pos += y_axis_data/114.285F;
	if ((y_axis_data/100.0F) > 150.0F)
	{
		y = y_axis_data;
		y_pos += 1;
	}
	if ((y_axis_data/100.0F) < (-150.0F))
	{	
		y = y_axis_data;
		y_pos -= 1;
	}
//	GUI_GotoXY(0, 300);
//	GUI_DispString("Y_pos: ");
//	GUI_DispFloat(y_pos, 10);
	//UpdateScreen();
	
}

//Rotate if button was pressed
void TetrisButton(void);
	
//Game function
void TetrisGame(void)
{
	Block_t block;
	BlockCreate(&current_block);
	BlockAdd(&current_block);
	GUI_GotoXY(0,300);
	GUI_DispString("Ypos: ");
	GUI_DispFloat(y_pos, 10);
	GUI_DispString("\nYaxis: ");
	GUI_DispFloat(y, 10);
	UpdateScreen();

}