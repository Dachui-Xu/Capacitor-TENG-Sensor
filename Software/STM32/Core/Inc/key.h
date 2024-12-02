#ifndef __KEY_H
#define __KEY_H

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

/* Mode defined event bits */
#define MODE_ADC_ONLY_BIT   (1 << 0)  // ADC detection mode only
#define MODE_CAP_ONLY_BIT   (1 << 1)  // Capacitive detection mode only
#define MODE_BOTH_BIT       (1 << 2)  // Simultaneous ADC and capacitor detection


/* Key task function declaration */
void Key_Task(void *pvParameters);

#endif 





