/**
 ******************************************************************************
 * @file    ultrasonic.c
 * @brief   Implementation of the HC-SR04 ultrasonic ranging driver.
 *
 * @details Measurement sequence per control cycle:
 *            1. Ultrasonic_TriggerMeasurement() issues the 10us trigger
 *               pulse via gpio.c and arms the TIM2 input-capture state
 *               machine in timer.c.
 *            2. The HC-SR04 emits its ultrasonic burst and drives Echo high
 *               for the round-trip time-of-flight. timer.c's ISR captures
 *               the rising and falling edges asynchronously.
 *            3. Ultrasonic_Update() is polled once per control cycle: if a
 *               new pulse-width measurement is available, it is converted
 *               to millimeters and range-checked; if too much time has
 *               elapsed with no measurement, a timeout is declared and the
 *               timer capture state machine is reset via
 *               Timer_AbortEchoCapture() so the next trigger starts clean.
 ******************************************************************************
 */

#include "ultrasonic.h"
#include "timer.h"
#include "gpio.h"

static uint32_t     s_lastValidDistanceMM = 0U;
static bool          s_measurementInProgress = false;

void Ultrasonic_Init(void)
{
    s_lastValidDistanceMM = 0U;
    s_measurementInProgress = false;
}

void Ultrasonic_TriggerMeasurement(void)
{
    /* Defensive: if a previous measurement never completed (shouldn't
     * happen given the timeout logic in Ultrasonic_Update(), but this
     * guards against re-triggering on top of an in-flight capture and
     * corrupting the timer state machine), abort it first. */
    if (s_measurementInProgress)
    {
        Timer_AbortEchoCapture();
    }

    s_measurementInProgress = true;
    GPIO_IssueUltrasonicTrigger();
}

US_Status_t Ultrasonic_Update(uint32_t elapsedMsSinceTrigger)
{
    if (!s_measurementInProgress)
    {
        return US_STATUS_NOT_READY;
    }

    if (Timer_IsNewPulseWidthAvailable())
    {
        uint32_t pulseWidthUS = Timer_GetLastPulseWidthUS();
        s_measurementInProgress = false;

        /* Convert time-of-flight to distance. See docs/ALGORITHM.md Section 5
         * for the speed-of-sound derivation and the round-trip divide-by-two
         * already folded into US_SOUND_SPEED_MM_PER_US. */
        uint32_t distanceMM = (uint32_t)((float)pulseWidthUS * US_SOUND_SPEED_MM_PER_US);

        if ((distanceMM < US_MIN_VALID_MM) || (distanceMM > US_MAX_VALID_MM))
        {
            /* Implausible reading -- do NOT update s_lastValidDistanceMM.
             * Fail-safe principle: stale-but-valid is preferable to
             * accepting garbage, and the caller (acc.c) treats
             * US_STATUS_OUT_OF_RANGE as a fault condition regardless. */
            return US_STATUS_OUT_OF_RANGE;
        }

        s_lastValidDistanceMM = distanceMM;
        return US_STATUS_OK;
    }

    if (elapsedMsSinceTrigger >= US_ECHO_TIMEOUT_MS)
    {
        /* No echo received in time -- sensor disconnected, no target in
         * range, or a wiring fault. Reset the timer capture state machine
         * so the next trigger cycle starts from a known-clean state. */
        Timer_AbortEchoCapture();
        s_measurementInProgress = false;
        return US_STATUS_TIMEOUT;
    }

    return US_STATUS_NOT_READY;
}

uint32_t Ultrasonic_GetDistanceMM(void)
{
    return s_lastValidDistanceMM;
}
