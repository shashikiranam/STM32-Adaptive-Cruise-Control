/******************************************************************************
 * File        : main.c
 * Description : implementation for STM32-Based Adaptive Cruise
 *               Control and Collision Avoidance System
 *
 * Target MCU  : STM32G071RBT6 (NUCLEO-G071RB)
 ******************************************************************************/

#include "main.h"

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
float vehicleDistance = 0.0f;
uint8_t vehicleSpeed = 0;

/*---------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
static void System_Init(void);
static void Read_Sensors(void);
static void Execute_ACC(void);
static void Update_Actuators(void);

/*---------------------------------------------------------------------------
 * Main Function
 *---------------------------------------------------------------------------*/
int main(void)
{
    /* Initialize complete system */
    System_Init();

    /* Main Control Loop */
    while (1)
    {
        /* Read ultrasonic sensor */
        Read_Sensors();

        /* Adaptive Cruise Control Algorithm */
        Execute_ACC();

        /* Update motor speed */
        Update_Actuators();
    }
}

/*---------------------------------------------------------------------------
 * System Initialization
 *---------------------------------------------------------------------------*/
static void System_Init(void)
{
    GPIO_Init_Custom();

    Timer_Init_Custom();

    PWM_Init();

    Motor_Init();

    Ultrasonic_Init();

    ACC_Init();

    Motor_Start();
}

/*---------------------------------------------------------------------------
 * Read Sensors
 *---------------------------------------------------------------------------*/
static void Read_Sensors(void)
{
    vehicleDistance = Ultrasonic_ReadDistance();
}

/*---------------------------------------------------------------------------
 * Execute Adaptive Cruise Control
 *---------------------------------------------------------------------------*/
static void Execute_ACC(void)
{
    ACC_Update();
}

/*---------------------------------------------------------------------------
 * Update Motor
 *---------------------------------------------------------------------------*/
static void Update_Actuators(void)
{
    if (vehicleDistance > SAFE_DISTANCE)
    {
        vehicleSpeed = 100;
    }
    else if (vehicleDistance > WARNING_DISTANCE)
    {
        vehicleSpeed = 60;
    }
    else if (vehicleDistance > STOP_DISTANCE)
    {
        vehicleSpeed = 30;
    }
    else
    {
        vehicleSpeed = 0;
    }

    Motor_SetSpeed(vehicleSpeed);
}
