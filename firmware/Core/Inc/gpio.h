/**
 ******************************************************************************
 * @file    gpio.h
 * @brief   GPIO driver: status LEDs, ultrasonic trigger pin, and motor
 *          direction pins (IN1/IN2 on the L298N).
 ******************************************************************************
 */

#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include <stdint.h>

/** Logical LED identifiers, decoupled from the underlying GPIO pin so the
 *  application layer never references a raw port/pin directly. */
typedef enum
{
    LED_GREEN = 0,
    LED_YELLOW,
    LED_RED,
    LED_COUNT
} LedId_t;

/**
 * @brief Performs one-time application-level GPIO setup beyond what the
 *        CubeMX-generated MX_GPIO_Init() already configures (pin modes,
 *        pull resistors, initial output states are set by MX_GPIO_Init();
 *        this function ensures all outputs start in a known-safe state).
 */
void GPIO_AppInit(void);

/**
 * @brief Sets a status LED on or off.
 * @param led    Logical LED identifier.
 * @param state  GPIO_PIN_SET to turn on, GPIO_PIN_RESET to turn off.
 */
void GPIO_SetStatusLED(LedId_t led, GPIO_PinState state);

/**
 * @brief Turns all status LEDs off. Used during state transitions before
 *        the new state's LED pattern is applied, and during fault entry.
 */
void GPIO_AllLedsOff(void);

/**
 * @brief Toggles a status LED. Used to implement blink patterns (BRAKE,
 *        FAULT) driven from the main control loop's cycle counter.
 * @param led Logical LED identifier.
 */
void GPIO_ToggleStatusLED(LedId_t led);

/**
 * @brief Drives the HC-SR04 trigger pin through a 10us HIGH pulse, per the
 *        HC-SR04 datasheet's minimum trigger pulse width
 *        (US_TRIG_PULSE_WIDTH_US in system_config.h).
 * @note  This function uses a short blocking delay (10us), which is
 *        acceptable because it is issued once per 50ms control cycle and
 *        is far shorter than the cycle budget.
 */
void GPIO_IssueUltrasonicTrigger(void);

/**
 * @brief Sets the L298N IN1/IN2 direction pins directly. Used only by
 *        motor.c -- application code should call Motor_SetSpeed() instead
 *        of this function directly.
 * @param in1 GPIO_PIN_SET or GPIO_PIN_RESET for the IN1 line.
 * @param in2 GPIO_PIN_SET or GPIO_PIN_RESET for the IN2 line.
 */
void GPIO_SetMotorDirectionPins(GPIO_PinState in1, GPIO_PinState in2);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */
