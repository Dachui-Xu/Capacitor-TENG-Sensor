/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include "main.h"
#include "queue.h"
#include "event_groups.h"
#include <string.h>
#include "semphr.h"
#include "adc.h"
#include "FDC2214.h"
#include "key.h"

/* External variables */
extern UART_HandleTypeDef huart1;
extern QueueHandle_t xQueueADC;
extern QueueHandle_t xQueueCap;
extern EventGroupHandle_t xEventGroup;
extern SemaphoreHandle_t xUARTSemaphore;
extern SemaphoreHandle_t xUARTMutex;


/* UART transmit buffer */
#define UART_BUFFER_SIZE 512
char uart_tx_buffer[UART_BUFFER_SIZE];


/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
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
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_TX Init */
    hdma_usart3_tx.Instance = DMA1_Channel2;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart3_tx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/*
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
//	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&ch,1);
  return ch;
}
 
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}

/*!
 *  @brief      FreeRTOS中的串口打印
 *  @param      无
 *  @since      v1.0
 *  @descript   互斥量避免多任务读写uart，信号量用于得知串口是否发送完成。
 *  Sample usage:          xTaskCreate(UART_Task, "UART_Task", 256, NULL, 1, NULL);
 */
void UART_Task(void *pvParameters)
{
    ADC_Data_t adcData;
    Cap_Data_t capData;
    size_t offset;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20); // Task execution cycle 20ms

    for(;;)
    {
        offset = 0;
        memset(uart_tx_buffer, 0, sizeof(uart_tx_buffer));

        /* Get the current mode */
        EventBits_t uxBits = xEventGroupGetBits(xEventGroup);

        /* Get UART mutex */
        if(xSemaphoreTake(xUARTMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            /* need to read capacitance data */
            if((uxBits & MODE_CAP_ONLY_BIT) || (uxBits & MODE_BOTH_BIT))
            {
                /* Receive capacitive data from the queue, non-blocking */
                if(xQueueReceive(xQueueCap, &capData, 0) == pdTRUE)
                {
                    /* Add Identifier */
                    offset += snprintf(uart_tx_buffer + offset, UART_BUFFER_SIZE - offset, "CAP,");

                    /* Formatting capacitor data */
                    for(int i = 0; i < CAP_CHANNEL_NUM; i++)
                    {
                        int32_t capValue = (int32_t)(capData.capacitances[i] * 1000); // 放大 1000 倍，转换为整数
                        int ret = snprintf(uart_tx_buffer + offset, UART_BUFFER_SIZE - offset, "%d,", capValue);
                        if(ret < 0 || ret >= (UART_BUFFER_SIZE - offset))
                        {
                            // Handling buffer overflows
                            break;
                        }
                        else
                        {
                            offset += ret;
                        }
                    }

                    /* Remove the last comma and add a line terminator */
                    if(offset > 0)
                    {
                        uart_tx_buffer[offset - 1] = '\r';
                        uart_tx_buffer[offset++] = '\n';
                    }
                }
            }

            /* need to read ADC data */
            if((uxBits & MODE_ADC_ONLY_BIT) || (uxBits & MODE_BOTH_BIT))
            {
                /* Receive ADC data from the queue, non-blocking */
                if(xQueueReceive(xQueueADC, &adcData, 0) == pdTRUE)
                {
                    /* Add Identifier */
                    offset += snprintf(uart_tx_buffer + offset, UART_BUFFER_SIZE - offset, "ADC,");

                    /* Formatting ADC data */
                    for(int i = 0; i < ADC_BUFFER_SIZE; i++)
                    {
                        int32_t adcValue = (int32_t)(adcData.voltages[i] * 1000); // 放大 1000 倍，转换为整数
                        int ret = snprintf(uart_tx_buffer + offset, UART_BUFFER_SIZE - offset, "%d,", adcValue);
                        if(ret < 0 || ret >= (UART_BUFFER_SIZE - offset))
                        {
                            // Handling buffer overflows
                            break;
                        }
                        else
                        {
                            offset += ret;
                        }
                    }

                    /* Remove the last comma and add a line terminator */
                    if(offset > 0)
                    {
                        uart_tx_buffer[offset - 1] = '\r';
                        uart_tx_buffer[offset++] = '\n';
                    }
                }
            }

            /* Check if there is data to be sent */
            if(offset > 0)
            {
                /* Sending Data */
                if(HAL_UART_Transmit_DMA(&huart1, (uint8_t*)uart_tx_buffer, offset) == HAL_OK)
                {
                    /* Waiting for sending to complete */
                    if(xSemaphoreTake(xUARTSemaphore, pdMS_TO_TICKS(100)) != pdTRUE)
                    {
                        // Handling send timeouts
                    }
                }
                else
                {
                    // Handling UART Transmit Errors
                }
            }

            /* Release the UART mutex */
            xSemaphoreGive(xUARTMutex);
        }
        else
        {
            // Handle failure to acquire UART mutex
        }

        /* Accurate timing to ensure task cycle */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



/* UART transmission complete callback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xUARTSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}




void UART_Test(void *pvParameters)
{
    /* Prepare the message to send */

    while(1)
    {
        /* Transmit data using DMA */
        if(HAL_UART_Transmit_DMA(&huart1, (uint8_t*)uart_tx_buffer, strlen(uart_tx_buffer)) == HAL_OK)
        {
            /* Wait for transmission complete */
            if(xSemaphoreTake(xUARTSemaphore, portMAX_DELAY) == pdTRUE)
            {
                /* Transmission complete */
            }
        }
        else
        {
            /* Transmission error */
        }

        /* Delay for a while */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* USER CODE END 1 */
