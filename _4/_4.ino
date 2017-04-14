// Control a LED with Bluetooth LE on an Arduino 101
//try to write to ble service on signal
#include <CurieBLE.h>
#include <CurieIMU.h>

#define LED_PIN  6


#define RATE 100
#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //G
#define STEP  1000000 / RATE //step between the meassurements
#define DURATION 2000000//number of microseconds per strike

BLEPeripheral blePeripheral;

// create service
BLEService ledService = BLEService("FF10");

// create switch and Output characteristic
BLECharCharacteristic switchCharacteristic = BLECharCharacteristic("FF15", BLERead | BLEWrite);
BLEDescriptor switchDescriptor = BLEDescriptor("2905", "Switch");
BLELongCharacteristic OutputCharacteristic = BLELongCharacteristic("FF1D", BLERead | BLEWrite| BLENotify);
BLEDescriptor OutputDescriptor = BLEDescriptor("290D", "Output");

// event handlers
void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic);
void OutputCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic);

// initial values
long int nDim = 128;
bool bState = false;
unsigned long  microsPrevious, timeStart;

void setup() {
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(LED_PIN, OUTPUT);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("LED");
  blePeripheral.setDeviceName("LED");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);
  blePeripheral.addAttribute(switchDescriptor);
  blePeripheral.addAttribute(OutputCharacteristic);
  blePeripheral.addAttribute(OutputDescriptor);

  // assign event handlers for characteristic
  switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
  OutputCharacteristic.setEventHandler(BLEWritten, OutputCharacteristicWritten);

  // setting initial values so tey can be read correctly after start up
  switchCharacteristic.setValue((char)bState);
  OutputCharacteristic.setValue(0);
  
  // begin initialization
  blePeripheral.begin();

  Serial.println(F("Bluetooth LED"));
}

void loop() {
  // poll peripheral
  blePeripheral.poll(); 
    // put your main code here, to run repeatedly:
  updateData();
}

void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print(F("Characteristic event, written: "));
  bState = (bool)switchCharacteristic.value();

  if (bState) {
    Serial.println(F("LED on"));
    digitalWrite(LED_PIN, nDim);
  } else {
    Serial.println(F("LED off"));
    digitalWrite(LED_PIN, 0);
  }
}

void OutputCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {

  nDim = OutputCharacteristic.value();
  Serial.print(F("Output set to: "));
  Serial.println(nDim);

  if (bState)
    analogWrite(LED_PIN, nDim);
}

void updateData(){
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
    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + STEP;
    OutputCharacteristic.setValue(aix);
    Serial.print(aix);
  }
}
