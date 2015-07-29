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

#define PCA9685_RESTART 0x80 // 0b10000000
#define PCA9685_EXTCLK 0x40 // 0b01000000
#define PCA9685_AI 0x20 // 0b00100000
#define PCA9685_SLEEP 0x10 // 0b00010000
#define PCA9685_SUB1 0x8 // 0b00001000
#define PCA9685_SUB2 0x4 // 0b00000100
#define PCA9685_SUB3 0x2 // 0b00000010
#define PCA9685_ALLCALL 0x1 // 0b00000001

//MODE 2
#define PCA9685_INVRT 0x10 // 0b00010000
#define PCA9685_OCH 0x8 // 0b00001000
#define PCA9685_OUTDRV 0x4 // 0b00000100
#define PCA9685_OUTNE1 0x2 // 0b00000010
#define PCA9685_OUTNE2 0x1 // 0b00000001

#define PCA9685_PRESCALER 0xFE
#define PCA9685_MODE1 0x00
#define PCA9685_MODE2 0x01

#define CALC_FREQ(x) 25000000/4096/x-1

#define LED_ON_L(x) 0x6+4*x
#define LED_ON_H(x) 0x7+4*x
#define LED_OFF_L(x) 0x8+4*x
#define LED_OFF_H(x) 0x9+4*x

#define LOW_PART(x) x&0xFF
#define HIGH_PART(x) x>>8


uint32_t timer;
uint8_t i2cres=0;
uint8_t i2cadress;

uint16_t cnt;
uint16_t pwm1,pwm2,pwm3;
int16_t dird=1;

uint8_t writeRegg2(uint8_t reg,uint8_t data)
{
  if (hal.i2c->writeRegister(PCA9685_I2C_BASE_ADDRESS , reg,data)!=0)
  {
    hal.console->println("PCA9685 I2C connection error.");
  }
}

void initPCA9685()
{
  uint16_t led0on = 0x409;
  uint16_t led0off = 0x1228;
  writeRegg2(PCA9685_MODE1,0x10);
  //uint32_t fr = 500000/4096-1;
  writeRegg2(PCA9685_PRESCALER,133);
  writeRegg2(PCA9685_MODE1,0x1);
  writeRegg2(PCA9685_MODE2,0x14&(~PCA9685_INVRT));

  writeRegg2(LED_ON_L(0),LOW_PART(led0on));
  writeRegg2(LED_ON_H(0),HIGH_PART(led0on));
  writeRegg2(LED_OFF_L(0),LOW_PART(led0off));
  writeRegg2(LED_OFF_H(0),HIGH_PART(led0off));
}

void setPWM(uint8_t num, uint16_t on,uint16_t off)
{
  writeRegg2(LED_ON_L(num),LOW_PART(on));
  writeRegg2(LED_ON_H(num),HIGH_PART(on));
  writeRegg2(LED_OFF_L(num),LOW_PART(off));
  writeRegg2(LED_OFF_H(num),HIGH_PART(off));
}

void setServo(uint8_t num, uint16_t pwm) //angle from 1000 to 2000
{
  // 4096 - 20 msecs
  // 4096/20 - 1 msec
  // 4096/10 - 2 msec
  uint32_t pwm2d = 4096*((uint32_t)pwm);
  pwm2d/=20000;
  hal.console->printf("PWM2: %lu",pwm2d);
  writeRegg2(LED_ON_L(num),LOW_PART(0));
  writeRegg2(LED_ON_H(num),HIGH_PART(0));
  writeRegg2(LED_OFF_L(num),LOW_PART(pwm2d));
  writeRegg2(LED_OFF_H(num),HIGH_PART(pwm2d));
}

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

