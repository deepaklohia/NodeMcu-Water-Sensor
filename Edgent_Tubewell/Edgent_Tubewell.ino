#define BLYNK_TEMPLATE_ID "your id"
#define BLYNK_TEMPLATE_NAME "Tubewell"
#define BLYNK_FIRMWARE_VERSION  "0.0.6"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

//#define APP_DEBUG

/**********CHANGES********
27-June-2024 -- changed Sv arr to 10

*/

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"

#define SSR_ON HIGH
#define SSR_OFF LOW
#define ER_ON LOW
#define ER_OFF HIGH

//NODE MCU
static const uint8_t D0 = 16; //—— HIGH ON BOOT
static const uint8_t D1 = 5;  //SAFE PIN - LOW ON BOOT 
static const uint8_t D2 = 4;  //SAFE PIN - LOW ON BOOT
static const uint8_t D3 = 0;  //—— HIGH ON BOOT
static const uint8_t D4 = 2;  //—— HIGH ON BOOT, FAIURE IF PULLED LOW ON BOOT
static const uint8_t D5 = 14; //SAFE PIN - HIGH ON BOOT ??
static const uint8_t D6 = 12; //SAFE PIN - HIGH ON BOOT
static const uint8_t D7 = 13; //SAFE PIN - HIGH ON BOOT
static const uint8_t D8 = 15; //—— LOW ON BOOT ??, FAILURE IF PULLED HIGH ON BOOT
static const uint8_t RX = 3;  //—— HIGH ON BOOT
static const uint8_t TX = 1;  //—— HIGH ON BOOT, FAIURE IF PULLED LOW ON BOOT
static const uint8_t SD2 = 9; //—— HIGH ON BOOT
static const uint8_t SD3 = 10;//—— HIGH ON BOOT

const int buff_size_sv = 10; //taking last 12 values of the SV to determine if there is water
long arr_sv[buff_size_sv];

int sensorLimitRW = 860;
int sensorValueRW = 0 ;
bool motorFlag = false;
String txt;

unsigned long tme ;
unsigned long seconds ;
unsigned long minutes ;
unsigned long hours ;
unsigned long days ;

const int sensorPin = A0;
const int pumpRelay = D1;
const int startRelay = D2 ;
const int pullupRelay = D0;
//const int pullupRelay = D8;

const int tank_level1_pullup = D5;
const int tank_level2_pullup = D6;
const int tank_level3_pullup = D7;
bool tankStatusFirst ;
bool tankStatusLast ;
int tankVal;
bool tempTankLevelOff;
bool deepSleep;

unsigned long startMilliSec;
unsigned long editModeTimeout = 10000 ;
unsigned long runningWaterWait = 300000 ; //5 min

unsigned long waitBeforeOff_RW ; //15 Sec before off
int waitBeforeOffSec_RW ;

unsigned long cdnTime = 30UL * 60UL * 1000UL;  //SET 30 MIN DEFAULT
int timeCountdown ;

unsigned long timeRemaining ;
unsigned long endMilliSec ;
unsigned long balMilliSec ;

bool runningWater ;
bool powerStarted ;
bool pumpStarted;
bool pumpFlag ;

String rslt;
String strRW;
String timeStamp;
bool timeUpdateFlag;
int waitForUpdate;

BLYNK_CONNECTED(){
  Blynk.syncVirtual(V1);  // pump status
  Blynk.syncVirtual(V2);  // SV Limit
  Blynk.syncVirtual(V3);  // in water reset
  Blynk.syncVirtual(V4);  // tank level
  Blynk.syncVirtual(V5);  // status
  Blynk.syncVirtual(V6);  // Deep Sleep  
  Blynk.syncVirtual(V7);  // time countdown
  Blynk.syncVirtual(V8);  // wait before RW off
  Blynk.sendInternal("utc", "iso");  // ISO-8601 formatted time
  //Blynk.virtualWrite(V4,  tankVal );
}

//event when value changes
BLYNK_WRITE(V2){
  int value = param.asInt();
  sensorLimitRW = value;  
}

//event when value changes
BLYNK_WRITE(V6){
  deepSleep = param.asInt();
}

//time countdown
BLYNK_WRITE(V7){
  timeCountdown = param.asInt();
  cdnTime  = timeCountdown * 60UL * 1000UL ;  // in min
}

//wait before off RW
BLYNK_WRITE(V8){
  waitBeforeOffSec_RW = param.asInt();
  waitBeforeOff_RW  = waitBeforeOffSec_RW * 1000UL ;  // in sec
}

// Receive UTC data
BLYNK_WRITE(InternalPinUTC) {
    String cmd = param[0].asStr();
    if (cmd == "tz_name") {
      String tz_name = param[1].asStr();
      //printV5("Timezone: " +tz_name);
    }
    else if (cmd == "iso") {
      String iso_time = param[1].asStr();
      //printV5("ISO-8601 time:   "+ iso_time);

      int pos = iso_time.indexOf("T") ;
      String raw = iso_time.substring(0, pos ); //getting just date data

      pos = raw.indexOf("-") ; //getting position of the -
      String year = raw.substring(0,pos);

      raw = raw.substring( pos + 1 , raw.length() ); //removing year info in new raw string
      pos = raw.indexOf("-") ;
      String month = raw.substring(0,pos);
      String day = raw.substring( pos + 1 );

      //getting time 
      iso_time = param[1].asStr();
      raw = iso_time.substring(iso_time.indexOf("T") + 1, iso_time.indexOf("+") ); //getting time
      String hh = raw.substring(0,2);
      String mm = raw.substring(5,3);

      int hr_24  ;
      int hr_12 ;
      String AMPM ;

      hr_24 = hh.toInt();
      if (hr_24==0) hh=12;
      else hh=hr_24%12;
      if (hr_24<12) AMPM = "AM";
      else AMPM = "PM";

      txt = hh + ":" + mm + AMPM;
      year = year.substring(2,4);
      timeStamp = day + "-" + getMonthStr(month.toInt()) + "-" + year + " [" + txt + "]";
      //timeStamp = day + "-" + getMonthStr(month.toInt()) + " [" + txt + "]";
    }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  Blynk.virtualWrite(V1, 0); //pump status
  Blynk.virtualWrite(V3, 0); //in water reset
  Blynk.virtualWrite(V4, 0); //tank level reset
  Blynk.virtualWrite(V5, ""); //
  //Blynk.sendInternal("utc", "iso");  // ISO-8601 formatted time

  printV5 ("JAI GURU DEV") ;
  printV5 ("JAI SHIV SHANKAR") ;
  printV5 ("DESIGNED BY DEEPAK LOHIA") ;

  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  pinMode(pumpRelay, OUTPUT) ;
  pinMode(startRelay, OUTPUT) ;
  pinMode(pullupRelay, OUTPUT) ;
  pinMode(tank_level1_pullup , INPUT_PULLUP) ;
  pinMode(tank_level2_pullup , INPUT_PULLUP) ;
  pinMode(tank_level3_pullup , INPUT_PULLUP) ;

  //relay power
  digitalWrite(pumpRelay , SSR_OFF) ;
  digitalWrite(startRelay , ER_OFF) ;

  startMilliSec = millis() ;
}

void loop() {
  BlynkEdgent.run();

  //TUBEWELL WORK DONE UPDATE LEVEL
  if (!tankStatusFirst){
    setTankLevel();
    tankStatusFirst = true;
  }

  //first water run to ensure water flow, till the water flow starts we put X
  if (!runningWater){
    if ( (millis()  -  startMilliSec) >= runningWaterWait ){
      runningWater  = true ;
    }
  }

  if (!powerStarted){
    digitalWrite(pumpRelay, SSR_ON); //START MOTOR 1
    txt = "POWER ON";
    printV5(txt);
    powerStarted = true;
  }
  else if (powerStarted && !pumpStarted){
    //funTestRelay();
    digitalWrite(startRelay, ER_ON);
    delay(800);
    digitalWrite(startRelay, ER_OFF);
    pumpStarted = true;
    txt = "PUMP ON" ;
    printV5(txt);
    pumpFlag = true;
    Blynk.virtualWrite(V1, 1);
  }
  
  //if pump is ON
  else if (pumpFlag){
    sensorValueRW = get_avg_sv(analogRead(sensorPin));
    //IF TANK FULL OFF POWER
    if (tempTankLevelOff && digitalRead(tank_level3_pullup) == 0){
      txt = "TANK FULL WAITING...30 sec" ;
      printV5(txt) ;
      delay(30000); //wait 30 secs
      if (digitalRead(tank_level3_pullup) == 0){
        digitalWrite(pumpRelay , SSR_OFF) ;
        pumpFlag = false;
        rslt =  "SNSR OFF SV:" + String(sensorValueRW) + " " + getRunTime() ;
        printV5(rslt) ;
      }
    }
    //IF NO WATER OFF
    else if (sensorValueRW > sensorLimitRW && runningWater){
      txt = "NO WTR SV:" + String(sensorValueRW) + String(waitBeforeOff_RW / 1000) + "Secs" ;
      printV5(txt) ;
      delay(waitBeforeOff_RW); 
      sensorValueRW = get_avg_sv(analogRead(sensorPin));
      if (sensorValueRW > sensorLimitRW){
        digitalWrite(pumpRelay , SSR_OFF) ;
        pumpFlag = false ;
        rslt =  "FRC OFF SV:" + String(sensorValueRW) + " " + getRunTime() ;
        printV5(rslt) ;
      }
    }

    //IF TIME OVER OFF
    else if ((millis()  -  startMilliSec) >= cdnTime ){
      digitalWrite(pumpRelay , SSR_OFF) ;
      pumpFlag = false;
      rslt =  "CNDN OV OFF SV:" + String(sensorValueRW) + " " + getRunTime() ;
      printV5(rslt) ;
    }
    
    timeRemaining  = startMilliSec + millis() ;  //IN MILLISECS
    tme = timeRemaining;
    seconds = tme / 1000;
    minutes = seconds / 60;
    hours = minutes / 60;
    days = hours / 24;

    tme %= 1000;
    seconds %= 60;
    minutes %= 60;
    hours %= 24;
  
    if (!runningWater){  strRW = "X";}
    else{  strRW = "";   }
   
    txt  = padZero(hours) + ":" + padZero(minutes) + ":" + padZero(seconds)  + " << " + String(   (cdnTime / 60)  / 1000  ) + "M "  ;
    txt =  txt + "RW-" + String(sensorValueRW) + strRW + " [LMT:" + String(sensorLimitRW) + "]";
    printV5(txt) ;

    //UPDATING BLYNK STATUS
    if (pumpFlag){  Blynk.virtualWrite(V1, 1);
    } else{    Blynk.virtualWrite(V1, 0);  }  
    Blynk.virtualWrite(V3, sensorValueRW); //UPDATING M2 SENSOR VALUE
  }
  //IF PUMP IS OFF
  else{
    //TUBEWELL WORK DONE UPDATE LEVEL
    if (!tankStatusLast && waitForUpdate >=7){
      setTankLevel();
      tankStatusLast = true;
    }
    //after 10 rounds sleep
    else if(waitForUpdate > 15){
      if (deepSleep) {
        //ESP.deepSleep(0);
        ESP.deepSleep(ESP.deepSleepMax()); //does it create problem in waking up ??
      }
    }
    printV5(rslt);

    delay(1000);
    waitForUpdate += 1;
  }
}

void printV5(String txt){
  Blynk.virtualWrite(V5, txt); 
  delay(1000);
}

void setTankLevel(){
  digitalWrite(pullupRelay, ER_ON);
  delay(2000);
  if (digitalRead(tank_level3_pullup) == 0 ){  tankVal = 45; }
  else if (digitalRead(tank_level2_pullup) == 0 ){  tankVal = 30;  }
  else if (digitalRead(tank_level1_pullup) == 0 ){  tankVal = 15;}
  else {  tankVal = 5;}
  //updating tank status
  Blynk.virtualWrite(V4,  tankVal );
  //tank_level = tankVal;
  digitalWrite(pullupRelay, ER_OFF);
  delay(500);
}

/* we are taking last x values of the SV value to see the actual status of SV */
int get_avg_sv(int val_sv)
{
  long total = 0;
  byte count = 0;
  static byte i_sv = 0;
 
  arr_sv[i_sv++] = val_sv;
  if (i_sv == buff_size_sv){ i_sv = 0;}
  
  while (count < buff_size_sv) {
    total = total + arr_sv[i_sv++];
    if (i_sv == buff_size_sv){i_sv = 0;}
    count++;
  }
  return total/buff_size_sv;
}

String padZero(int val){
  if (val < 10){ return "0" + String(val);}
  else{  return String(val); }
}

String getRunTime(){
  endMilliSec = millis();
  balMilliSec = endMilliSec - startMilliSec ;
  tme = balMilliSec;
  seconds = tme / 1000;
  minutes = seconds / 60;
  hours = minutes / 60;
  days = hours / 24;
  tme %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  return  "RAN FOR:" + padZero(hours) + ":" + padZero(minutes) + ":" + padZero(seconds)   ;
}

String getMonthStr(int m){
  String arr[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  return arr[m-1];
}

