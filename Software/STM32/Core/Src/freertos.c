/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "adc.h"
#include "key.h"
#include "FDC2214.h"
#include "usart.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
QueueHandle_t xQueueADC;
QueueHandle_t xQueueCap;
EventGroupHandle_t xEventGroup;
SemaphoreHandle_t xI2CMutex;
SemaphoreHandle_t xADCSemaphore;
SemaphoreHandle_t xUARTSemaphore;
SemaphoreHandle_t xUARTMutex;

BaseType_t xReturn;

/* Task Handles */
TaskHandle_t xADCTaskHandle = NULL;
TaskHandle_t xFDC2214TaskHandle = NULL;
TaskHandle_t xUARTTaskHandle = NULL;
TaskHandle_t xKeyTaskHandle = NULL;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
	/* Create event group */
	xEventGroup = xEventGroupCreate();
	
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
	
	xI2CMutex = xSemaphoreCreateMutex();
	if (xI2CMutex == NULL)
	{
			/* Handle mutex creation failure */
	}
	 // Creating a UART Mutex
	xUARTMutex = xSemaphoreCreateMutex();
	if (xUARTMutex == NULL)
	{
			printf("Failed to create UART Mutex.\n");
			while (1);
	}
	
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	xADCSemaphore = xSemaphoreCreateBinary();
	if (xADCSemaphore == NULL)
	{
			/* Handle semaphore creation failure */
	}
	xUARTSemaphore = xSemaphoreCreateBinary();
	if (xUARTSemaphore == NULL)
	{
			/* Handle semaphore creation failure */
	}
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	
  /* add queues, ... */
	  xQueueADC = xQueueCreate(10, sizeof(ADC_Data_t)); // Queue length: 10
    xQueueCap = xQueueCreate(10, sizeof(Cap_Data_t));

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	/* 创建 UART 任务 */
	xReturn = xTaskCreate(UART_Task, "UART_Task", 256, NULL, 2, &xUARTTaskHandle);
	if(xReturn != pdPASS)
	{
		printf("UART_Task Failed");
			// 处理任务创建失败
			while(1);
	}

	/* 创建 ADC 任务 */
	xReturn = xTaskCreate(ADC_Task, "ADC_Task", 256, NULL, 3, &xADCTaskHandle);
	if(xReturn != pdPASS)
	{
		printf("ADC_Task Failed");
			// 处理任务创建失败
			while(1);
	}

	/* 创建电容检测任务 */
	xReturn = xTaskCreate(FDC2214_Task, "FDC2214_Task", 256, NULL, 3, &xFDC2214TaskHandle);
	if(xReturn != pdPASS)
	{
		printf("FDC2214_Task Failed");
			// 处理任务创建失败
			while(1);
	}

	/* 创建按键任务 */
	xReturn = xTaskCreate(Key_Task, "Key_Task", 256, NULL, 4, &xKeyTaskHandle);
	if(xReturn != pdPASS)
	{
		printf("Key_Task Failed");
			// 处理任务创建失败
			while(1);
	}
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

