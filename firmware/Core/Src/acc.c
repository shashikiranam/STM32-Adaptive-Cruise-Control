/**
 ******************************************************************************
 * @file    acc.c
 * @brief   Implementation of the Adaptive Cruise Control / Collision
 *          Avoidance finite state machine and proportional control law.
 *
 * @details This file is intentionally the only module that "knows about"
 *          the overall system behavior -- it composes the ultrasonic,
 *          motor, gpio, ADC, and UART drivers but contains no direct
 *          register access itself. See docs/ALGORITHM.md for the full FSM
 *          diagram, control law derivation, and tuning methodology.
 ******************************************************************************
 */

#include "acc.h"
#include "ultrasonic.h"
#include "motor.h"
#include "gpio.h"
#include "pwm.h"
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------------
 * Module-private state
 * ------------------------------------------------------------------------ */
static ADC_HandleTypeDef  *s_hadc1 = NULL;
static UART_HandleTypeDef *s_huart2 = NULL;

static ACC_State_t s_state = ACC_STATE_INIT;
static uint8_t     s_setpointPercent = 0U;
static uint32_t    s_lastDistanceMM = 0U;
static uint32_t    s_msSinceTrigger = 0U;
static uint32_t    s_recoverClearCycles = 0U;
static uint32_t    s_ledBlinkCycleCounter = 0U;
static uint8_t     s_recoverRampStep = 0U;

/* ------------------------------------------------------------------------
 * Forward declarations of internal helpers
 * ------------------------------------------------------------------------ */
static uint8_t      ACC_ReadSetpointPercent(void);
static uint8_t      ACC_ComputeSlowDownDuty(uint32_t distanceMM, uint8_t setpoint);
static uint8_t      ACC_ComputeBrakeDuty(uint32_t distanceMM, uint8_t setpoint);
static void          ACC_UpdateLEDsForState(ACC_State_t state);
static void          ACC_EnterState(ACC_State_t newState);
static void          ACC_SendTelemetry(US_Status_t usStatus);
static uint32_t       ACC_Clamp(uint32_t value, uint32_t min, uint32_t max);

void ACC_Init(ADC_HandleTypeDef *hadc1, UART_HandleTypeDef *huart2)
{
    s_hadc1 = hadc1;
    s_huart2 = huart2;

    s_state = ACC_STATE_INIT;
    s_setpointPercent = 0U;
    s_lastDistanceMM = 0U;
    s_msSinceTrigger = 0U;
    s_recoverClearCycles = 0U;
    s_ledBlinkCycleCounter = 0U;
    s_recoverRampStep = 0U;

    Ultrasonic_Init();
    Motor_Init();
    GPIO_AllLedsOff();
}

void ACC_Update(void)
{
    /* -------------------------------------------------------------------
     * 1. Kick off (or continue waiting on) the ultrasonic measurement for
     *    this control cycle.
     * ------------------------------------------------------------------- */
    Ultrasonic_TriggerMeasurement();

    /* The HC-SR04 echo response completes asynchronously via TIM2 input
     * capture interrupts. We poll once here; because the trigger was just
     * issued, in the common case the previous cycle's measurement (from
     * one control-loop period ago) is what completed, since interrupt
     * completion is effectively instantaneous relative to the 50ms period
     * for realistic HC-SR04 echo times (< 25ms at typical bench ranges).
     * We therefore poll Ultrasonic_Update() in a short bounded wait loop
     * bounded by the sensor's own timeout constant, which is well inside
     * the 50ms control cycle budget. */
    US_Status_t usStatus = US_STATUS_NOT_READY;
    uint32_t waitStartTick = HAL_GetTick();

    while (usStatus == US_STATUS_NOT_READY)
    {
        uint32_t elapsed = HAL_GetTick() - waitStartTick;
        usStatus = Ultrasonic_Update(elapsed);

        if (elapsed >= US_ECHO_TIMEOUT_MS)
        {
            /* Ultrasonic_Update() itself declares timeout at this bound
             * and will have returned US_STATUS_TIMEOUT already, but this
             * guards against ever spinning past the sensor's own timeout
             * budget under any circumstance. */
            break;
        }
    }

    if (usStatus == US_STATUS_OK)
    {
        s_lastDistanceMM = Ultrasonic_GetDistanceMM();
    }

    /* -------------------------------------------------------------------
     * 2. Read the cruise-speed setpoint from the potentiometer.
     * ------------------------------------------------------------------- */
    s_setpointPercent = ACC_ReadSetpointPercent();

    /* -------------------------------------------------------------------
     * 3. Evaluate the finite state machine.
     * ------------------------------------------------------------------- */
    switch (s_state)
    {
        case ACC_STATE_INIT:
        {
            /* Self-test: require at least one valid, in-range reading
             * before trusting the sensor enough to enter CRUISE. */
            if (usStatus == US_STATUS_OK)
            {
                ACC_EnterState(ACC_STATE_CRUISE);
            }
            /* Remain in INIT (motor stays off via Motor_Init()'s safe
             * default) until a valid reading is obtained. */
            break;
        }

        case ACC_STATE_CRUISE:
        {
            if ((usStatus == US_STATUS_TIMEOUT) || (usStatus == US_STATUS_OUT_OF_RANGE))
            {
                ACC_EnterState(ACC_STATE_FAULT);
                break;
            }

            if (s_lastDistanceMM <= ACC_WARN_DIST_MM)
            {
                ACC_EnterState(ACC_STATE_SLOW_DOWN);
                break;
            }

            Motor_SetSpeed(s_setpointPercent, MOTOR_DIR_FORWARD);
            break;
        }

        case ACC_STATE_SLOW_DOWN:
        {
            if ((usStatus == US_STATUS_TIMEOUT) || (usStatus == US_STATUS_OUT_OF_RANGE))
            {
                ACC_EnterState(ACC_STATE_FAULT);
                break;
            }

            if (s_lastDistanceMM <= ACC_BRAKE_DIST_MM)
            {
                ACC_EnterState(ACC_STATE_BRAKE);
                break;
            }

            if (s_lastDistanceMM > (ACC_WARN_DIST_MM + ACC_HYSTERESIS_MM))
            {
                ACC_EnterState(ACC_STATE_CRUISE);
                break;
            }

            uint8_t duty = ACC_ComputeSlowDownDuty(s_lastDistanceMM, s_setpointPercent);
            Motor_SetSpeed(duty, MOTOR_DIR_FORWARD);
            break;
        }

        case ACC_STATE_BRAKE:
        {
            if ((usStatus == US_STATUS_TIMEOUT) || (usStatus == US_STATUS_OUT_OF_RANGE))
            {
                ACC_EnterState(ACC_STATE_FAULT);
                break;
            }

            if (s_lastDistanceMM <= ACC_ESTOP_DIST_MM)
            {
                ACC_EnterState(ACC_STATE_EMERGENCY_STOP);
                break;
            }

            if (s_lastDistanceMM > (ACC_BRAKE_DIST_MM + ACC_HYSTERESIS_MM))
            {
                ACC_EnterState(ACC_STATE_SLOW_DOWN);
                break;
            }

            uint8_t duty = ACC_ComputeBrakeDuty(s_lastDistanceMM, s_setpointPercent);
            Motor_SetSpeed(duty, MOTOR_DIR_FORWARD);
            break;
        }

        case ACC_STATE_EMERGENCY_STOP:
        {
            /* Defense-in-depth: force the motor off every cycle while in
             * this state, independent of how we got here. */
            Motor_EmergencyOff();

            if ((usStatus == US_STATUS_TIMEOUT) || (usStatus == US_STATUS_OUT_OF_RANGE))
            {
                /* Sensor fault while stopped: stay stopped, but transition
                 * to FAULT so the operator/telemetry is aware the sensor
                 * itself needs attention. */
                ACC_EnterState(ACC_STATE_FAULT);
                break;
            }

            if (s_lastDistanceMM > (ACC_ESTOP_DIST_MM + ACC_HYSTERESIS_MM))
            {
                s_recoverClearCycles++;
                if (s_recoverClearCycles >= ACC_RECOVER_CYCLES)
                {
                    ACC_EnterState(ACC_STATE_RECOVER);
                }
            }
            else
            {
                s_recoverClearCycles = 0U; /* Reset debounce on any close reading. */
            }
            break;
        }

        case ACC_STATE_RECOVER:
        {
            if ((usStatus == US_STATUS_TIMEOUT) || (usStatus == US_STATUS_OUT_OF_RANGE))
            {
                ACC_EnterState(ACC_STATE_FAULT);
                break;
            }

            if (s_lastDistanceMM <= ACC_ESTOP_DIST_MM)
            {
                /* Obstacle re-approached during recovery -- go straight
                 * back to EMERGENCY_STOP rather than continuing to ramp. */
                ACC_EnterState(ACC_STATE_EMERGENCY_STOP);
                break;
            }

            /* Ramp duty up smoothly over ACC_RECOVER_RAMP_STEPS cycles
             * rather than jumping straight back to full setpoint duty,
             * avoiding an abrupt torque transient after a full stop. */
            s_recoverRampStep++;
            uint32_t rampDuty = ((uint32_t)s_setpointPercent * s_recoverRampStep) / ACC_RECOVER_RAMP_STEPS;
            rampDuty = ACC_Clamp(rampDuty, 0U, s_setpointPercent);
            Motor_SetSpeed((uint8_t)rampDuty, MOTOR_DIR_FORWARD);

            if (s_recoverRampStep >= ACC_RECOVER_RAMP_STEPS)
            {
                ACC_EnterState(ACC_STATE_CRUISE);
            }
            break;
        }

        case ACC_STATE_FAULT:
        {
            /* Defense-in-depth: force the motor off every cycle while
             * faulted. */
            Motor_EmergencyOff();

            if (usStatus == US_STATUS_OK)
            {
                /* Sensor has recovered -- re-run the INIT self-test path
                 * rather than jumping straight back into CRUISE, so a
                 * single good reading after a fault doesn't immediately
                 * re-arm the system without re-validation. */
                ACC_EnterState(ACC_STATE_INIT);
            }
            break;
        }

        default:
        {
            /* Unreachable in correct operation; fail safe if it ever is. */
            ACC_EnterState(ACC_STATE_FAULT);
            break;
        }
    }

    /* -------------------------------------------------------------------
     * 4. Update status LEDs (including blink-pattern cycle counter) and
     *    emit UART telemetry for this cycle.
     * ------------------------------------------------------------------- */
    s_ledBlinkCycleCounter++;
    ACC_UpdateLEDsForState(s_state);
    ACC_SendTelemetry(usStatus);
}

ACC_State_t ACC_GetState(void)
{
    return s_state;
}

uint8_t ACC_GetSetpointPercent(void)
{
    return s_setpointPercent;
}

/* ==========================================================================
 * Internal helpers
 * ========================================================================== */

static uint8_t ACC_ReadSetpointPercent(void)
{
    if (s_hadc1 == NULL)
    {
        return 0U;
    }

    uint32_t rawValue = 0U;

    if (HAL_ADC_Start(s_hadc1) == HAL_OK)
    {
        if (HAL_ADC_PollForConversion(s_hadc1, 5U) == HAL_OK)
        {
            rawValue = HAL_ADC_GetValue(s_hadc1);
        }
        HAL_ADC_Stop(s_hadc1);
    }

    /* Scale the 12-bit ADC reading (0-4095) to a 0-100% setpoint. */
    uint32_t percent = (rawValue * 100U) / ADC_MAX_RAW_VALUE;
    return (uint8_t)ACC_Clamp(percent, 0U, 100U);
}

static uint8_t ACC_ComputeSlowDownDuty(uint32_t distanceMM, uint8_t setpoint)
{
    /* Proportional control law -- see docs/ALGORITHM.md Section 3.
     * error_ratio = (distance - BRAKE) / (WARN - BRAKE), clamped [0,1]
     * target_duty = setpoint * error_ratio                              */
    if (distanceMM <= ACC_BRAKE_DIST_MM)
    {
        return 0U;
    }
    if (distanceMM >= ACC_WARN_DIST_MM)
    {
        return setpoint;
    }

    uint32_t numerator = distanceMM - ACC_BRAKE_DIST_MM;
    uint32_t denominator = ACC_WARN_DIST_MM - ACC_BRAKE_DIST_MM;

    /* Fixed-point scaling (x1000) to avoid pulling in floating point for
     * this ratio, keeping the FSM's hot path integer-only. */
    uint32_t errorRatioX1000 = (numerator * 1000U) / denominator;
    uint32_t duty = ((uint32_t)setpoint * errorRatioX1000) / 1000U;

    return (uint8_t)ACC_Clamp(duty, 0U, setpoint);
}

static uint8_t ACC_ComputeBrakeDuty(uint32_t distanceMM, uint8_t setpoint)
{
    /* Same proportional law, referenced to the BRAKE/ESTOP boundaries and
     * clamped to ACC_BRAKE_MAX_DUTY_PCT regardless of setpoint -- see
     * docs/ALGORITHM.md Section 3. */
    uint8_t effectiveMax = (setpoint < ACC_BRAKE_MAX_DUTY_PCT) ? setpoint : ACC_BRAKE_MAX_DUTY_PCT;

    if (distanceMM <= ACC_ESTOP_DIST_MM)
    {
        return 0U;
    }
    if (distanceMM >= ACC_BRAKE_DIST_MM)
    {
        return effectiveMax;
    }

    uint32_t numerator = distanceMM - ACC_ESTOP_DIST_MM;
    uint32_t denominator = ACC_BRAKE_DIST_MM - ACC_ESTOP_DIST_MM;

    uint32_t errorRatioX1000 = (numerator * 1000U) / denominator;
    uint32_t duty = ((uint32_t)effectiveMax * errorRatioX1000) / 1000U;

    return (uint8_t)ACC_Clamp(duty, 0U, effectiveMax);
}

static void ACC_UpdateLEDsForState(ACC_State_t state)
{
    switch (state)
    {
        case ACC_STATE_CRUISE:
            GPIO_AllLedsOff();
            GPIO_SetStatusLED(LED_GREEN, GPIO_PIN_SET);
            break;

        case ACC_STATE_SLOW_DOWN:
            GPIO_AllLedsOff();
            GPIO_SetStatusLED(LED_YELLOW, GPIO_PIN_SET);
            break;

        case ACC_STATE_BRAKE:
            GPIO_SetStatusLED(LED_GREEN, GPIO_PIN_RESET);
            GPIO_SetStatusLED(LED_YELLOW, GPIO_PIN_RESET);
            if ((s_ledBlinkCycleCounter % (2U * LED_BRAKE_BLINK_HALF_PERIOD_CYCLES)) < LED_BRAKE_BLINK_HALF_PERIOD_CYCLES)
            {
                GPIO_SetStatusLED(LED_RED, GPIO_PIN_SET);
            }
            else
            {
                GPIO_SetStatusLED(LED_RED, GPIO_PIN_RESET);
            }
            break;

        case ACC_STATE_EMERGENCY_STOP:
            GPIO_AllLedsOff();
            GPIO_SetStatusLED(LED_RED, GPIO_PIN_SET);
            break;

        case ACC_STATE_RECOVER:
            GPIO_AllLedsOff();
            GPIO_SetStatusLED(LED_YELLOW, GPIO_PIN_SET);
            break;

        case ACC_STATE_INIT:
            GPIO_AllLedsOff();
            break;

        case ACC_STATE_FAULT:
        default:
            /* All three LEDs blink together to signal a fault condition
             * distinct from any normal operating state's LED pattern. */
            if ((s_ledBlinkCycleCounter % (2U * LED_FAULT_BLINK_HALF_PERIOD_CYCLES)) < LED_FAULT_BLINK_HALF_PERIOD_CYCLES)
            {
                GPIO_SetStatusLED(LED_GREEN, GPIO_PIN_SET);
                GPIO_SetStatusLED(LED_YELLOW, GPIO_PIN_SET);
                GPIO_SetStatusLED(LED_RED, GPIO_PIN_SET);
            }
            else
            {
                GPIO_AllLedsOff();
            }
            break;
    }
}

static void ACC_EnterState(ACC_State_t newState)
{
    /* Reset per-state bookkeeping counters on every transition so stale
     * debounce/ramp state never leaks from one state entry to the next. */
    if (newState == ACC_STATE_EMERGENCY_STOP)
    {
        s_recoverClearCycles = 0U;
        Motor_EmergencyOff();
    }
    else if (newState == ACC_STATE_RECOVER)
    {
        s_recoverRampStep = 0U;
    }
    else if (newState == ACC_STATE_FAULT)
    {
        Motor_EmergencyOff();
    }

    s_state = newState;
}

static void ACC_SendTelemetry(US_Status_t usStatus)
{
    if (s_huart2 == NULL)
    {
        return;
    }

    static const char *stateNames[] =
    {
        "INIT", "CRUISE", "SLOW_DOWN", "BRAKE", "EMERGENCY_STOP", "RECOVER", "FAULT"
    };

    static const char *usStatusNames[] =
    {
        "OK", "TIMEOUT", "OUT_OF_RANGE", "NOT_READY"
    };

    const char *stateName = (s_state <= ACC_STATE_FAULT) ? stateNames[s_state] : "UNKNOWN";
    const char *usName = (usStatus <= US_STATUS_NOT_READY) ? usStatusNames[usStatus] : "UNKNOWN";

    char frame[128];
    int len = snprintf(frame, sizeof(frame),
                        "[ACC] state=%s dist_mm=%lu us=%s setpoint=%u%% duty=%u%% dir=%d\r\n",
                        stateName,
                        (unsigned long)s_lastDistanceMM,
                        usName,
                        s_setpointPercent,
                        PWM_GetCurrentDutyCycle(),
                        (int)Motor_GetCurrentDirection());

    if (len > 0)
    {
        HAL_UART_Transmit(s_huart2, (uint8_t *)frame, (uint16_t)len, DEBUG_UART_TX_TIMEOUT_MS);
    }
}

static uint32_t ACC_Clamp(uint32_t value, uint32_t min, uint32_t max)
{
    if (value < min) { return min; }
    if (value > max) { return max; }
    return value;
}
