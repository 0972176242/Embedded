/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BRAKE_NO_ABS	255
#define BRAKE_ABS		0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void normalRunning(void*);
void brakeWithABS(void*);
void brakeWithoutABS(void*);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * 0: vehicle speed = 100
 * 1: duty cycle or wheel speed (assume)
 * 2: ABS button press test
 * 3: no ABS button press test
 * */
uint8_t vehicleSpeed = 100;
uint8_t wheelSpeed;

const TickType_t xDelay = 500 / portTICK_PERIOD_MS; // delay for 500ms
float slip_ratio;

TaskHandle_t normalRunning_handle;
TaskHandle_t abs_handle;
TaskHandle_t no_abs_handle;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	slip_ratio = abs(vehicleSpeed - wheelSpeed) / (float)vehicleSpeed;
	HAL_UART_Receive_IT(huart, &wheelSpeed, 1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);

//  HAL_UART_Receive_IT(&huart1, dataFromSensor, 4);

  xTaskCreate(normalRunning, "Normal running", 100, NULL, 0, &normalRunning_handle);
  xTaskCreate(brakeWithABS, "Brake with ABS", 100, NULL, 0, &abs_handle);
  xTaskCreate(brakeWithoutABS, "Brake without ABS", 100, NULL, 0, &no_abs_handle);

  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 48000-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ABSlight_Pin|noABSlight_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ABSlight_Pin noABSlight_Pin */
  GPIO_InitStruct.Pin = ABSlight_Pin|noABSlight_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void normalRunning(void *parameter){
	while(1){
		vTaskSuspend(abs_handle);
		vTaskSuspend(no_abs_handle);

		/* take data from SensorSimu */
		HAL_UART_Receive_IT(&huart1, &wheelSpeed, 1);

		if (wheelSpeed == BRAKE_NO_ABS)
			vTaskResume(no_abs_handle);
		else if (wheelSpeed == BRAKE_ABS)
			vTaskResume(abs_handle);
		else {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, wheelSpeed);

			/* ABS active */
			if (slip_ratio >= 0.85)
				vTaskResume(abs_handle);

		}
	}
}

void brakeWithABS(void *parameter){
	while (1){	// while here to let this task conquer CPU
		vTaskSuspend(normalRunning_handle);
		vTaskSuspend(no_abs_handle);
		/* ABS light to notify ABS is on active */
		HAL_GPIO_WritePin(ABSlight_GPIO_Port, ABSlight_Pin, 1); // set user label for GPIO pin in MX

		uint8_t wSpeed = wheelSpeed;
		uint8_t vSpeed = vehicleSpeed;
		float slip_ratio_local;

		/* Slip stable phase */
		while(1){
			vSpeed -= 10;
			wSpeed += 2;
			slip_ratio_local = abs(vSpeed - wSpeed) / (float)vSpeed;

			if (slip_ratio_local < 0.4){
				/* Brake slowly phase */
				int8_t i;
				for (i = (int8_t) wSpeed; i >= 0; i -= 2){
					__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, i);
					vTaskDelay(xDelay);
				}

				vTaskDelay(2000/ portTICK_PERIOD_MS);
				HAL_GPIO_WritePin(ABSlight_GPIO_Port, ABSlight_Pin, 0);
				break;	// done ABS phase, return to normal
			} else {
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, wSpeed);
				vTaskDelay(xDelay);
			}
		}
		vTaskResume(normalRunning_handle);

		/* take wheel speed at point of block wheel */
//		while(1){
//
//			uint8_t vSpeed = vehicleSpeed;
//			int8_t dutyCycle = wheelSpeed;
//			int8_t i;
//
//			/** assume that when wheel is blocked
//			 * - Vehicle speed will gradually decrease
//			 * - wheel cycle < vehicle much (because it is blocked
//			 *
//			 * Solve: increase wheel slow until slip ratio is < 0.35 then decrease wheel to 0 slowly
//			 * @Follow: https://cecas.clemson.edu/cvel/auto/systems/braking.html
//			 *  */
//			if (vSpeed >= 2) // avoid vehicle = 0 (error divide 0)
//				vSpeed -= 2;
//
//
//			/* increase wheel speed to try to escape slip */
//			dutyCycle += 2;
//
//			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, dutyCycle);
//
//			slip_ratio = abs(vSpeed - dutyCycle) / (float)vSpeed;
//
//			/* Check slip ration */
//			if (slip_ratio < 0.35){
//	//				/* then stop the car slow */
//				for(i = (int8_t)dutyCycle; i >= 0; i--){
//					__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, i);
//					vTaskDelay(xDelay);
//				}
//
//				/* Resume normal running */
//				HAL_GPIO_WritePin(ABSlight_GPIO_Port, ABSlight_Pin, 0);
//				vTaskResume(normalRunning_handle);
//			}
//		}
	}
}

void brakeWithoutABS(void *parameter){
	while(1){
		vTaskSuspend(normalRunning_handle);
		vTaskSuspend(abs_handle);

		/* Turn on LED of braking without ABS */
		HAL_GPIO_WritePin(noABSlight_GPIO_Port, noABSlight_Pin, 1);

		/* Assume wheel speed approximate 0 when it is blocked */
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
//		HAL_Delay(2000);
		vTaskDelay(2000 / portTICK_PERIOD_MS); // do not work

		HAL_GPIO_WritePin(noABSlight_GPIO_Port, noABSlight_Pin, 0);

		vTaskResume(normalRunning_handle);
	}
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void){
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
