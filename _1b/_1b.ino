//First test
//Accelerometer data over serial
//Attempt serial receives command to start

#include <CurieIMU.h>
#include <CurieBLE.h>
#include <BMI160.h>

#define RATE 100
#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //G
#define STEP  1000000 / RATE //step between the meassurements
#define LED 13//LED pin number
#define BUTTON 4
#define DURATION 2000000//number of microseconds per strike

unsigned long  microsPrevious, timeStart;

// the setup function runs once when you press reset or power the board
void setup() {
  // init sensor
  CurieIMU.begin();
  CurieIMU.setGyroRate(RATE);
  CurieIMU.setAccelerometerRate(RATE);
  CurieIMU.setGyroRange(GYRO_RANGE);
  CurieIMU.setAccelerometerRange(ACCEL_RANGE);
  
  //init timer
  timeStart=-1000000;
  
  //init serial for sync
  Serial.begin(9600);
  while (!Serial); 
  Serial.print("READY");
  Serial.flush();
  //pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT); // use the LED on pin 13 as an output
}

// the loop function runs over and over again forever
void loop() {
  // put your main code here, to run repeatedly:
  int aix, aiy, aiz;
  int gix, giy, giz;
  unsigned long microsNow;
  int incoming=0;
  if (Serial.available() > 0) {
    // read the incoming bytes:
    //incoming is the number of bytes read, whatever data was send is discarded actually
    incoming = Serial.read();
    if (incoming!=0){ 
      timeStart=micros();
      microsPrevious = micros();
      Serial.print("\n");
    }
  }
  
  microsNow = micros();
  if ((microsNow-timeStart <= DURATION) && (microsNow - microsPrevious >= STEP)) {
    // read raw data from CurieIMU
    CurieIMU.readMotionSensor(aix, aiy, aiz, gix, giy, giz);
    if (microsNow - microsPrevious >= 2*STEP) {
      Serial.print("(");
      Serial.print((microsNow - microsPrevious)/STEP);
      Serial.print(" steps skipped)");
    }
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
    Serial.print("|");
    Serial.flush();
  }
}
