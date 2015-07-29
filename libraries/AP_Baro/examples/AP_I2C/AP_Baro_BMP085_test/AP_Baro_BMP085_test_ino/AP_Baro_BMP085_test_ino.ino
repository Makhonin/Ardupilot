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
#include <PCA9685.h>
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



uint32_t timer;
uint8_t i2cres=0;
uint8_t i2cadress;

uint16_t cnt;
uint16_t pwm1,pwm2,pwm3;
int16_t dird=1;


void setup()
{

  hal.console->println("PCA9685 library test");
  hal.console->println("Initialising PCA9685...");

  hal.scheduler->delay(100);
  AP_HAL::Semaphore* i2c_sem = hal.i2c->get_semaphore();

  // take i2c bus sempahore
  if (!i2c_sem->take(HAL_SEMAPHORE_BLOCK_FOREVER))
    return;

  dird=5;
  cnt=1;
  pwm1=1;
  pwm2=1;
  pwm3=1;
  initPCA9685();
}

void loop()
{
  cnt+=dird;
  pwm1+=pwm2;
  if (cnt>1000)
    dird=-dird;
  if (cnt<6)
    dird=-dird;
  if (pwm1>1000)
    pwm2=-pwm2;
  if (pwm1<1)
    pwm2=-pwm2;
  //setServo(0,0x0,cnt);
  hal.console->printf("Servo1 PWM %lu",(uint32_t)(1000+cnt));
  setServo(1,1000+cnt); //cnt from 0 to 4096
  hal.console->printf("Servo2 PWM %lu",(uint32_t)(1000+1000*(pwm1&0x08)/0x08));
  setServo(2,1000+1000*(pwm1&0x08)/0x08);
  hal.console->printf("Servo3 PWM %lu",(uint32_t)(1000+1000*(pwm1&0x10)/0x10));
  setServo(3,1000+1000*(pwm1&0x10)/0x10);
  hal.console->printf("Servo4 PWM %lu",(uint32_t)(1000+1000*(pwm1&0x20)/0x20));
  setServo(4,1000+1000*(pwm1&0x20)/0x20);

  //hal.console->println("Again.");
  hal.scheduler->delay(100);
}

AP_HAL_MAIN();

