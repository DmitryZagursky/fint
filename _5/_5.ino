//Just put data on air
//DOESNT WORK RELIABLY ARDUINO HANGS UP
#include <CurieBLE.h>
#include <CurieIMU.h>
#include <CurieTime.h>


#define LED_PIN  13
#define RATE 10
#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //G
#define STEP  1000000 / RATE //step between the meassurements
#define DURATION 2000000//number of microseconds per strike

BLEPeripheral blePeripheral;


// create service
BLEService mainService = BLEService("19B10000-E8F2-537E-4F6C-D104768AAD4");
// create switch and Output characteristic
BLEIntCharacteristic axTooth = BLEIntCharacteristic("19B10001-E8F2-537E-4F6C-D104768AAD4", BLERead );
BLEIntCharacteristic ayTooth = BLEIntCharacteristic("19B10002-E8F2-537E-4F6C-D104768AAD4", BLERead );
BLEIntCharacteristic azTooth = BLEIntCharacteristic("19B10003-E8F2-537E-4F6C-D104768AAD4", BLERead );
void initBluetooth();

// event handlers
//void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic);
//void OutputCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic);

unsigned long  microsPrevious, timeStart;
void initAccelerometers();

void setup() {
  //init serial for sync
  Serial.begin(9600);
  
  initAccelerometers();
  initBluetooth();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,1);
  //while (!Serial); 
  Serial.print(F("READY"));
  Serial.flush();
  
  microsPrevious = micros();
}

void loop() {
  updateData();
  blink();
}

void updateData(){
 int aix, aiy, aiz;
  int gix, giy, giz;
  unsigned long microsNow;
  microsNow = micros();
  if (microsNow - microsPrevious >= STEP) {
    // read raw data from CurieIMU
    CurieIMU.readMotionSensor(aix, aiy, aiz, gix, giy, giz);
    // increment previous time, so we keep proper pace
    Serial.print(microsNow - microsPrevious);
    Serial.print(' ');
    microsPrevious = microsPrevious + STEP;
    //microsPrevious = microsNow;
    axTooth.setValue(aix);
    ayTooth.setValue(aiy);
    azTooth.setValue(aiz);
    Serial.print(aix);
    Serial.print("|");
    //Serial.print(aiy);
    //Serial.print("|");
    //Serial.print(aiz);
    //Serial.print('-');
    Serial.print(axTooth.value());
    //Serial.print("|");
    //Serial.print(ayTooth.value());
    //Serial.print("|");
    //Serial.print(azTooth.value());
    Serial.print("\n");
  }
}

void initBluetooth(){
    // set advertised local name and service UUID
  blePeripheral.setLocalName("FINT");
  blePeripheral.setDeviceName("FINT_DEV");
  blePeripheral.setAdvertisedServiceUuid(mainService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(mainService);
  blePeripheral.addAttribute(axTooth);
  blePeripheral.addAttribute(ayTooth);
  blePeripheral.addAttribute(azTooth);
  // assign event handlers for characteristic
  
  // setting initial values so tey can be read correctly after start up
  axTooth.setValue(0);
  ayTooth.setValue(0);
  azTooth.setValue(0);
  
  // begin initialization
  blePeripheral.begin();
}
void initAccelerometers(){
    // init sensor
  CurieIMU.begin();
  CurieIMU.setGyroRate(RATE);
  CurieIMU.setAccelerometerRate(RATE);
  CurieIMU.setGyroRange(GYRO_RANGE);
  CurieIMU.setAccelerometerRange(ACCEL_RANGE);
  
  //init timer
  timeStart=-1000000;
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
