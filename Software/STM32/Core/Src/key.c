#include "key.h"
#include "main.h"


extern EventGroupHandle_t xEventGroup;

void Key_Task(void *pvParameters)
{
    EventBits_t uxBits;

    /* Initialization: Set the default mode to simultaneous detection mode, light up all LEDs */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);  // Turn on LED1
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);  // Turn on LED2

    /* Set the initial mode to MODE_BOTH_BIT */
    xEventGroupClearBits(xEventGroup, MODE_ADC_ONLY_BIT | MODE_CAP_ONLY_BIT | MODE_BOTH_BIT);
    xEventGroupSetBits(xEventGroup, MODE_BOTH_BIT);

    while(1)
    {
        /* Read the button status (low level is pressed) */
        if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
        {
            /* Debounce */
            vTaskDelay(pdMS_TO_TICKS(50));
            if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
            {
                /* Wait for the key to be released to prevent repeated triggering */
                while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }

                /* Switching Mode */
                uxBits = xEventGroupGetBits(xEventGroup);
                xEventGroupClearBits(xEventGroup, MODE_ADC_ONLY_BIT | MODE_CAP_ONLY_BIT | MODE_BOTH_BIT);

                if(uxBits & MODE_ADC_ONLY_BIT)
                {
                    /* Currently in ADC detection only mode, switch to capacitance detection only mode */
                    xEventGroupSetBits(xEventGroup, MODE_CAP_ONLY_BIT);

                    /* Update LED status */
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);   // Turn off LED1
                    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // Turn on LED2
                }
                else if(uxBits & MODE_CAP_ONLY_BIT)
                {
                    /* Currently in capacitance detection mode only, switch to simultaneous detection mode */
                    xEventGroupSetBits(xEventGroup, MODE_BOTH_BIT);

                    /* Update LED status */
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // Turn on LED1
                    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // Turn on LED2
                }
                else
                {
                    /* Currently in simultaneous detection mode or others, switch to ADC only detection mode */
                    xEventGroupSetBits(xEventGroup, MODE_ADC_ONLY_BIT);

                    /* Update LED status */
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // Turn on LED1
                    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);   // Turn off LED2
                }

                /* To prevent key jitter, delay for a while */
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }

        /* Task delay to avoid taking up too much CPU time */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


