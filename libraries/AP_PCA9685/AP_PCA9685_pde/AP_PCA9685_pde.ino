/*
 *       Example of PCA9685
 *       Code by Sgw32
 */


#include <AP_Common.h>
#include <AP_ADC.h>
#include <AP_InertialSensor.h>
#include <math.h>
#include <AP_Progmem.h>
#include <AP_Param.h>
#include <AP_Math.h>
#include <AP_HAL.h>
#include <AP_Buffer.h>
#include <Filter.h>
#include <GCS_MAVLink.h>
#include <AP_Notify.h>

#include <AP_HAL_AVR.h>
#include <AP_HAL_FLYMAPLE.h>
#include <AP_HAL_Linux.h>
#include <AP_HAL_Empty.h>

/* Build this example sketch only for the APM1. */
const AP_HAL::HAL& hal = AP_HAL_BOARD_DRIVER;

#include <PCA9685.h>

uint32_t timer;

void setup()
{
    hal.console->println("I2C testing");
    hal.console->println("Initialising device...");

    hal.scheduler->delay(100);
    
    
    hal.console->println("Initialisation complete.");
    hal.scheduler->delay(1000);
    timer = hal.scheduler->micros();
}

void loop()
{
   
    hal.scheduler->delay(1);
}

AP_HAL_MAIN();
