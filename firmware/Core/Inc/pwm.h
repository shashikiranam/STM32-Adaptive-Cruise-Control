#ifndef __PWM_H
#define __PWM_H

#include <stdint.h>

void PWM_Init(void);

void PWM_SetDutyCycle(uint8_t duty);

#endif
