//Current running version
//Threshold acceleration detection
//Now with beeper
//Measurements are in the buffer
//Reduced memory usage
//-3 on serial gets all data from the buffer
//Read gyro as well TODO
#include <CurieBLE.h>
#include <CurieIMU.h>
#include <CurieTime.h>
#include <MemoryFree.h>
#include <limits>


#define LED_PIN  LED_BUILTIN
#define GYRO_RANGE 500 //rad/sec
#define ACCEL_RANGE 16 //Range of accelerometer in g. Raw g is integer from -32768 to +32768. 
#define INT uint8_t
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
const short BEEPER=12;
const short measurementRate=2e2;//Times per second
const short measurementStep=1e3/measurementRate;//Milliseconds
const short measurementDuration=1e3;//Milliseconds
const short measurementN=measurementDuration/measurementStep;//Size of a single measurement
const short Nmeasurements=10;//Amount of measurements stored on board
long int measurementPreviousTime=0;
long int measurementBeginning=0;
int shockTreshhold=3e3;//mg
int shockDuration=25;//ms

int measurementPos=0;
INT measurements[Nmeasurements][measurementN];
//INT **measurements;
//int temp[Nmeasurements][measurementN];
//INT *tmp;
uint16_t currentMeasurementPos=-1;
INT *currentMeasurement;

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
  for (int i=0; i<Nmeasurements; i++){
    for (int j = 0; j < measurementN; j++)
    {
      measurements[i][j]=0;
    }
  }
  
  //measurements=(uint16_t**)malloc(sizeof(uint16_t*)*Nmeasurements);
  ////for (int j = 0; j < Nmeasurements; j++){
    //while (!tmp){
      //tmp=(INT*)malloc (sizeof(INT)*measurementN*Nmeasurements);
      //if (tmp==NULL){
        //Nmeasurements=Nmeasurements-1;
      //}
    //}
    //for (int i = 0; i < measurementN; i++)
    //{
      //tmp[i]=0;
    //}
    

    //for (int i = 0; i < Nmeasurements; i++)
    //{
      //measurements[i]=&(tmp[i*measurementN]);
    //}
    
    //measurements[j]=tmp;
  //}
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
  digitalWrite(BEEPER,1);
}
long map(int64_t x, int64_t in_min, int64_t in_max, int64_t out_min, int64_t out_max)
{
  return (x - in_min)  / (in_max - in_min) * (out_max - out_min) + out_min;
}

void measure(){
  long int currentTime=millis();
  if (currentTime-measurementPreviousTime >= measurementStep){
    uint32_t a=0;
    int ax,ay,az;
    INT m;
    CurieIMU.readAccelerometer(ax,ay,az);
    a=ax*ax+ay*ay+az*az;
    #undef min
    #undef max
    m=map(a,
      //std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(),
      0, std::numeric_limits<uint32_t>::max(),
      //std::numeric_limits<INT>::min(),  std::numeric_limits<INT>::max()
      0,  std::numeric_limits<INT>::max()
      );
    //Serial.print("mapping ");
    Serial.print(a);
    Serial.print("  ");
    Serial.print(m);
    Serial.print("\n");
    currentMeasurement[measurementPos]=m;
    measurementPreviousTime=currentTime;
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
BLEUnsignedLongCharacteristic packedoutSignal = BLEUnsignedLongCharacteristic("19B10004-E8F2-537E-4F6C-D104768AAD4",BLERead);


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
    unsigned long res=0;
    short n=sizeof(long)/sizeof(INT);
    for (int i = ind; (i <measurementN && i < ind+n); i++){
      res= res | currentMeasurement[i];
      res =res << sizeof(INT);
      Serial.print(currentMeasurement[i]);
      Serial.print(" ");
    }
    Serial.print("|");
    Serial.print(res);
    Serial.print("\n");
    //outSignal.setValue(sizeof(INT));
    int o=0;
    o+=currentMeasurement[ind];
    outSignal.setValue(o);
    packedoutSignal.setValue(res);
    setState(SENT);
    //sOut(currentMeasurement[ind]);
    //bleInput(3);
  }else{
    setState(WAITING);
  }
}
void indexSignalWritten(BLECentral& central, BLECharacteristic& characteristic){
  indexSignalCallback();
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
  blePeripheral.addAttribute(packedoutSignal);
  
  // setting initial values so tey can be read correctly after start up
  inSignal.setValue(0);
  outSignal.setValue(0);
  indexSignal.setValue(-1);
  
  inSignal.setEventHandler(BLEWritten, inSignalWritten);
  indexSignal.setEventHandler(BLEWritten, indexSignalWritten);
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
  digitalWrite(BEEPER,0);
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
  pinMode(BEEPER,OUTPUT);
  digitalWrite(LED_PIN,1);
  
  bleInit();
  measurementsInit();
  
  setState(WAITING);
  longBlink();
  digitalWrite(BEEPER,1);
  delay(500);
  digitalWrite(BEEPER,0);
  //Serial.write("READY\n");
}

void loop() {
  //Serial.write("LOOP\n");
  blink();
  update();
}
