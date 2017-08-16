#include "GUI.h"
#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "../gyroscope/stm32f429i_discovery_gyroscope.h"
#include "../threads/gyro_thread.h"

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
 
  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
 
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
 
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}


void GUIThread(void* params) {
	float XYZ[3];
	AngularRates_t x;
	// Initialize the Graphics Component
	GUI_Init();
	// Put some string to display
	GUI_DispString("Hello World!");
	uint64_t tick;
  tick = osKernelGetTickCount(); 
	while (1) {
		GUI_Clear();
		BSP_GYRO_GetXYZ(x.XYZ);
		GUI_DispString("x: ");
		GUI_DispFloat(x.Axes.x, 10);
		GUI_DispString("\n");
		GUI_DispString("y: ");
		GUI_DispFloat(x.Axes.y, 10);
		GUI_DispString("\n");
		GUI_DispString("z: ");
		GUI_DispFloat(x.Axes.z, 10);
		GUI_DispString("\n");
		// Execute all GUI jobs ... Return 0 if nothing was done
		GUI_Exec();
		// Nothing left to do for the moment ... Idle processing
		GUI_X_ExecIdle();
		tick += 400;                           // delay 1000 ticks periodically
    osDelayUntil(tick);
	}
}

int main(void)
{
	// Initialize the RTOS Kernel.
    osKernelInitialize();
    // Initialize STM32Cube HAL library
    HAL_Init();
    // Initialize PLL
    SystemClock_Config();
		BSP_GYRO_Init();
    // Create and run the GUI thread
    osThreadNew(GUIThread, NULL, NULL);
    // Start the OS
    osKernelStart();
 
    return 0;
}