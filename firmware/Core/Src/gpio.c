/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   Implementation of the GPIO driver for status LEDs, ultrasonic
 *          trigger, and motor direction pins.
 ******************************************************************************
 */

#include "gpio.h"

/* ------------------------------------------------------------------------
 * Internal lookup table mapping logical LED IDs to physical port/pin,
 * keeping every other function in this file free of raw pin literals.
 * ------------------------------------------------------------------------ */
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t      pin;
} LedPinMap_t;

static const LedPinMap_t s_ledMap[LED_COUNT] =
{
    [LED_GREEN]  = { LED_GREEN_GPIO_PORT,  LED_GREEN_GPIO_PIN  },
    [LED_YELLOW] = { LED_YELLOW_GPIO_PORT, LED_YELLOW_GPIO_PIN },
    [LED_RED]    = { LED_RED_GPIO_PORT,    LED_RED_GPIO_PIN    },
};

void GPIO_AppInit(void)
{
    /* Ensure a known-safe startup state: all LEDs off, motor direction pins
     * both low (coast / no drive), trigger pin low. Pin modes themselves are
     * configured by the CubeMX-generated MX_GPIO_Init() in main.c -- this
     * function only enforces the safe logical output state on top of that. */
    GPIO_AllLedsOff();
    GPIO_SetMotorDirectionPins(GPIO_PIN_RESET, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(US_TRIG_GPIO_PORT, US_TRIG_GPIO_PIN, GPIO_PIN_RESET);
}

void GPIO_SetStatusLED(LedId_t led, GPIO_PinState state)
{
    if (led >= LED_COUNT)
    {
        return; /* Defensive bounds check -- invalid logical ID, no-op. */
    }

    HAL_GPIO_WritePin(s_ledMap[led].port, s_ledMap[led].pin, state);
}

void GPIO_AllLedsOff(void)
{
    for (uint32_t i = 0; i < (uint32_t)LED_COUNT; i++)
    {
        HAL_GPIO_WritePin(s_ledMap[i].port, s_ledMap[i].pin, GPIO_PIN_RESET);
    }
}

void GPIO_ToggleStatusLED(LedId_t led)
{
    if (led >= LED_COUNT)
    {
        return;
    }

    HAL_GPIO_TogglePin(s_ledMap[led].port, s_ledMap[led].pin);
}

void GPIO_IssueUltrasonicTrigger(void)
{
    HAL_GPIO_WritePin(US_TRIG_GPIO_PORT, US_TRIG_GPIO_PIN, GPIO_PIN_SET);

    /* Busy-wait for the datasheet-minimum 10us trigger pulse width. This is
     * a deliberate, bounded, extremely short blocking delay -- acceptable
     * because it executes once per 50ms control cycle (0.02% duty impact)
     * and DWT-based microsecond delay avoids pulling in HAL_Delay's 1ms
     * SysTick granularity, which would be far too coarse for a 10us pulse. */
    volatile uint32_t cycles = (SystemCoreClock / 1000000U) * US_TRIG_PULSE_WIDTH_US;
    for (volatile uint32_t i = 0; i < cycles; i++)
    {
        __NOP();
    }

    HAL_GPIO_WritePin(US_TRIG_GPIO_PORT, US_TRIG_GPIO_PIN, GPIO_PIN_RESET);
}

void GPIO_SetMotorDirectionPins(GPIO_PinState in1, GPIO_PinState in2)
{
    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_PORT, MOTOR_IN1_GPIO_PIN, in1);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_PORT, MOTOR_IN2_GPIO_PIN, in2);
}
