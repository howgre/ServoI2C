
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>


//Create object to represent PCA9685 at default I2C address
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pca96852 = Adafruit_PWMServoDriver(0x41);

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// Define maximum and minimum number of "ticks" for the servo motors
// Range from 0 to 4095
// This determines the pulse width

#define SERVOMIN 90 // Minimum value
#define SERVOMAX 500 // Maximum value

// Define servo motor connections (expand as required)
#define SER0 0   // Servo Motor 0 on connector 1
#define SER2  1  // My Servo motor on connector 2
#define SER3  2  // My Servo motor on connector 3


#define SER12 12  // Servo motor 12 on connector 12
#define SER13 13  // Servo motor 13 on connector 13
#define SER14 14  // Servo motor 14 on connector 14

long previousMillis = 0;
long interval = 10000;

int posDegrees = 0;

int posDelta = 1;

// timestamp
uint32_t ts = 0;

uint32_t td = 0;  //timedaelay for motion

void setup() {

  Serial.begin(115200);
  
  // Print to monitor
  Serial.println("PCA9685 Servo Test");

  // Initialise PCA9685
  pca9685.begin();
  pca96852.begin();

  //set PWM Frequency to 50Hz
  pca9685.setPWMFreq(SERVO_FREQ);
  pca96852.setPWMFreq(SERVO_FREQ);

}

void loop() {

  // only print every 100 millis
  if(millis()>ts+100) {
    ts = millis();
    // Print to serial monitor
    Serial.print("Pos degrees = ");
    Serial.println(posDegrees);
  }
  int pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX);
  int pwm0_rev = map(180-posDegrees, 0, 180, SERVOMIN, SERVOMAX);
  
  int pwm2 = map(posDegrees, 180, 0, SERVOMIN, SERVOMAX);
  int pwm2_rev = map(180-posDegrees, 0, 180, SERVOMIN, SERVOMAX);
  
  pca9685.setPWM(SER0, 0, pwm0);
  pca9685.setPWM(SER2, 0, pwm0_rev);
  pca96852.setPWM(SER0, 0, pwm2);

   // delay(10);
  if(millis()>td+10) {
    td = millis();
    // 
    
  
  //Attempt to narrow the start postion
  if(posDegrees<60) {
    posDegrees=60;
  }
  
  // increment posDegrees as necessary
  if(posDegrees+posDelta>120) {
    posDelta = -posDelta;
  } else if(posDegrees+posDelta<60) {
    posDelta = -posDelta;
  }
  //for(int cycles = 0; cycles <= 10; cycles++){
  posDegrees += posDelta;
  //}
  
  }
  //delay(1000);
}