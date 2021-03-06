//Reinvention of serial interface over BLE
//Program Client writes to Arduino service, arduino does stuff sets the service to initial value, sets other service value to result 
//Multiple storage
#include <CurieBLE.h>
#include <CurieIMU.h>
#include <CurieTime.h>
#include <MemoryFree.h>


#define LED_PIN  LED_BUILTIN
#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //Range of accelerometer in g. Raw g is integer from -32768 to +32768. 
enum state {//Possible states of arduino board
  MEASURING,//Reads accelerometers, deduces acceleration value, at the end of measurement outputs mean
  WAITING,//Does polls events, don't know why, events are detected even without it
  SENDING,//Does nothing, so that indexCharacteristic callbacks could run unhindered, inSignal=2
  SENT,//Does nothing, inSignal=3
  //PREPARED,//Awaits for acceleration interrupt
};

void bleOutput(int);
void bleInput(int);
void setState(state S);
void sOut(int);
////////////////////
//MEASUREMENT TASK//
const int measurementRate=2e2;//Times per second
const int measurementStep=1e3/measurementRate;//Milliseconds
const int measurementDuration=1e3;//Milliseconds
const int measurementN=measurementDuration/measurementStep;//Size of a single measurement
const int Nmeasurements=5;//Amount of measurements stored on board
long int measurementPreviousTime=0;
long int measurementBeginning=0;
int shockTreshhold=3e3;//mg
int shockDuration=25;//ms

int measurementPos=0;
//int measurements[Nmeasurements][measurementN];
int **measurements;
//int temp[Nmeasurements][measurementN];
int* tmp;
int currentMeasurementPos=-1;
int *currentMeasurement;

static void accelerationEvent(){
  if (CurieIMU.getInterruptStatus(CURIE_IMU_SHOCK)) {
    setState(MEASURING);
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
  /* Enable Free Fall Detection */
  CurieIMU.setDetectionThreshold(CURIE_IMU_SHOCK, shockTreshhold); // 1g=1000mg
  CurieIMU.setDetectionDuration(CURIE_IMU_SHOCK, shockDuration);  // 50ms
  //for (int i=0; i<Nmeasurements; i++){
    ////measurements[i]=new int[measurementN];
    //measurements[i]=(int*)malloc (sizeof(int)*measurementN);
    //for (int j=0; j<measurementN; j++){
      //currentMeasurement[j]=0;
    //}
  //}
  measurements=(int**)malloc(sizeof(int*)*Nmeasurements);
  for (int j = 0; j < Nmeasurements; j++)
  {
    tmp=(int*)malloc (sizeof(int)*measurementN);
    for (int i = 0; i < measurementN; i++)
    {
      tmp[i]=0;
    }
    measurements[j]=tmp;
  }
}
void startMeasuring(){
  measurementBeginning=millis();
  measurementPreviousTime=millis();
  measurementPos=0;
  currentMeasurementPos++;
  if (currentMeasurementPos>=Nmeasurements){
    currentMeasurementPos=0;
  }
  currentMeasurement=&measurements[currentMeasurementPos][0];
  for (int i=0; i<measurementN; i++){
    currentMeasurement[i]=0;
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
    currentMeasurement[measurementPos]=a;
    measurementPos++;
  }
  digitalWrite(LED_PIN,1);
  //if ((measurementPos>=measurementN)||(currentTime-measurementBeginning >= measurementDuration)){
  if (measurementPos>=measurementN){
    //int mean=0;
    //for (int i=0;i<measurementN;i++){
      //mean+=currentMeasurement[i];
    //}
    bleOutput(currentMeasurementPos);
    bleInput(0);
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
BLEIntCharacteristic indexSignal = BLEIntCharacteristic("19B10003-E8F2-537E-4F6C-D104768AAD4",BLERead|BLEWrite);
BLEIntCharacteristic shockTreshholdSignal = BLEIntCharacteristic("19B10004-E8F2-537E-4F6C-D104768AAD4",BLERead|BLEWrite);
BLEIntCharacteristic durationTreshholdSignal = BLEIntCharacteristic("19B10005-E8F2-537E-4F6C-D104768AAD4",BLERead|BLEWrite);


void inSignalCallback(){
  glowBlink();
  switch (inSignal.value()){
    case 1: setState(MEASURING); longBlink(); break;
    case 2: setState(SENDING); break;
    //case 3: setState(PREPARED); break;
  }
}
void inSignalWritten(BLECentral& central, BLECharacteristic& characteristic){
  inSignalCallback();
}
void indexSignalCallback(){
  setState(SENDING);
  int ind=indexSignal.value();
  //Serial.write("Requested: ");
  //Serial.print(ind);
  //Serial.write("\n");
  //Serial.print(1);
  if (0<=ind && ind<measurementN){
    outSignal.setValue(currentMeasurement[ind]);
    setState(SENT);
    sOut(currentMeasurement[ind]);
    //bleInput(3);
  }else{
    setState(WAITING);
  }
}
void indexSignalWritten(BLECentral& central, BLECharacteristic& characteristic){
  indexSignalCallback();
}
void shockTreshholdSignalCallback(BLECentral& central, BLECharacteristic& characteristic){
  shockTreshhold =shockTreshholdSignal.value();
  CurieIMU.setDetectionThreshold(CURIE_IMU_SHOCK, shockTreshhold); // 1g=1000mg
}
void durationTreshholdSignalCallback(BLECentral& central, BLECharacteristic& characteristic){
  shockDuration =durationTreshholdSignal.value();
  CurieIMU.setDetectionDuration(CURIE_IMU_SHOCK, shockDuration);  // 50ms
}
void startSending(){
  bleInput(2);
}
void startSent(){
  bleInput(3);
}
void bleOutput(int a){
  outSignal.setValue(a);
}
void bleInput(int a){
  inSignal.setValue(a);
}

void bleInit(){
    // set advertised local name and service UUID
  blePeripheral.setLocalName("FINT");
  blePeripheral.setDeviceName("FINT_DEV");
  blePeripheral.setAdvertisedServiceUuid(mainService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(mainService);
  blePeripheral.addAttribute(inSignal);
  blePeripheral.addAttribute(indexSignal);
  blePeripheral.addAttribute(outSignal);
  blePeripheral.addAttribute(shockTreshholdSignal);
  blePeripheral.addAttribute(durationTreshholdSignal);
  
  // setting initial values so tey can be read correctly after start up
  inSignal.setValue(0);
  outSignal.setValue(0);
  indexSignal.setValue(-1);
  
  inSignal.setEventHandler(BLEWritten, inSignalWritten);
  indexSignal.setEventHandler(BLEWritten, indexSignalWritten);
  shockTreshholdSignal.setEventHandler(BLEWritten,shockTreshholdSignalCallback);
  durationTreshholdSignal.setEventHandler(BLEWritten,durationTreshholdSignalCallback);
  // begin initialization
  blePeripheral.begin();
}
//Serial communication
//void sIn(){
  
//}
void sOut(int res){
  Serial.print(res);
  Serial.print("\n");
}
void serialEvent(){
  int i=Serial.parseInt();
  //Serial.print("Received");
  //Serial.print(i);
  //Serial.print(int(in));
  //Serial.print("\n");
  if (i>-1){
    indexSignal.setValue(i);
    indexSignalCallback();
  }else if(i==-1){
    inSignal.setValue(1);
    inSignalCallback();
  }else if (i==-2){
    Serial.print("|");
    for (int j=0; j<measurementN; j++) {
      Serial.print(currentMeasurement[j]);
      Serial.print(" ");
    }
    Serial.print("|\n");
  }else if (i==-3){
    for (int j=0; j<Nmeasurements; j++){
      Serial.print("|");
      Serial.print(j);
      Serial.print("|");
      for (int k=0; k<measurementN; k++) {
        Serial.print(measurements[j][k]);
        Serial.print(" ");
      }
      Serial.print("|-|\n");
    }
  }else if(i==-4){
     Serial.println("Free memory: " + String(freeMemory()));
     Serial.println("Free stack: " + String(freeStack()));
     Serial.println("Free heap: " + String(freeHeap()));
  }
}
//blinkers
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
void longBlink(){
  //int prev=digitalRead(LED_BUILTIN);
  //for (int i = 0; i < 10; i++){
    //digitalWrite(LED_BUILTIN,0);
    //delay(100);
    //digitalWrite(LED_BUILTIN,1);
    //delay(100);
  //}
  //digitalWrite(LED_BUILTIN,prev);
}
void glowBlink(){
  //int prev=digitalRead(LED_BUILTIN);
  //digitalWrite(LED_BUILTIN,0);
  //delay(100);
  //digitalWrite(LED_BUILTIN,1);
  //delay(2000);
  //digitalWrite(LED_BUILTIN,prev);
}
void longShortBlink(){
  //int prev=digitalRead(LED_BUILTIN);
  //for (int i = 0; i < 5; i++){
    //digitalWrite(LED_BUILTIN,0);
    //delay(100);
    //digitalWrite(LED_BUILTIN,1);
    //delay(100);
    //digitalWrite(LED_BUILTIN,0);
    //delay(500);
    //digitalWrite(LED_BUILTIN,1);
    //delay(500);
  //}
  //digitalWrite(LED_BUILTIN,prev);
}

void startWaiting(){
  bleInput(0);
  CurieIMU.interrupts(CURIE_IMU_SHOCK);
}
void stopWaiting(){
  CurieIMU.noInterrupts(CURIE_IMU_SHOCK);
}
////////////////////////////
//DISPATCH TASK//
state BoardState = WAITING;
void setState(state S){
  //Serial.write("CHANGING STATE\n");
  longShortBlink();
  switch(S){
        case WAITING: startWaiting(); break;
        case MEASURING: stopWaiting(); startMeasuring(); break;//prepare values before measuring
        case SENDING: stopWaiting(); startSending(); break;
        case SENT: stopWaiting(); startSent(); break;
        //case PREPARED: startPrepared(); break;
      }
  BoardState=S;
  switch(S){
    case WAITING: Serial.write("WAITING\n"); break;
    case MEASURING: Serial.write("MEASURING\n"); break;
    case SENDING: Serial.write("SENDING\n"); break;
    case SENT: Serial.write("SENT\n"); break; 
    //case PREPARED: Serial.write("PREPARED\n"); break;
  };
}
void update(){
  //Serial.write("UPDATING\n");
  switch(BoardState){
    case WAITING: {
      //Serial.write("WAITING\n"); 
      blePeripheral.poll(); 
      //Serial.print("beep");
      break;
    };
    case MEASURING: {
      //Serial.write("MEASURING\n"); 
      measure(); 
      break;
    };
    case SENDING: {
      //Serial.write("SENDING\n"); 
      break;
    };
    case SENT:{
      break;
    }
  };
}
////////////////////////////////////////////////////////////////////////
void setup() {  
  //initAccelerometers();
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,1);
  
  bleInit();
  measurementsInit();
  
  setState(WAITING);
  longBlink();
  //Serial.write("READY\n");
}

void loop() {
  //Serial.write("LOOP\n");
  blink();
  update();
}
