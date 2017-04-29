//Simple tap detect
#include <CurieBLE.h>
#include <CurieIMU.h>
#include <CurieTime.h>
#include <MemoryFree.h>
#include <limits>

#define LED_PIN  LED_BUILTIN
#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //Range of accelerometer in g. Raw g is integer from -32768 to +32768. 
//#define EVENT CURIE_IMU_SHOCK
#define EVENT CURIE_IMU_TAP

void bleOutput(int);
void beep(int);
const short BEEPER=12;

const int shockTreshhold=12e3;//mg
const int shockDuration=50;//ms
int measurementRate=2e2;

static void accelerationEvent(){
  if (CurieIMU.getInterruptStatus(EVENT)) {
    bleOutput(1);
    beep(100);
    bleOutput(0);
  }
}
void measurementsInit(){
  // init sensor
  CurieIMU.begin();
  CurieIMU.setGyroRate(measurementRate);
  CurieIMU.setAccelerometerRate(measurementRate);
  CurieIMU.setGyroRange(GYRO_RANGE);
  CurieIMU.setAccelerometerRange(ACCEL_RANGE);
  CurieIMU.attachInterrupt(accelerationEvent);
  CurieIMU.interrupts(EVENT);
  CurieIMU.setDetectionThreshold(EVENT, shockTreshhold); // 1g=1000mg
  CurieIMU.setDetectionDuration(EVENT, shockDuration);  // 50ms
  
  //CurieIMU.begin();
  //CurieIMU.attachInterrupt(eventCallback);
  //// Increase Accelerometer range to allow detection of stronger taps (< 4g)
  //CurieIMU.setAccelerometerRange(4);

  //// Reduce threshold to allow detection of weaker taps (>= 750mg)
  //CurieIMU.setDetectionThreshold(CURIE_IMU_TAP, 750); // (750mg)

  //// Enable Double-Tap detection
  //CurieIMU.interrupts(CURIE_IMU_TAP);

}
  
////////////////////
//Bluetooth TASK//
BLEPeripheral blePeripheral;
// create service
BLEService mainService = BLEService("19B10000-E8F2-537E-4F6C-D104768AAD4");
// create switch and Output characteristic
BLEIntCharacteristic outSignal = BLEIntCharacteristic("19B10002-E8F2-537E-4F6C-D104768AAD4", BLERead );


void bleOutput(int a){
  outSignal.setValue(a);
}

void bleInit(){
  blePeripheral.setLocalName("FINT");
  blePeripheral.setAdvertisedServiceUuid(mainService.uuid());
  blePeripheral.addAttribute(outSignal);
  outSignal.setValue(0);
  blePeripheral.begin();
}
void beep(int t){
  digitalWrite(BEEPER,1);
  delay(t);
  digitalWrite(BEEPER,0);
}
void blink(){
  int t=second();
  int state;
  if (t%2==0)
    state=1;
  else
    state=0;
  if (state!=digitalRead(LED_PIN))
    digitalWrite(LED_PIN,state);
}
////////////////////////////////////////////////////////////////////////
void setup() {  
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BEEPER,OUTPUT);
  digitalWrite(LED_PIN,1);
  bleInit(); 
  beep(200);
  measurementsInit();
}

void loop() {
  blink();
}
