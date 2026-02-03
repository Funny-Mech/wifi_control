/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "spi.h"
#include "lcd_spi_200.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef hspi6;

/*************************************************************************************************
*	函 数 名:	MX_SPI6_Init
*	入口参数:	无
*	返 回 值:	无
*	函数功能:	初始化SPI配置
*	说    明:使用硬件片选
*************************************************************************************************/

void MX_SPI6_Init(void)
{
	LCD_SPI.Instance 									= SPI6;							   					//	使用SPI
	LCD_SPI.Init.Mode 								= SPI_MODE_MASTER;            					//	主机模式
	LCD_SPI.Init.Direction 							= SPI_DIRECTION_1LINE;       					   //	单线
	LCD_SPI.Init.DataSize 							= SPI_DATASIZE_8BIT;          					//	8位数据宽度
	LCD_SPI.Init.CLKPolarity 						= SPI_POLARITY_LOW;           					//	CLK空闲时保持低电平
	LCD_SPI.Init.CLKPhase 							= SPI_PHASE_1EDGE;            					//	数据在CLK第一个边沿有效
	LCD_SPI.Init.NSS 									= SPI_NSS_HARD_OUTPUT;        					//	使用硬件片选

// 在 main.c文件 SystemClock_Config()函数里设置 SPI6 的内核时钟为137.5M，然后再经过2分频得到 68.75M 的SCK驱动时钟
	LCD_SPI.Init.BaudRatePrescaler 				= SPI_BAUDRATEPRESCALER_2;

	LCD_SPI.Init.FirstBit	 						= SPI_FIRSTBIT_MSB;									//	高位在先
	LCD_SPI.Init.TIMode 								= SPI_TIMODE_DISABLE;         					//	禁止TI模式
	LCD_SPI.Init.CRCCalculation					= SPI_CRCCALCULATION_DISABLE; 					//	禁止CRC
	LCD_SPI.Init.CRCPolynomial 					= 0x0;                        					// CRC校验项，这里用不到
	LCD_SPI.Init.NSSPMode 							= SPI_NSS_PULSE_DISABLE;      					//	不使用片选脉冲模式
	LCD_SPI.Init.NSSPolarity 						= SPI_NSS_POLARITY_LOW;      						//	片选低电平有效
	LCD_SPI.Init.FifoThreshold 					= SPI_FIFO_THRESHOLD_02DATA;  					//	FIFO阈值
	LCD_SPI.Init.TxCRCInitializationPattern 	= SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;   // 发送端CRC初始化模式，这里用不到
	LCD_SPI.Init.RxCRCInitializationPattern 	= SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;   // 接收端CRC初始化模式，这里用不到
	LCD_SPI.Init.MasterSSIdleness 				= SPI_MASTER_SS_IDLENESS_00CYCLE;            // 额外延迟周期为0
	LCD_SPI.Init.MasterInterDataIdleness 		= SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;     // 主机模式下，两个数据帧之间的延迟周期
	LCD_SPI.Init.MasterReceiverAutoSusp 		= SPI_MASTER_RX_AUTOSUSP_DISABLE;            // 禁止自动接收管理
	LCD_SPI.Init.MasterKeepIOState 				= SPI_MASTER_KEEP_IO_STATE_DISABLE; 	 		//	主机模式下，禁止SPI保持当前引脚状态
	LCD_SPI.Init.IOSwap 							= SPI_IO_SWAP_DISABLE;				            // 不交换MOSI和MISO

   HAL_SPI_Init(&LCD_SPI);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(spiHandle->Instance==SPI6)
  {
	  __HAL_RCC_SPI6_CLK_ENABLE();     // 使能SPI时钟

	        __HAL_RCC_GPIOG_CLK_ENABLE();    // 使能 SPI GPIO 时钟
	        GPIO_LDC_Backlight_CLK_ENABLE;   // 使能 背光        引脚时钟
	        GPIO_LDC_DC_CLK_ENABLE;          // 使能 数据指令选择 引脚时钟

	  /******************************************************
	  		PG8     ------> SPI6_NSS
	  		PG13    ------> SPI6_SCK
	  		PG14    ------> SPI6_MOSI

	        PG12    ------> 背光  引脚
	        PG15    ------> 数据指令选择 引脚
	  *******************************************************/

	  // 初始化 SCK、MOSI、片选引脚
	  		GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_13|GPIO_PIN_14;
	  		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;						// 复用推挽输出
	  		GPIO_InitStruct.Pull = GPIO_NOPULL;								// 无上下拉
	  		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;			// 最高速度等级
	  		GPIO_InitStruct.Alternate = GPIO_AF5_SPI6;					// 复用到SPI，复用线5
	  		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	  // 初始化 背光 引脚
	  		GPIO_InitStruct.Pin 		= LCD_Backlight_PIN;				// 背光 引脚
	  		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// 推挽输出模式
	  		GPIO_InitStruct.Pull 	= GPIO_PULLDOWN;					// 下拉，默认保持低电平
	  		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;			// 速度等级低
	  		HAL_GPIO_Init(LCD_Backlight_PORT, &GPIO_InitStruct);	// 初始化

	  // 初始化 数据指令选择 引脚
	  		GPIO_InitStruct.Pin 		= LCD_DC_PIN;				      // 数据指令选择 引脚
	  		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// 推挽输出模式
	  		GPIO_InitStruct.Pull 	= GPIO_NOPULL;						// 无上下拉
	  		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;			// 速度等级低
	  		HAL_GPIO_Init(LCD_DC_PORT, &GPIO_InitStruct);	      // 初始化
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI6)
  {
  /* USER CODE BEGIN SPI6_MspDeInit 0 */

  /* USER CODE END SPI6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI6_CLK_DISABLE();

    /**SPI6 GPIO Configuration
    PA0     ------> SPI6_NSS
    PA5     ------> SPI6_SCK
    PA7     ------> SPI6_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_5|GPIO_PIN_7);

  /* USER CODE BEGIN SPI6_MspDeInit 1 */

  /* USER CODE END SPI6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
