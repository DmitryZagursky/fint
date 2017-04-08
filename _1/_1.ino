//First test
//Accelerometer data over serial

#include <CurieIMU.h>
#include <CurieBLE.h>
#include <BMI160.h>

#define RATE 100
#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //G
#define STEP  1000000 / RATE //step between the meassurements
#define LED 13
#define BUTTON 4

unsigned long  microsPrevious;

// the setup function runs once when you press reset or power the board
void setup() {
  // init sensor
  CurieIMU.begin();
  CurieIMU.setGyroRate(RATE);
  CurieIMU.setAccelerometerRate(RATE);
  CurieIMU.setGyroRange(GYRO_RANGE);
  CurieIMU.setAccelerometerRange(ACCEL_RANGE);
  
  // initialize variables to pace updates to correct rate
  microsPrevious = micros();
  
  //init serial for sync
  Serial.begin(9600);
  while (!Serial); 
  Serial.println("Hellow...");//testing
  pinMode(LED, OUTPUT); // use the LED on pin 13 as an output
  //pinMode(BUTTON, INPUT);
}

// the loop function runs over and over again forever
void loop() {
  // put your main code here, to run repeatedly:
  int aix, aiy, aiz;
  int gix, giy, giz;
  unsigned long microsNow;
  microsNow = micros();
  if (microsNow - microsPrevious >= STEP) {
    // read raw data from CurieIMU
    CurieIMU.readMotionSensor(aix, aiy, aiz, gix, giy, giz);


    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + STEP;
    Serial.print(">");
    Serial.print(aix);
    Serial.print(" ");
    Serial.print(aiy);
    Serial.print(" ");
    Serial.print(aiz);
    Serial.print(" ");
    Serial.print(gix);
    Serial.print(" ");
    Serial.print(giy);
    Serial.print(" ");
    Serial.print(giz);
    Serial.print("\n");
  }
}
