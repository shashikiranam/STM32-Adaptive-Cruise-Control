/**
 ******************************************************************************
 * @file    pwm.c
 * @brief   Implementation of the TIM3-based PWM driver used for motor speed
 *          control via the L298N's ENA input.
 ******************************************************************************
 */

#include "pwm.h"

static TIM_HandleTypeDef *s_htim3 = NULL;
static uint8_t             s_currentDutyPercent = 0U;

HAL_StatusTypeDef PWM_Init(TIM_HandleTypeDef *htim3)
{
    if (htim3 == NULL)
    {
        return HAL_ERROR;
    }

    s_htim3 = htim3;
    s_currentDutyPercent = 0U;

    /* Ensure the motor starts at 0% duty before PWM output is enabled, so
     * there is never a brief undefined/garbage duty cycle applied to the
     * motor at power-up. */
    PWM_SetDutyCycle(0U);

    return HAL_TIM_PWM_Start(s_htim3, PWM_TIM_CHANNEL);
}

void PWM_SetDutyCycle(uint8_t dutyPercent)
{
    if (s_htim3 == NULL)
    {
        return; /* Driver not initialized -- fail safe by doing nothing
                    rather than dereferencing a NULL handle. */
    }

    /* Clamp rather than reject: this function sits on the actuation path,
     * and a caller passing an out-of-range value (e.g. 150) almost always
     * means "as much as possible", which clamping to 100 satisfies safely,
     * while an accidental negative-cast-to-uint8_t wrap is clamped to 0. */
    uint8_t clamped = dutyPercent;
    if (clamped > PWM_DUTY_MAX_PCT)
    {
        clamped = PWM_DUTY_MAX_PCT;
    }

    s_currentDutyPercent = clamped;

    /* Convert the 0-100% duty cycle into a timer compare value based on the
     * auto-reload register value configured by MX_TIM3_Init() for
     * PWM_FREQUENCY_HZ. Using the live ARR value (rather than a hard-coded
     * period) keeps this driver correct even if the PWM frequency is
     * retuned in CubeMX without touching this file. */
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(s_htim3);
    uint32_t compareValue = ((uint32_t)clamped * (arr + 1U)) / 100U;

    __HAL_TIM_SET_COMPARE(s_htim3, PWM_TIM_CHANNEL, compareValue);
}

uint8_t PWM_GetCurrentDutyCycle(void)
{
    return s_currentDutyPercent;
}
