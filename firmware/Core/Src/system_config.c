/******************************************************************************
 * File        : system_config.c
 * Description : System Configuration
 ******************************************************************************/

#include "system_config.h"

/*---------------------------------------------------------------------------
 * Global Configuration Parameters
 *---------------------------------------------------------------------------*/

const uint8_t MAX_VEHICLE_SPEED = MAX_SPEED;

const uint8_t MIN_VEHICLE_SPEED = MIN_SPEED;

const uint8_t SAFE_DISTANCE_THRESHOLD = SAFE_DISTANCE;

const uint8_t WARNING_DISTANCE_THRESHOLD = WARNING_DISTANCE;

const uint8_t STOP_DISTANCE_THRESHOLD = STOP_DISTANCE;

/*---------------------------------------------------------------------------
 * Configuration Functions
 *---------------------------------------------------------------------------*/

uint8_t Get_MaxSpeed(void)
{
    return MAX_VEHICLE_SPEED;
}

uint8_t Get_MinSpeed(void)
{
    return MIN_VEHICLE_SPEED;
}

uint8_t Get_SafeDistance(void)
{
    return SAFE_DISTANCE_THRESHOLD;
}

uint8_t Get_WarningDistance(void)
{
    return WARNING_DISTANCE_THRESHOLD;
}

uint8_t Get_StopDistance(void)
{
    return STOP_DISTANCE_THRESHOLD;
}
