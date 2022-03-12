#include "Grove_Motor_Driver_TB6612FNG.h"
#include <Wire.h>

MotorDriver motor;

int AIN1 = 32; // green, short side, 4 up from bottom
int AIN2 = 15; // green, next one up
int PWMA = 33; // yellow, next one up
int STBY = 27; // white, next one up
// black to gnd, 4 down from long side
// Red is V Motor (VM)
// Orange is V Logic (VCC)

void setup()
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    Serial.begin(9600);
//    motor.init();
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
}

void loop()
{
    // drive 2 dc motors at speed=255, clockwise
    Serial.println("run at speed=255");
//    motor.dcMotorRun(MOTOR_CHA, 255);
    digitalWrite(PWMA, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
//    motor.dcMotorRun(MOTOR_CHB, 255);
    delay(1000);
    
    // brake
    Serial.println("brake");
    digitalWrite(PWMA, HIGH);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);    
   // motor.dcMotorBrake(MOTOR_CHA);
//    motor.dcMotorBrake(MOTOR_CHB);
    delay(1000);

    // drive 2 dc motors at speed=200, anticlockwise
    //Serial.println("run at speed=-200");
  //  motor.dcMotorRun(MOTOR_CHA, -200);
 //   motor.dcMotorRun(MOTOR_CHB, -200);
    delay(1000);

    // stop 2 motors
    Serial.println("stop");
   // motor.dcMotorStop(MOTOR_CHA);
   // motor.dcMotorStop(MOTOR_CHB);
    delay(1000);
}
