/**
 ******************************************************************************
 * @file    system_config.h
 * @brief   Central configuration header for the STM32 Adaptive Cruise Control
 *          and Collision Avoidance System.
 *
 * @details This header is the SINGLE SOURCE OF TRUTH for every pin mapping,
 *          timing constant, distance threshold, and tunable parameter used
 *          throughout the firmware. No other module should hard-code a pin
 *          name, GPIO port, or magic threshold value -- everything is defined
 *          here so the system can be retuned or re-targeted to a different
 *          board by editing this single file.
 *
 *          Cross-reference: hardware/PIN_CONNECTIONS.md documents the same
 *          mapping in human-readable table form.
 *
 * Target:  STM32G071RBT6 (NUCLEO-G071RB)
 ******************************************************************************
 */

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"

/* =========================================================================
 * GENERAL SYSTEM TIMING
 * ========================================================================= */

/** Main control loop period, in milliseconds. All FSM/control-law evaluation
 *  happens once per this period. Must be >= HC-SR04 minimum re-trigger
 *  interval (~60ms recommended by most datasheets; 50ms is used here with
 *  the ultrasonic driver's own timeout logic providing the safety margin). */
#define CONTROL_LOOP_PERIOD_MS          (50U)

/* =========================================================================
 * ULTRASONIC SENSOR (HC-SR04) CONFIGURATION
 * ========================================================================= */

/** Trigger pin: GPIO output, 10us pulse issued once per control cycle. */
#define US_TRIG_GPIO_PORT               GPIOA
#define US_TRIG_GPIO_PIN                GPIO_PIN_1

/** Echo pin: TIM2 Channel 1 input capture (AF2 on PA0 for STM32G071). */
#define US_ECHO_GPIO_PORT               GPIOA
#define US_ECHO_GPIO_PIN                GPIO_PIN_0
#define US_ECHO_TIM_CHANNEL             TIM_CHANNEL_1

/** Trigger pulse width in microseconds (HC-SR04 datasheet minimum: 10us). */
#define US_TRIG_PULSE_WIDTH_US           (10U)

/** Maximum time to wait for an echo before declaring a timeout fault, in ms.
 *  Corresponds to a max range well beyond the HC-SR04's rated 400cm. */
#define US_ECHO_TIMEOUT_MS               (30U)

/** Speed of sound approximation at ~20C, in mm per microsecond, already
 *  divided by 2 to account for the round-trip time-of-flight measurement.
 *  distance_mm = pulse_width_us * US_SOUND_SPEED_MM_PER_US */
#define US_SOUND_SPEED_MM_PER_US         (0.343f)

/** Valid distance range. Readings outside this band are treated as
 *  implausible/faulty rather than trusted. */
#define US_MIN_VALID_MM                  (20U)
#define US_MAX_VALID_MM                  (4500U)

/* =========================================================================
 * MOTOR / PWM CONFIGURATION (L298N via TIM3 CH1)
 * ========================================================================= */

#define PWM_TIM_CHANNEL                  TIM_CHANNEL_1
#define PWM_FREQUENCY_HZ                  (1000U)   /* 1 kHz PWM, silent to human ear range edge, fine for small brushed DC motors */
#define PWM_DUTY_MIN_PCT                  (0U)
#define PWM_DUTY_MAX_PCT                  (100U)

/** L298N direction control pins. */
#define MOTOR_IN1_GPIO_PORT              GPIOB
#define MOTOR_IN1_GPIO_PIN               GPIO_PIN_0
#define MOTOR_IN2_GPIO_PORT              GPIOB
#define MOTOR_IN2_GPIO_PIN               GPIO_PIN_1

/* =========================================================================
 * STATUS LED CONFIGURATION
 * ========================================================================= */

#define LED_GREEN_GPIO_PORT              GPIOB
#define LED_GREEN_GPIO_PIN               GPIO_PIN_3   /* CRUISE indicator */

#define LED_YELLOW_GPIO_PORT             GPIOB
#define LED_YELLOW_GPIO_PIN              GPIO_PIN_4   /* SLOW_DOWN indicator */

#define LED_RED_GPIO_PORT                GPIOB
#define LED_RED_GPIO_PIN                 GPIO_PIN_5   /* BRAKE / EMERGENCY_STOP indicator */

/** Blink periods for LED patterns, in control-loop cycles (not ms), so they
 *  scale automatically if CONTROL_LOOP_PERIOD_MS changes. */
#define LED_BRAKE_BLINK_HALF_PERIOD_CYCLES     (5U)   /* ~2 Hz at 50ms loop */
#define LED_FAULT_BLINK_HALF_PERIOD_CYCLES     (3U)   /* ~4 Hz at 50ms loop */

/* =========================================================================
 * ADC / POTENTIOMETER CONFIGURATION
 * ========================================================================= */

#define ADC_POT_CHANNEL                  ADC_CHANNEL_4   /* PA4 = ADC1_IN4 */
#define ADC_RESOLUTION_BITS              (12U)
#define ADC_MAX_RAW_VALUE                ((1U << ADC_RESOLUTION_BITS) - 1U)  /* 4095 */

/* =========================================================================
 * UART DEBUG CONFIGURATION
 * ========================================================================= */

#define DEBUG_UART_BAUDRATE               (115200U)
#define DEBUG_UART_TX_TIMEOUT_MS           (10U)

/* =========================================================================
 * ADAPTIVE CRUISE CONTROL / COLLISION AVOIDANCE THRESHOLDS
 * ========================================================================= */

/** Distance zone boundaries, in millimeters. See docs/ALGORITHM.md for the
 *  full derivation and tuning methodology. */
#define ACC_WARN_DIST_MM                  (400U)   /* Enter SLOW_DOWN below this */
#define ACC_BRAKE_DIST_MM                 (250U)   /* Enter BRAKE below this */
#define ACC_ESTOP_DIST_MM                 (120U)   /* Enter EMERGENCY_STOP below this */

/** Hysteresis band added on "retreat" transitions to prevent state chatter
 *  near a boundary. */
#define ACC_HYSTERESIS_MM                 (30U)

/** Consecutive clear control-cycles required before leaving EMERGENCY_STOP.
 *  At CONTROL_LOOP_PERIOD_MS = 50, 10 cycles = 500 ms debounce. */
#define ACC_RECOVER_CYCLES                (10U)

/** Hard ceiling on motor duty cycle while in the BRAKE zone, regardless of
 *  what the proportional control law would otherwise allow. */
#define ACC_BRAKE_MAX_DUTY_PCT            (20U)

/** Number of ramp steps used when transitioning RECOVER -> CRUISE, to avoid
 *  a hard jump in commanded duty cycle after a full stop. */
#define ACC_RECOVER_RAMP_STEPS            (5U)

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_CONFIG_H */
