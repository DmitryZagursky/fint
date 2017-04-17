//Minimal data exchange
//Program Client writes to Arduino service, arduino does stuff sets the service to initial value, sets other service value to result 
#include <CurieBLE.h>
#include <CurieIMU.h>
#include <CurieTime.h>

#define LED_PIN  LED_BUILTIN
#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //Range of accelerometer in g. Raw g is integer from -32768 to +32768. 
#define DURATION 2000000//number of microseconds per strike
enum state {//Possible states of arduino board
  MEASURING,//Reads accelerometers, deduces acceleration value, at the end of measurement outputs mean
  WAITING//Does polls events, don't know why, events are detected even without it
};

void bleOutput(int);
void setState(state S);
////////////////////
//MEASUREMENT TASK//
const int measurementRate=1e2;//Times per second
const int measurementStep=1e3/measurementRate;//Milliseconds
const int measurementDuration=2e3;//Milliseconds
const int measurementN=measurementDuration/measurementStep;
long int measurementPreviousTime=0;
long int measurementBeginning=0;
int measurementPos=0;
int measurements[measurementN];

void measurementsInit(){
  // init sensor
  CurieIMU.begin();
  CurieIMU.setGyroRate(measurementRate);
  CurieIMU.setAccelerometerRate(measurementRate);
  CurieIMU.setGyroRange(GYRO_RANGE);
  CurieIMU.setAccelerometerRange(ACCEL_RANGE);
}
void startMeasuring(){
  measurementBeginning=millis();
  measurementPreviousTime=millis();
  measurementPos=0;
  for (int i=0; i<measurementN; i++){
    measurements[i]=0;
  }
}
void measure(){
  long int currentTime=millis();
  if (currentTime-measurementPreviousTime >= measurementStep){
    int a=0;
    int ax,ay,az;
    CurieIMU.readAccelerometer(ax,ay,az);
    a=ax*ax+ay*ay+az*az;
    measurementPreviousTime=currentTime;
    measurements[measurementPos]=a;
    measurementPos++;
    bleOutput(measurementPos);
  }
  //if ((measurementPos>=measurementN)||(currentTime-measurementBeginning >= measurementDuration)){
  if (measurementPos>=measurementN){
    int mean=0;
    for (int i=0;i<measurementN;i++){
      mean+=measurements[i];
    }
    bleOutput(mean);
    setState(WAITING);
  }
}


////////////////////
//Bluetooth TASK//
BLEPeripheral blePeripheral;
// create service
BLEService mainService = BLEService("19B10000-E8F2-537E-4F6C-D104768AAD4");
// create switch and Output characteristic
BLECharCharacteristic inSignal = BLECharCharacteristic("19B10001-E8F2-537E-4F6C-D104768AAD4", BLERead|BLEWrite );
BLEIntCharacteristic outSignal = BLEIntCharacteristic("19B10002-E8F2-537E-4F6C-D104768AAD4", BLERead );

void inSignalWritten(BLECentral& central, BLECharacteristic& characteristic){
  if (inSignal.value()==1)
    setState(MEASURING);
}
void bleOutput(int a){
  inSignal.setValue(0);
  outSignal.setValue(a);
}

void bleInit(){
    // set advertised local name and service UUID
  blePeripheral.setLocalName("FINT");
  blePeripheral.setDeviceName("FINT_DEV");
  blePeripheral.setAdvertisedServiceUuid(mainService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(mainService);
  blePeripheral.addAttribute(inSignal);
  blePeripheral.addAttribute(outSignal);
  
  // setting initial values so tey can be read correctly after start up
  inSignal.setValue(0);
  outSignal.setValue(0);
  
  inSignal.setEventHandler(BLEWritten, inSignalWritten);
  // begin initialization
  blePeripheral.begin();
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

////////////////////////////
//DISPATCH TASK//
state BoardState = WAITING;
void setState(state S){
  switch(BoardState){
    case WAITING:{
        switch(S){
          case WAITING: break;
          case MEASURING: startMeasuring(); break;
        }
    break;
    };
    case MEASURING:{
      break;
    };
  };
  BoardState=S;
}
void update(){
  switch(BoardState){
    case WAITING: blePeripheral.poll(); break;
    case MEASURING: measure(); break;
  };
}
////////////////////////////////////////////////////////////////////////
void setup() {  
  //initAccelerometers();
  bleInit();
  measurementsInit();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,1);
}

void loop() {
  blink();
  update();
}
