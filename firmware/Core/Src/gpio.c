/******************************************************************************
 * File        : gpio.c
 * Description : GPIO Configuration
 * Target      : STM32G071RB
 ******************************************************************************/

#include "gpio.h"

/*---------------------------------------------------------------------------
 * GPIO Initialization
 *---------------------------------------------------------------------------*/
void GPIO_Init_Custom(void)
{
    /*
     * Reference GPIO Configuration
     *
     * Ultrasonic Sensor
     * -----------------
     * Trigger  -> PA0 (Output)
     * Echo     -> PA1 (Input)
     *
     * Motor Driver (L298N)
     * --------------------
     * IN1 -> PB0
     * IN2 -> PB1
     * ENA -> PA8 (PWM)
     *
     * Status LED
     * ----------
     * LD2 -> PA5
     *
     * Actual GPIO initialization is normally generated
     * by STM32CubeMX inside MX_GPIO_Init().
     */
}

/*---------------------------------------------------------------------------
 * Trigger Pin HIGH
 *---------------------------------------------------------------------------*/
void Trigger_High(void)
{
    /* HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); */
}

/*---------------------------------------------------------------------------
 * Trigger Pin LOW
 *---------------------------------------------------------------------------*/
void Trigger_Low(void)
{
    /* HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); */
}

/*---------------------------------------------------------------------------
 * Read Echo Pin
 *---------------------------------------------------------------------------*/
uint8_t Echo_Read(void)
{
    /*
    return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    */

    return 0;
}

/*---------------------------------------------------------------------------
 * Motor Direction
 *---------------------------------------------------------------------------*/
void Motor_Forward(void)
{
    /*
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    */
}

void Motor_StopGPIO(void)
{
    /*
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    */
}
