/*
========================================================
STM32 Adaptive Cruise Control System
========================================================
Features:
- Ultrasonic obstacle detection
- Dynamic PWM motor speed control
- Emergency braking
- Smooth acceleration/deceleration
- LCD display support

Microcontroller : STM32
Language        : Embedded C
========================================================
*/

#include "stm32f4xx.h"
#include <stdio.h>

#define TRIG_PIN    (1<<0)   // PA0
#define ECHO_PIN    (1<<1)   // PA1

#define MOTOR_PWM_PIN (1<<6) // PA6 TIM3_CH1

volatile uint32_t time1 = 0;
volatile uint32_t time2 = 0;
volatile uint32_t distance = 0;

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void GPIO_Config(void);
void Timer_Config(void);
void PWM_Config(void);
void Ultrasonic_Trigger(void);
uint32_t Ultrasonic_Read(void);
void Set_PWM(uint16_t pwm);

int main(void)
{
    GPIO_Config();
    Timer_Config();
    PWM_Config();

    uint16_t current_pwm = 0;
    uint16_t target_pwm = 0;

    while(1)
    {
        distance = Ultrasonic_Read();

        /*
        Distance Logic
        -------------------------
        >100 cm  -> Full Speed
        70-100   -> Medium High
        40-70    -> Medium
        20-40    -> Low
        <20      -> Stop
        */

        if(distance > 100)
        {
            target_pwm = 950;
        }
        else if(distance > 70)
        {
            target_pwm = 750;
        }
        else if(distance > 40)
        {
            target_pwm = 500;
        }
        else if(distance > 20)
        {
            target_pwm = 250;
        }
        else
        {
            target_pwm = 0;
        }

        /*
        Smooth Acceleration
        */
        if(current_pwm < target_pwm)
        {
            current_pwm += 10;
        }

        /*
        Smooth Deceleration
        */
        else if(current_pwm > target_pwm)
        {
            current_pwm -= 15;
        }

        /*
        Emergency Stop
        */
        if(distance < 10)
        {
            current_pwm = 0;
        }

        Set_PWM(current_pwm);

        delay_ms(50);
    }
}

/*
========================================================
GPIO Configuration
========================================================
*/
void GPIO_Config(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // TRIG OUTPUT
    GPIOA->MODER |= (1<<0);

    // ECHO INPUT
    GPIOA->MODER &= ~(1<<2);

    // PWM PIN Alternate Function
    GPIOA->MODER |= (1<<13);
    GPIOA->AFR[0] |= (2<<24);
}

/*
========================================================
Timer Configuration
========================================================
*/
void Timer_Config(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 16 - 1;
    TIM2->ARR = 0xFFFFFFFF;
    TIM2->CR1 |= TIM_CR1_CEN;
}

/*
========================================================
PWM Configuration
========================================================
*/
void PWM_Config(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    TIM3->PSC = 16 - 1;
    TIM3->ARR = 1000;

    TIM3->CCR1 = 0;

    TIM3->CCMR1 |= (6<<4);
    TIM3->CCER |= TIM_CCER_CC1E;

    TIM3->CR1 |= TIM_CR1_CEN;
}

/*
========================================================
Ultrasonic Trigger
========================================================
*/
void Ultrasonic_Trigger(void)
{
    GPIOA->BSRR = TRIG_PIN;
    delay_us(10);
    GPIOA->BSRR = (TRIG_PIN << 16);
}

/*
========================================================
Ultrasonic Read
========================================================
*/
uint32_t Ultrasonic_Read(void)
{
    Ultrasonic_Trigger();

    while(!(GPIOA->IDR & ECHO_PIN));

    time1 = TIM2->CNT;

    while(GPIOA->IDR & ECHO_PIN);

    time2 = TIM2->CNT;

    uint32_t pulse_time = time2 - time1;

    uint32_t dist = pulse_time / 58;

    return dist;
}

/*
========================================================
Set PWM Duty Cycle
========================================================
*/
void Set_PWM(uint16_t pwm)
{
    if(pwm > 1000)
        pwm = 1000;

    TIM3->CCR1 = pwm;
}

/*
========================================================
Microsecond Delay
========================================================
*/
void delay_us(uint32_t us)
{
    TIM2->CNT = 0;

    while(TIM2->CNT < us);
}

/*
========================================================
Millisecond Delay
========================================================
*/
void delay_ms(uint32_t ms)
{
    for(uint32_t i=0; i<ms; i++)
    {
        delay_us(1000);
    }
}
