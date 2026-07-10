/**
 ******************************************************************************
 * @file    pwm.h
 * @brief   PWM driver: TIM3 Channel 1 configured to drive the L298N's ENA
 *          pin, providing motor speed control.
 ******************************************************************************
 */

#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include <stdint.h>

/**
 * @brief Binds the PWM driver to a CubeMX-generated TIM3 handle and starts
 *        PWM generation on the configured channel at 0% duty cycle.
 * @param htim3 Pointer to the HAL TIM handle for TIM3 (owned by main.c,
 *              configured via MX_TIM3_Init() for PWM_FREQUENCY_HZ).
 * @retval HAL_StatusTypeDef HAL_OK on success.
 */
HAL_StatusTypeDef PWM_Init(TIM_HandleTypeDef *htim3);

/**
 * @brief Sets the motor drive PWM duty cycle.
 * @param dutyPercent Desired duty cycle, 0-100. Values outside this range
 *                     are clamped rather than rejected, since this function
 *                     sits on the safety-critical actuation path and must
 *                     never silently do nothing on a bad input.
 */
void PWM_SetDutyCycle(uint8_t dutyPercent);

/**
 * @brief Returns the currently applied duty cycle, as last set by
 *        PWM_SetDutyCycle(). Useful for UART telemetry reporting.
 * @retval uint8_t Current duty cycle, 0-100.
 */
uint8_t PWM_GetCurrentDutyCycle(void);

#ifdef __cplusplus
}
#endif

#endif /* PWM_H */
