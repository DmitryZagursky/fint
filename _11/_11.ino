//Attempt to use "notify" ble characteristic
//It's not very effective
#include <CurieBLE.h>
#include <CurieIMU.h>
#include <CurieTime.h>

#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //Range of accelerometer in g. Raw g is integer from -32768 to +32768. 
////////////////////
//MEASUREMENT TASK//
const short BEEPER=12;
const short measurementRate=2e2;//Times per second
const short measurementStep=1e3/measurementRate;//Milliseconds

void beep();

void measurementsInit(){
  // init sensor
  CurieIMU.begin();
  CurieIMU.setGyroRate(measurementRate);
  CurieIMU.setAccelerometerRate(measurementRate);
  CurieIMU.setGyroRange(GYRO_RANGE);
  CurieIMU.setAccelerometerRange(ACCEL_RANGE);
}
////////////////////
//Bluetooth TASK//
BLEPeripheral blePeripheral;
// create service
BLEService mainService = BLEService("19B10000-E8F2-537E-4F6C-D104768AAD4");
// create switch and Output characteristic
BLEIntCharacteristic outSignal = BLEIntCharacteristic("19B10002-E8F2-537E-4F6C-D104768AAD4", BLERead|BLENotify );

void outputEvent(BLECentral& central, BLECharacteristic& characteristic){
  beep();
  int a=0;
  int ax,ay,az;
  CurieIMU.readAccelerometer(ax,ay,az);
  a=ax*ax+ay*ay+az*az;
  outSignal.setValue(a);
  delay(1000);
}
void bleInit(){
    // set advertised local name and service UUID
  blePeripheral.setLocalName("FINT");
  blePeripheral.setAdvertisedServiceUuid(mainService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(mainService);
  blePeripheral.addAttribute(outSignal);
  outSignal.setEventHandler(BLESubscribed,outputEvent);
  // begin initialization
  blePeripheral.begin();
}
//blinkers
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

void beep(){
  digitalWrite(BEEPER,1);
  delay(500);
  digitalWrite(BEEPER,0);
}
void setup() {  
  bleInit();
}

void loop() {
  //Serial.write("LOOP\n");
  blink();
}
