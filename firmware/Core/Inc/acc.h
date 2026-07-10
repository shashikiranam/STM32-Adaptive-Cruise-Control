/**
 ******************************************************************************
 * @file    acc.h
 * @brief   Adaptive Cruise Control / Collision Avoidance application core.
 *          Implements the system finite state machine, the proportional
 *          adaptive speed control law, ADC-based cruise setpoint reading,
 *          status LED updates, and UART telemetry reporting.
 *
 *          See docs/ALGORITHM.md for the full state machine diagram and
 *          control law derivation.
 ******************************************************************************
 */

#ifndef ACC_H
#define ACC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include <stdint.h>

/** System finite state machine states. */
typedef enum
{
    ACC_STATE_INIT = 0,
    ACC_STATE_CRUISE,
    ACC_STATE_SLOW_DOWN,
    ACC_STATE_BRAKE,
    ACC_STATE_EMERGENCY_STOP,
    ACC_STATE_RECOVER,
    ACC_STATE_FAULT
} ACC_State_t;

/**
 * @brief Initializes the ACC application core: binds the ADC handle used
 *        for the cruise-speed potentiometer, binds the UART handle used
 *        for telemetry, and resets the FSM to ACC_STATE_INIT.
 * @param hadc1  Pointer to the CubeMX-generated ADC1 handle.
 * @param huart2 Pointer to the CubeMX-generated USART2 handle.
 */
void ACC_Init(ADC_HandleTypeDef *hadc1, UART_HandleTypeDef *huart2);

/**
 * @brief Runs one iteration of the ACC control loop: reads the ultrasonic
 *        distance and potentiometer setpoint, evaluates the FSM, computes
 *        and applies the target motor duty cycle, updates status LEDs, and
 *        emits a UART telemetry frame. Intended to be called once per
 *        CONTROL_LOOP_PERIOD_MS from the main loop in main.c.
 */
void ACC_Update(void);

/**
 * @brief Returns the current FSM state, primarily for testing/telemetry.
 * @retval ACC_State_t Current state.
 */
ACC_State_t ACC_GetState(void);

/**
 * @brief Returns the last-read cruise speed setpoint as a percentage
 *        (0-100), derived from the potentiometer ADC reading.
 * @retval uint8_t Setpoint percentage.
 */
uint8_t ACC_GetSetpointPercent(void);

#ifdef __cplusplus
}
#endif

#endif /* ACC_H */
