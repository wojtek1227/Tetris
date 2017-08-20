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
	uint8_t x;
	uint8_t y;
	uint8_t type;
	uint8_t position;
	uint8_t color_num;
}Block_t;

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

static const uint16_t block_type[NUMBER_OF_BLOCKS][NUMBER_OF_POSITIONS] = 
{
	{0, 0, 0, 0},
	{0x0F00, 0x4444, 0x00F0, 0x2222},
	{0x2260, 0x0470, 0x0644, 0x0E20},
	{0x4460, 0x0740, 0x0622, 0x02E0},
	{0x0660, 0x0660, 0x0660, 0x0660},
	{0x06C0, 0x0462, 0x06C0, 0x0462},
	{0x0720, 0x0262, 0x04E0, 0x0464},
	{0x0630, 0x0264, 0x0630, 0x0264}
};


//Global variables
/*static*/ uint8_t field[FIELD_X_SIZE][FIELD_Y_SIZE];
Block_t current_block;

//Private functions
/*static*/ void DrawSquare(uint16_t x, uint16_t y, uint32_t color_index)
{
	if ((x < FIELD_X_SIZE) && (y < FIELD_Y_SIZE))
	{
		uint32_t prev_color = GUI_GetColor();
		
		x *= SQUARE_SIZE;
		y *= SQUARE_SIZE;
		GUI_SetColor(SQUARE_FRAME_COLOR);
		GUI_DrawRect(x, y, x + SQUARE_SIZE, y + SQUARE_SIZE);
		GUI_SetColor(color_index);
		GUI_FillRect(x + SQUARE_FRAME, y + SQUARE_FRAME, x + SQUARE_SIZE - SQUARE_FRAME, y + SQUARE_SIZE - SQUARE_FRAME);
		
		GUI_SetColor(prev_color);
	}
}

/*static*/ void EraseSquare(uint16_t x, uint16_t y)
{
	uint32_t prev_color = GUI_GetColor();
	if ((x < FIELD_X_SIZE) && (y < FIELD_Y_SIZE))
	{
		GUI_SetColor(GUI_GetBkColor());
		x *= SQUARE_SIZE;
		y *= SQUARE_SIZE;
		GUI_FillRect(x, y, x + SQUARE_SIZE, y + SQUARE_SIZE);
		GUI_SetColor(prev_color);
	}
}

static void UpdateScreen(void)
{
	for(uint8_t y = 0; y < FIELD_Y_SIZE; y++)
	{
		for(uint8_t x = 0; x < FIELD_X_SIZE; x++)
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

static uint8_t CollisionDetection(Block_t block)
{
	return 0;
}

/*static*/ uint8_t CreateBlock(Block_t* block)
{
	uint32_t random = 3;
	//popiac hala
	block->x = 5;
	block->y = 0;
	block->type = random % 7 + 1;
	block->position = random % 4;
	block->color_num = random % 7;
	//Collision detection
	if (CollisionDetection(*block))
	{
		return 1;
	}
	return 0;
}

static void AddBlock(const Block_t* block)
{
	uint16_t tmp = 0x8000;
	
	for(uint8_t i = 0; i < 4; i++)
	{
		for(uint8_t j = 0; j < 4; j++)
		{
			if (block_type[block->type][block->position] & tmp)
			{
				field[block->y + i][block->x + j] = colors[block->color_num];
			}
			tmp >>= 1;
		}
	}
}	

//Public functions
void TetrisInit(void)
{
	GUI_Init();
}

//Use gyro data
void TetrisGyro(float y_axis_data);

//Rotate if button was pressed
void TetrisButton(void);
	
//Game function
void TetrisGame(void)
{
	Block_t block;
	CreateBlock(&block);
	AddBlock(&block);
	UpdateScreen();
}