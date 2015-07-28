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
#include <AP_Baro.h>
#include <GCS_MAVLink.h>
#include <AP_Notify.h>

#include <AP_HAL_AVR.h>
#include <AP_HAL_FLYMAPLE.h>
#include <AP_HAL_Linux.h>
#include <AP_HAL_Empty.h>

/* Build this example sketch only for the APM1. */
const AP_HAL::HAL& hal = AP_HAL_BOARD_DRIVER;

#define PCA9685_I2C_ADDRESS 0x38
#define PCA9685_I2C_ADDRESS2 0x38
#define PCA9685_I2C_BASE_ADDRESS 0x40
#define PCA9685_MODE1 0x00 // location for Mode1 register address
#define PCA9685_MODE2 0x01 // location for Mode2 reigster address
#define PCA9685_LED0 0x06 // location for start of LED0 registers

#define I2C_24C02 0x50
#define PCA9685_I2C_BASE_ADDRESS 0x40
#define PCA9685_MODE1 0x00 // location for Mode1 register address
#define PCA9685_MODE2 0x01 // location for Mode2 reigster address
#define PCA9685_LED0 0x06 // location for start of LED0 registers


uint32_t timer;
uint8_t i2cres=0;
uint8_t i2cadress;

void setup()
{
    
      hal.console->println("ArduPilot Mega BMP085 library test");
    hal.console->println("Initialising PCA9685...");

    hal.scheduler->delay(100);
    AP_HAL::Semaphore* i2c_sem = hal.i2c->get_semaphore();

    // take i2c bus sempahore
    if (!i2c_sem->take(HAL_SEMAPHORE_BLOCK_FOREVER))
        return;
        
    i2cadress =  PCA9685_I2C_BASE_ADDRESS;  
}

void loop()
{
 
	 
	if (hal.i2c->writeRegister(i2cadress, 0x0,0)!=0)
	{
		hal.console->println("PCA9685 I2C connection error.");
	}
        hal.console->println("PCA9685 MODE1.");
	
        if (hal.i2c->writeRegister(i2cadress, 0x1,0x4)!=0)
	{
		hal.console->println("PCA9685 I2C connection error.");
	}
        hal.console->println("PCA9685 MODE2.");

        hal.console->println("Again.");
}

AP_HAL_MAIN();
