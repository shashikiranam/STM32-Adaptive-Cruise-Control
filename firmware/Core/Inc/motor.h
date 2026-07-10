/**
 ******************************************************************************
 * @file    motor.h
 * @brief   Motor driver: translates a desired speed/direction command into
 *          coordinated PWM (speed) and GPIO (direction) actuation of the
 *          L298N H-bridge. Also owns the fail-safe "emergency off" primitive
 *          used by the ACC/FSM safety logic.
 ******************************************************************************
 */

#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include <stdint.h>

/** Logical motor direction, decoupled from the L298N's raw IN1/IN2 truth
 *  table so the application layer never has to reason about H-bridge logic
 *  levels directly. */
typedef enum
{
    MOTOR_DIR_FORWARD = 0,   /* IN1=1, IN2=0 */
    MOTOR_DIR_REVERSE,       /* IN1=0, IN2=1 */
    MOTOR_DIR_BRAKE,         /* IN1=1, IN2=1 -- active brake (both high) */
    MOTOR_DIR_COAST          /* IN1=0, IN2=0 -- coast (free-wheel) */
} MotorDirection_t;

/**
 * @brief Initializes the motor driver. Must be called after PWM_Init() and
 *        GPIO_AppInit(), since this module composes both.
 */
void Motor_Init(void);

/**
 * @brief Commands a motor speed and direction.
 * @param dutyPercent Desired PWM duty cycle, 0-100 (clamped internally by
 *                     the underlying PWM driver).
 * @param direction    Desired direction (MOTOR_DIR_FORWARD / _REVERSE).
 *                      Passing MOTOR_DIR_BRAKE or MOTOR_DIR_COAST here also
 *                      works but Motor_EmergencyOff()/Motor_Coast() are the
 *                      preferred explicit APIs for those cases.
 */
void Motor_SetSpeed(uint8_t dutyPercent, MotorDirection_t direction);

/**
 * @brief Immediately forces the motor to a safe stopped state: 0% PWM duty
 *        and the L298N direction pins set to active-brake (both high).
 *        This is the primary safety primitive called on EMERGENCY_STOP and
 *        FAULT entry -- it is independent of the normal FSM duty-cycle
 *        computation so it can never be skipped by a logic bug elsewhere.
 */
void Motor_EmergencyOff(void);

/**
 * @brief Sets the motor to coast (free-wheel, both direction pins low,
 *        0% PWM duty). Distinct from Motor_EmergencyOff() which actively
 *        brakes; coast is used for benign state transitions where an
 *        active brake is not required.
 */
void Motor_Coast(void);

/**
 * @brief Returns the last commanded direction, for UART telemetry reporting.
 * @retval MotorDirection_t Current direction state.
 */
MotorDirection_t Motor_GetCurrentDirection(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H */
