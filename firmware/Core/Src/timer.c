/**
 ******************************************************************************
 * @file    timer.c
 * @brief   Implementation of the TIM2 input-capture driven echo pulse-width
 *          measurement used by ultrasonic.c.
 *
 * @details TIM2 CH1 is configured (via CubeMX / MX_TIM2_Init in main.c) with
 *          a prescaler that makes the counter increment once per microsecond.
 *          Input capture is configured to trigger on BOTH edges is NOT used
 *          here (STM32G0 TIM2 CH1 supports either-edge capture on some
 *          configurations, but for clarity and portability this driver uses
 *          a software-toggled edge polarity: it captures the rising edge,
 *          flips polarity to falling, captures the falling edge, computes
 *          the delta, then flips back to rising for the next measurement).
 *
 *          This avoids relying on both-edge hardware capture (not uniformly
 *          available across all timer channels) and keeps the driver
 *          portable across STM32 families with plain HAL calls only.
 ******************************************************************************
 */

#include "timer.h"

/* ------------------------------------------------------------------------
 * Internal state machine for the rising/falling edge capture sequence.
 * ------------------------------------------------------------------------ */
typedef enum
{
    ECHO_CAP_STATE_IDLE = 0,      /* Waiting for trigger to be issued */
    ECHO_CAP_STATE_WAIT_RISING,   /* Armed, waiting for the rising edge capture */
    ECHO_CAP_STATE_WAIT_FALLING   /* Rising edge captured, waiting for falling edge */
} EchoCapState_t;

static volatile EchoCapState_t s_capState = ECHO_CAP_STATE_IDLE;
static volatile uint32_t       s_risingTimestamp = 0U;
static volatile uint32_t       s_lastPulseWidthUS = 0U;
static volatile bool           s_newPulseAvailable = false;

HAL_StatusTypeDef Timer_InitEchoCapture(TIM_HandleTypeDef *htim2)
{
    if (htim2 == NULL)
    {
        return HAL_ERROR;
    }

    /* Timer base configuration (prescaler/period) is performed by the
     * CubeMX-generated MX_TIM2_Init() in main.c, sized so that the counter
     * increments at 1 MHz (1 tick = 1 microsecond) based on the current
     * APB timer clock. This function only resets the driver's internal
     * capture state machine to a known-good starting point. */
    s_capState = ECHO_CAP_STATE_IDLE;
    s_risingTimestamp = 0U;
    s_lastPulseWidthUS = 0U;
    s_newPulseAvailable = false;

    /* Ensure capture polarity starts on rising edge. */
    if (__HAL_TIM_SET_CAPTUREPOLARITY(htim2, US_ECHO_TIM_CHANNEL, TIM_INPUTCHANNELPOLARITY_RISING) != HAL_OK)
    {
        /* __HAL_TIM_SET_CAPTUREPOLARITY is a macro, not a function returning
         * HAL_StatusTypeDef in all HAL versions; guard compiles safely if it
         * expands to a void macro on this HAL revision. */
    }

    return HAL_OK;
}

HAL_StatusTypeDef Timer_StartEchoCapture(TIM_HandleTypeDef *htim2)
{
    if (htim2 == NULL)
    {
        return HAL_ERROR;
    }

    s_capState = ECHO_CAP_STATE_WAIT_RISING;

    return HAL_TIM_IC_Start_IT(htim2, US_ECHO_TIM_CHANNEL);
}

void Timer_EchoCaptureCallback(TIM_HandleTypeDef *htim2)
{
    if (htim2->Channel != HAL_TIM_ACTIVE_CHANNEL_1)
    {
        /* Not our channel -- ignore (defensive, in case other TIM2 channels
         * are ever used for something else in a future extension). */
        return;
    }

    uint32_t capturedValue = HAL_TIM_ReadCapturedValue(htim2, US_ECHO_TIM_CHANNEL);

    switch (s_capState)
    {
        case ECHO_CAP_STATE_WAIT_RISING:
        {
            /* Rising edge captured: record timestamp and switch polarity to
             * capture the falling edge next. */
            s_risingTimestamp = capturedValue;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim2, US_ECHO_TIM_CHANNEL, TIM_INPUTCHANNELPOLARITY_FALLING);
            s_capState = ECHO_CAP_STATE_WAIT_FALLING;
            break;
        }

        case ECHO_CAP_STATE_WAIT_FALLING:
        {
            /* Falling edge captured: compute pulse width, accounting for
             * the timer counter's possible wraparound (16-bit or 32-bit
             * depending on TIM2 instance width on the target part). */
            uint32_t fallingTimestamp = capturedValue;
            uint32_t pulseWidth;

            if (fallingTimestamp >= s_risingTimestamp)
            {
                pulseWidth = fallingTimestamp - s_risingTimestamp;
            }
            else
            {
                /* Counter wrapped around during the measurement window. */
                pulseWidth = (0xFFFFFFFFU - s_risingTimestamp) + fallingTimestamp + 1U;
            }

            s_lastPulseWidthUS = pulseWidth;
            s_newPulseAvailable = true;

            /* Return polarity to rising for the next measurement cycle and
             * go idle until the next trigger is issued by ultrasonic.c. */
            __HAL_TIM_SET_CAPTUREPOLARITY(htim2, US_ECHO_TIM_CHANNEL, TIM_INPUTCHANNELPOLARITY_RISING);
            s_capState = ECHO_CAP_STATE_IDLE;
            break;
        }

        case ECHO_CAP_STATE_IDLE:
        default:
        {
            /* Spurious capture event with no active measurement in
             * progress -- ignore. This can legitimately happen from
             * electrical noise on the echo line; the ultrasonic driver's
             * timeout logic is the authoritative safety net, not this ISR. */
            break;
        }
    }
}

bool Timer_IsNewPulseWidthAvailable(void)
{
    return s_newPulseAvailable;
}

uint32_t Timer_GetLastPulseWidthUS(void)
{
    s_newPulseAvailable = false;
    return s_lastPulseWidthUS;
}

void Timer_AbortEchoCapture(void)
{
    s_capState = ECHO_CAP_STATE_IDLE;
    s_newPulseAvailable = false;
}
