//Simple tap detect
//Two boards together
#include <CurieBLE.h>
#include <CurieIMU.h>
#include <CurieTime.h>
#include <MemoryFree.h>
#include <limits>

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

bool active=false;
long int t=0;
static void accelerationEvent(){
  if ((CurieIMU.getInterruptStatus(EVENT))&& active) {
    bleOutput(1);
    beep(100);
    t=millis();
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
BLECharCharacteristic inSignal=BLECharCharacteristic ("19B10001-E8F2-537E-4F6C-D104768AAD4", BLERead|BLEWrite );
BLECharCharacteristic outSignal=BLECharCharacteristic ("19B10002-E8F2-537E-4F6C-D104768AAD4", BLERead );


void bleOutput(int a){
  outSignal.setValue(a);
}

void inSignalWritten(BLECentral& central, BLECharacteristic& characteristic){
  if (inSignal.value()==0){
      active=false;
  }else{
      active=true;
      for (int i=0; i<3; i++){
          beep(100);
          delay(100);
      }
  }
}


void bleInit(){
  blePeripheral.setLocalName("FINT");
  blePeripheral.setDeviceName("FINT_DEV");
  blePeripheral.setAdvertisedServiceUuid(mainService.uuid());
  blePeripheral.addAttribute(mainService);
  blePeripheral.addAttribute(outSignal);
  blePeripheral.addAttribute(inSignal);
  inSignal.setEventHandler(BLEWritten, inSignalWritten);
  outSignal.setValue(0);
  inSignal.setValue(0);
  active=false;
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
  if (state!=digitalRead(LED_BUILTIN))
    digitalWrite(LED_BUILTIN,state);
}
////////////////////////////////////////////////////////////////////////
void setup() {  
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BEEPER,OUTPUT);
  digitalWrite(LED_BUILTIN,1);
  bleInit(); 
  measurementsInit();
  beep(200);
}

void loop() {
  blink();
  if (millis()-t>200){
   bleOutput(0);   
  }
}
