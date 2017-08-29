#include "button_thread.h"
#include "../tetris/tetris.h"

void ButtonThread(void* params)
{
	
	uint32_t tick = osKernelGetTickCount();
	uint8_t curr_state, last_state;
	GPIO_InitTypeDef  GPIO_InitStruct;
	//init
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
	__GPIOA_CLK_ENABLE();
	//Led init for tests
	__GPIOG_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	
  
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
	while(1)
	{
		curr_state = BSP_PB_GetState(BUTTON_KEY);
		if ((curr_state == 1) && (last_state == 0))
		{
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
			TetrisButton();
		}
		last_state = curr_state;
		tick += 20;
		osDelayUntil(tick);
	}
}