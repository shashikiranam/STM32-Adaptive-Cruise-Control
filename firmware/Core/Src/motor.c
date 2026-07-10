/**
 ******************************************************************************
 * @file    motor.c
 * @brief   Implementation of the motor driver composing pwm.c and gpio.c to
 *          drive the L298N H-bridge.
 ******************************************************************************
 */

#include "motor.h"
#include "pwm.h"
#include "gpio.h"

static MotorDirection_t s_currentDirection = MOTOR_DIR_COAST;

/**
 * @brief Applies the IN1/IN2 GPIO truth table for a given logical direction.
 *        Internal helper -- not exposed in motor.h.
 */
static void Motor_ApplyDirectionPins(MotorDirection_t direction)
{
    switch (direction)
    {
        case MOTOR_DIR_FORWARD:
            GPIO_SetMotorDirectionPins(GPIO_PIN_SET, GPIO_PIN_RESET);
            break;

        case MOTOR_DIR_REVERSE:
            GPIO_SetMotorDirectionPins(GPIO_PIN_RESET, GPIO_PIN_SET);
            break;

        case MOTOR_DIR_BRAKE:
            GPIO_SetMotorDirectionPins(GPIO_PIN_SET, GPIO_PIN_SET);
            break;

        case MOTOR_DIR_COAST:
        default:
            GPIO_SetMotorDirectionPins(GPIO_PIN_RESET, GPIO_PIN_RESET);
            break;
    }

    s_currentDirection = direction;
}

void Motor_Init(void)
{
    /* Start in a known-safe coasted, zero-duty state. */
    PWM_SetDutyCycle(0U);
    Motor_ApplyDirectionPins(MOTOR_DIR_COAST);
}

void Motor_SetSpeed(uint8_t dutyPercent, MotorDirection_t direction)
{
    /* Direction is applied before duty cycle so the H-bridge is never
     * commanded to drive current through OUT1/OUT2 in an undefined
     * direction state, even momentarily. */
    Motor_ApplyDirectionPins(direction);
    PWM_SetDutyCycle(dutyPercent);
}

void Motor_EmergencyOff(void)
{
    /* Zero the PWM duty FIRST, then apply the active-brake direction state.
     * Ordering matters: cutting drive current before latching the brake
     * truth table avoids a transient full-duty pulse through the brake
     * configuration during the switchover. */
    PWM_SetDutyCycle(0U);
    Motor_ApplyDirectionPins(MOTOR_DIR_BRAKE);
}

void Motor_Coast(void)
{
    PWM_SetDutyCycle(0U);
    Motor_ApplyDirectionPins(MOTOR_DIR_COAST);
}

MotorDirection_t Motor_GetCurrentDirection(void)
{
    return s_currentDirection;
}
