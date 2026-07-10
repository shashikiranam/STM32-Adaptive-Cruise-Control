/**
 ******************************************************************************
 * @file    timer.h
 * @brief   Timer driver: TIM2 configured as a free-running microsecond
 *          counter with input capture on Channel 1, used by the ultrasonic
 *          driver to measure HC-SR04 echo pulse width without blocking the
 *          main control loop.
 ******************************************************************************
 */

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initializes TIM2 for microsecond-resolution input capture on CH1
 *        (the HC-SR04 echo line). The timer prescaler is configured so the
 *        counter increments once per microsecond at the current SystemCoreClock.
 * @param htim2 Pointer to a HAL TIM handle owned by the caller (main.c),
 *              matching the CubeMX-generated MX_TIM2_Init() configuration.
 * @retval HAL_StatusTypeDef HAL_OK on success.
 */
HAL_StatusTypeDef Timer_InitEchoCapture(TIM_HandleTypeDef *htim2);

/**
 * @brief Starts input-capture interrupt mode on TIM2 CH1. Must be called once
 *        after Timer_InitEchoCapture(), typically during system init.
 * @param htim2 Pointer to the TIM2 handle.
 * @retval HAL_StatusTypeDef HAL_OK on success.
 */
HAL_StatusTypeDef Timer_StartEchoCapture(TIM_HandleTypeDef *htim2);

/**
 * @brief Input-capture callback hook, intended to be invoked from
 *        HAL_TIM_IC_CaptureCallback() in main.c (or a dedicated ISR file).
 *        Implements the rising/falling edge state machine that measures the
 *        HC-SR04 echo pulse width in microseconds.
 * @param htim2 Pointer to the TIM2 handle that generated the capture event.
 */
void Timer_EchoCaptureCallback(TIM_HandleTypeDef *htim2);

/**
 * @brief Returns whether a new, complete echo pulse-width measurement is
 *        available since the last call to Timer_GetLastPulseWidthUS().
 * @retval true if a new measurement is ready, false otherwise.
 */
bool Timer_IsNewPulseWidthAvailable(void);

/**
 * @brief Retrieves the most recently measured echo pulse width, in
 *        microseconds, and clears the "new measurement available" flag.
 * @retval uint32_t Pulse width in microseconds. Value is undefined if
 *         Timer_IsNewPulseWidthAvailable() returned false.
 */
uint32_t Timer_GetLastPulseWidthUS(void);

/**
 * @brief Marks the current control cycle's echo measurement as timed out.
 *        Called by the ultrasonic driver when US_ECHO_TIMEOUT_MS elapses
 *        with no completed capture. Resets the internal capture state
 *        machine so the next trigger starts cleanly.
 */
void Timer_AbortEchoCapture(void);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H */
