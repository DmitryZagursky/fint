//Second test
//Bluetooth connection
//Serial is used as control for indication

#include <CurieIMU.h>
#include <CurieBLE.h>
#include <BMI160.h>

const int ledPin = 13; // set ledPin to use on-board LED

//Ensure your 128-bit UUID is not the base BLE UUID (00000000-0000-1000-8000-00805F9B34FB)
//BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // create service
BLEService ledService("19B10200-E8F2-537E-4F6C-D104768A3124"); // create service

// create switch characteristic and allow remote device to read and write
BLECharCharacteristic switchChar("19B10200-E8F2-537E-4F6C-D104768A3124", BLERead | BLEWrite);


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); // use the LED on pin 13 as an output

  // begin initialization
  BLE.begin();
  // set the local name peripheral advertises
  BLE.setLocalName("LEDCB");
  // set the UUID for the service this peripheral advertises
  BLE.setAdvertisedService(ledService);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  
}

// the loop function runs over and over again forever
void loop() {
    
}


void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}
