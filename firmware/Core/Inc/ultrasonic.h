/**
 ******************************************************************************
 * @file    ultrasonic.h
 * @brief   HC-SR04 ultrasonic ranging driver. Issues trigger pulses, reads
 *          the timer-captured echo pulse width, converts it to a distance
 *          in millimeters, and reports timeout/implausible-reading faults.
 ******************************************************************************
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include <stdint.h>
#include <stdbool.h>

/** Result status for a distance measurement attempt. */
typedef enum
{
    US_STATUS_OK = 0,        /* Valid distance measured within range */
    US_STATUS_TIMEOUT,       /* No echo received within US_ECHO_TIMEOUT_MS */
    US_STATUS_OUT_OF_RANGE,  /* Echo received but implausible (too near/far) */
    US_STATUS_NOT_READY      /* Measurement still in progress, call again later */
} US_Status_t;

/**
 * @brief Initializes the ultrasonic driver. Must be called after
 *        Timer_InitEchoCapture()/Timer_StartEchoCapture() have configured
 *        and armed TIM2 for input capture.
 */
void Ultrasonic_Init(void);

/**
 * @brief Issues a new HC-SR04 trigger pulse, beginning a new measurement
 *        cycle. Should be called once per control loop iteration, before
 *        Ultrasonic_Update()/Ultrasonic_GetDistanceMM() are polled.
 * @note  Non-blocking beyond the fixed ~10us trigger pulse itself (see
 *        GPIO_IssueUltrasonicTrigger()). The actual echo measurement
 *        happens asynchronously via TIM2 input-capture interrupts.
 */
void Ultrasonic_TriggerMeasurement(void);

/**
 * @brief Services the ultrasonic driver's internal timeout logic. Must be
 *        called periodically (e.g. once per control loop iteration, after
 *        Ultrasonic_TriggerMeasurement()) so a missing echo is detected
 *        and reported rather than leaving the driver waiting forever.
 * @param elapsedMsSinceTrigger Milliseconds elapsed since the last call to
 *                               Ultrasonic_TriggerMeasurement(). Typically
 *                               the caller passes CONTROL_LOOP_PERIOD_MS or
 *                               tracks it via HAL_GetTick().
 * @retval US_Status_t Current measurement status.
 */
US_Status_t Ultrasonic_Update(uint32_t elapsedMsSinceTrigger);

/**
 * @brief Returns the most recently computed valid distance, in millimeters.
 *        Only meaningful when the last call to Ultrasonic_Update() returned
 *        US_STATUS_OK -- callers must check status before trusting this
 *        value, per the project's fail-safe design principle.
 * @retval uint32_t Distance in millimeters.
 */
uint32_t Ultrasonic_GetDistanceMM(void);

#ifdef __cplusplus
}
#endif

#endif /* ULTRASONIC_H */
