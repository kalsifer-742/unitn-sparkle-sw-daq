/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.c
  * @brief   This file provides code for the configuration
  *          of all the requested memory to memory DMA transfers.
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
#include "dma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
DMA_HandleTypeDef hdma_dma_generator0;

/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_dma_generator0
  */
void MX_DMA_Init(void)
{

  /* Local variables */
  HAL_DMA_MuxRequestGeneratorConfigTypeDef pRequestGeneratorConfig = {0};

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* Configure DMA request hdma_dma_generator0 on DMA1_Channel2 */
  hdma_dma_generator0.Instance = DMA1_Channel2;
  hdma_dma_generator0.Init.Request = DMA_REQUEST_GENERATOR0;
  hdma_dma_generator0.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_dma_generator0.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_dma_generator0.Init.MemInc = DMA_MINC_ENABLE;
  hdma_dma_generator0.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_dma_generator0.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_dma_generator0.Init.Mode = DMA_NORMAL;
  hdma_dma_generator0.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_dma_generator0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the DMAMUX request generator for the selected DMA channel */
  pRequestGeneratorConfig.SignalID = HAL_DMAMUX1_REQ_GEN_DMAMUX1_CH0_EVT;
  pRequestGeneratorConfig.Polarity = HAL_DMAMUX_REQ_GEN_RISING;
  pRequestGeneratorConfig.RequestNumber = 1;
  if (HAL_DMAEx_ConfigMuxRequestGenerator(&hdma_dma_generator0, &pRequestGeneratorConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMAMUX_OVR_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMAMUX_OVR_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMAMUX_OVR_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

