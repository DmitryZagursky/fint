// Control a LED with Bluetooth LE on an Arduino 101
#include <CurieBLE.h>

#define LED_PIN  6
BLEPeripheral blePeripheral;

// create service
BLEService ledService = BLEService("FF10");

// create switch and dimmer characteristic
BLECharCharacteristic switchCharacteristic = BLECharCharacteristic("FF15", BLERead | BLEWrite);
BLEDescriptor switchDescriptor = BLEDescriptor("2905", "Switch");
BLEUnsignedCharCharacteristic dimmerCharacteristic = BLEUnsignedCharCharacteristic("FF1D", BLERead | BLEWrite);
BLEDescriptor dimmerDescriptor = BLEDescriptor("290D", "Dimmer");

// event handlers
void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic);
void dimmerCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic);

// initial values
unsigned char nDim = 128;
bool bState = false;

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
  blePeripheral.addAttribute(dimmerCharacteristic);
  blePeripheral.addAttribute(dimmerDescriptor);

  // assign event handlers for characteristic
  switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
  dimmerCharacteristic.setEventHandler(BLEWritten, dimmerCharacteristicWritten);

  // setting initial values so tey can be read correctly after start up
  switchCharacteristic.setValue((char)bState);
  dimmerCharacteristic.setValue(nDim);
  
  // begin initialization
  blePeripheral.begin();

  Serial.println(F("Bluetooth LED"));
}

void loop() {
  // poll peripheral
  blePeripheral.poll();  
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

void dimmerCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {

  nDim = dimmerCharacteristic.value();
  Serial.print(F("Dimmer set to: "));
  Serial.println(nDim);

  if (bState)
    analogWrite(LED_PIN, nDim);
}
