/**
 ******************************************************************************
 * @file    main.h
 * @brief   Top-level application header. Declares the peripheral handles
 *          shared between main.c and the HAL MSP/IRQ callback files, and
 *          the CubeMX-style MX_*_Init() peripheral initialization functions
 *          implemented in main.c.
 ******************************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"
#include "system_config.h"

/* ------------------------------------------------------------------------
 * Peripheral handles (defined in main.c, referenced by driver modules and
 * by stm32g0xx_it.c for interrupt callbacks).
 * ------------------------------------------------------------------------ */
extern ADC_HandleTypeDef  hadc1;
extern TIM_HandleTypeDef  htim2;   /* Ultrasonic echo input capture */
extern TIM_HandleTypeDef  htim3;   /* Motor PWM */
extern UART_HandleTypeDef huart2;  /* Debug telemetry, routed to ST-LINK VCP */

/* ------------------------------------------------------------------------
 * Public functions implemented in main.c. The individual MX_*_Init()
 * peripheral-init functions (MX_GPIO_Init, MX_TIM2_Init, MX_TIM3_Init,
 * MX_ADC1_Init, MX_USART2_UART_Init) are CubeMX-style "generated glue" and
 * are declared `static` inside main.c itself (not exposed here), matching
 * standard CubeMX project structure where they are only ever called from
 * main() in the same translation unit.
 * ------------------------------------------------------------------------ */
void SystemClock_Config(void);
void Error_Handler(void);

/* ------------------------------------------------------------------------
 * Board-level pin aliases (NUCLEO-G071RB user LED / user button), used only
 * for basic board bring-up sanity checks -- NOT part of the ACC signal
 * path, which uses the pins defined in system_config.h.
 * ------------------------------------------------------------------------ */
#define NUCLEO_USER_LED_PORT   GPIOA
#define NUCLEO_USER_LED_PIN    GPIO_PIN_5
#define NUCLEO_USER_BTN_PORT   GPIOC
#define NUCLEO_USER_BTN_PIN    GPIO_PIN_13

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
