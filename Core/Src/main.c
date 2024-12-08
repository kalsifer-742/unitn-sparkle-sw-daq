/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdbool.h>

#include "plotter.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define POINTS_N 2048
#define POINTS_STORE_MUL 8
#define POINTS_STORE_N (POINTS_STORE_MUL*POINTS_N)
#define POINTS_STORE_TIMESTAMPS_N (POINTS_STORE_MUL*2+1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t adcs_raw[POINTS_N];

uint32_t points_store[POINTS_STORE_N];
size_t points_store_len = 0;
uint32_t points_store_timestamps[POINTS_STORE_TIMESTAMPS_N];
size_t points_store_timestamps_len = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
bool adcs_half_complete = false;
bool adcs_complete = false;
uint32_t adcs_half_complete_time = 0;
uint32_t adcs_complete_time = 0;

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
	adcs_half_complete = true;
	adcs_half_complete_time = plotter_get_time_us();
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	adcs_complete = true;
	adcs_complete_time = plotter_get_time_us();
}

static void store_data() {
	uint32_t start = adcs_half_complete_time < adcs_complete_time ? POINTS_N / 2 : 0;
	uint32_t end_time = adcs_complete_time > adcs_half_complete_time ? adcs_complete_time : adcs_half_complete_time;

	if(points_store_timestamps_len < POINTS_STORE_TIMESTAMPS_N) {
		points_store_timestamps[points_store_timestamps_len++] = end_time;
	}

	if (points_store_len < POINTS_STORE_N) {
		memcpy(points_store+points_store_len, adcs_raw+start, (POINTS_N/2) * sizeof(uint32_t));
		points_store_len += POINTS_N / 2;
	}
}
static void send_points_store() {
	const char* names[2] = {"ADC1", "ADC2"};
	for(int i = 0; i < POINTS_STORE_MUL*2; i++) {
		uint32_t start_time = points_store_timestamps[i];
		uint32_t end_time = points_store_timestamps[i+1];

		plotter_send_2_interleaved_u16_signals_lerp_time(names, (uint16_t*)(points_store + i * (POINTS_N / 2)), POINTS_N / 2, start_time, end_time);
	}
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
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

//  while(HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK);
//  while(HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_LPUART1_UART_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Transmit(&hlpuart1, (uint8_t*)"#daq\n", 5, 1000);

  if (HAL_ADC_Start(&hadc2) != HAL_OK) {
	  HAL_UART_Transmit(&hlpuart1, (uint8_t*)"#adc2_err\n", 9, 1000);
  }
  if (HAL_ADCEx_MultiModeStart_DMA(&hadc1, adcs_raw, POINTS_N) != HAL_OK) {
	  HAL_UART_Transmit(&hlpuart1, (uint8_t*)"#adc1_err\n", 9, 1000);
  }
  points_store_timestamps[points_store_timestamps_len++] = plotter_get_time_us();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (plotter_get_time_us() < 3000000 && points_store_len < POINTS_STORE_N) {
	  if(adcs_half_complete) {
		  store_data();
		  adcs_half_complete = false;
	  }
	  if(adcs_complete) {
		  store_data();
		  adcs_complete = false;
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  send_points_store();
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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
