/*
 *       Example of APM_BMP085 (absolute pressure sensor) library.
 *       Code by Jordi MuÒoz and Jose Julio. DIYDrones.com
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


uint32_t timer;

void setup()
{
    hal.console->println("I2C testing");
    hal.console->println("Initialising device...");

    hal.scheduler->delay(100);
    
    
    AP_HAL::Semaphore* i2c_sem = hal.i2c->get_semaphore();

    // take i2c bus sempahore
    if (!i2c_sem->take(HAL_SEMAPHORE_BLOCK_FOREVER))
        return false;
        
        
   // if (!bmp085.init()) {
        hal.console->println("FAILED\n");
  //  }
    hal.console->println("Initialisation complete.");
    hal.scheduler->delay(1000);
    timer = hal.scheduler->micros();
}

void loop()
{
   
    hal.scheduler->delay(1);
}

AP_HAL_MAIN();