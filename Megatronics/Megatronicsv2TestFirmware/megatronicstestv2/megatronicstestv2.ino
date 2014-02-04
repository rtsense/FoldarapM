#include "thermistortables.h"

//uncomment to test the SD card
//#define TESTSD 1

const int chipSelect = 53;    
#include <SD.h>
#include <LiquidCrystal.h>


Sd2Card card;
SdVolume volume;
File root;
#define BEEPER         64



#define X_STEP_PIN         26
#define X_DIR_PIN          27
#define X_ENABLE_PIN       25
#define X_MIN_PIN          37
#define X_MAX_PIN          40   //2 //Max endstops default to disabled "-1", set to commented value to enable.

#define Y_STEP_PIN         4 // A6
#define Y_DIR_PIN          54 // A0
#define Y_ENABLE_PIN       5
#define Y_MIN_PIN          41
#define Y_MAX_PIN          38   //15

#define Z_STEP_PIN         56 // A2
#define Z_DIR_PIN          60 // A6
#define Z_ENABLE_PIN       55 // A1
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define E0_STEP_PIN         35
#define E0_DIR_PIN          36
#define E0_ENABLE_PIN       34

#define E1_STEP_PIN         29
#define E1_DIR_PIN          39
#define E1_ENABLE_PIN       28

#define E2_STEP_PIN         23
#define E2_DIR_PIN          24
#define E2_ENABLE_PIN       22


#define SDPOWER            -1

#define EXTRUDERS 3

#define TEMP_SENSOR_AD595_OFFSET 0.0
#define TEMP_SENSOR_AD595_GAIN   1.0

#define THERMISTORHEATER_0 1
#define THERMISTORHEATER_1 1
#define THERMISTORHEATER_2 1

#define HEATER_0_USES_THERMISTOR 1
#define HEATER_1_USES_THERMISTOR 1
#define HEATER_2_USES_THERMISTOR 1
#define HEATER_3_USES_THERMISTOR 1

  static void *heater_ttbl_map[EXTRUDERS] = { (void *)heater_0_temptable
#if EXTRUDERS > 1
                                            , (void *)heater_1_temptable
#endif
#if EXTRUDERS > 2
                                            , (void *)heater_2_temptable
#endif
#if EXTRUDERS > 3
  #error Unsupported number of extruders
#endif
  };
  
    static int heater_ttbllen_map[EXTRUDERS] = { heater_0_temptable_len
#if EXTRUDERS > 1
                                             , heater_1_temptable_len
#endif
#if EXTRUDERS > 2
                                             , heater_2_temptable_len
#endif
#if EXTRUDERS > 3
  #error Unsupported number of extruders
#endif
  };
  
  #define PGM_RD_W(x)   (short)pgm_read_word(&x)

#define SDSS               53
#define LED_PIN            13


#define LCD_PINS_RS 14 
#define LCD_PINS_ENABLE 15
#define LCD_PINS_D4 30
#define LCD_PINS_D5 31 
#define LCD_PINS_D6 32
#define LCD_PINS_D7 33


#define PS_ON_PIN          12
#define KILL_PIN           -1


#define FAN_PIN            6
#define FAN2_PIN            7
#define HEATER_0_PIN       8
#define HEATER_1_PIN       9
#define HEATER_2_PIN       10



#define TEMP_0_PIN         15   // ANALOG NUMBERING
#define TEMP_1_PIN         14   // ANALOG NUMBERING
#define TEMP_2_PIN         13   // ANALOG NUMBERING
#define TEMP_3_PIN         4  // ANALOG NUMBERING
#define TEMP_4_PIN         8  // ANALOG NUMBERING

#define  c     3830    // 261 Hz
#define  d     3400    // 294 Hz
#define  e     3038    // 329 Hz
#define  f     2864    // 349 Hz
#define  g     2550    // 392 Hz
#define  a     2272    // 440 Hz
#define  b     2028    // 493 Hz
#define  C     1912    // 523 Hz 
#define  R     0
int melody[] = {  C,  b,  g,  C,  b,   e,  R,  C,  c,  g, a, C };
int beats[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 };
int MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration  = 0;
LiquidCrystal lcd(LCD_PINS_RS, LCD_PINS_ENABLE, LCD_PINS_D4, LCD_PINS_D5,LCD_PINS_D6,LCD_PINS_D7);

void playTone() {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      digitalWrite(BEEPER,HIGH);
      delayMicroseconds(tone_ / 2);

      // DOWN
      digitalWrite(BEEPER, LOW);
      delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    }
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                
}

void setup() {
  pinMode(BEEPER, OUTPUT);

  
  pinMode(FAN_PIN , OUTPUT);
  pinMode(FAN2_PIN , OUTPUT);
  pinMode(HEATER_0_PIN , OUTPUT);
  pinMode(HEATER_1_PIN , OUTPUT);
  pinMode(LED_PIN  , OUTPUT);
  
  pinMode(X_STEP_PIN  , OUTPUT);
  pinMode(X_DIR_PIN    , OUTPUT);
  pinMode(X_ENABLE_PIN    , OUTPUT);
  
  pinMode(Y_STEP_PIN  , OUTPUT);
  pinMode(Y_DIR_PIN    , OUTPUT);
  pinMode(Y_ENABLE_PIN    , OUTPUT);
  
  pinMode(Z_STEP_PIN  , OUTPUT);
  pinMode(Z_DIR_PIN    , OUTPUT);
  pinMode(Z_ENABLE_PIN    , OUTPUT);
  
  pinMode(E0_STEP_PIN  , OUTPUT);
  pinMode(E0_DIR_PIN    , OUTPUT);
  pinMode(E0_ENABLE_PIN    , OUTPUT);
  
  pinMode(E1_STEP_PIN  , OUTPUT);
  pinMode(E1_DIR_PIN    , OUTPUT);
  pinMode(E1_ENABLE_PIN    , OUTPUT);
  
  pinMode(E2_STEP_PIN  , OUTPUT);
  pinMode(E2_DIR_PIN    , OUTPUT);
  pinMode(E2_ENABLE_PIN    , OUTPUT);
  
  
  digitalWrite(X_ENABLE_PIN    , LOW);
  digitalWrite(Y_ENABLE_PIN    , LOW);
  digitalWrite(Z_ENABLE_PIN    , LOW);
  digitalWrite(E0_ENABLE_PIN    , LOW);
  digitalWrite(E1_ENABLE_PIN    , LOW);
  digitalWrite(E2_ENABLE_PIN    , LOW);
  Serial.begin(115200);
  #ifdef TESTSD
  
  Serial.print("\nInitializing SD card...");

   pinMode(53, OUTPUT);
   pinMode(48, OUTPUT);  
   digitalWrite(48,HIGH);
   pinMode(SPI_MISO_PIN, INPUT);
   pinMode(SPI_MOSI_PIN, OUTPUT);
   pinMode(SPI_SCK_PIN, OUTPUT);
   
   digitalWrite(SPI_MOSI_PIN,LOW);
   digitalWrite(SPI_SCK_PIN,LOW);
   digitalWrite(53,LOW);
   delay(1);
   

   SD.begin(53);
  #endif
  
  
  lcd.begin(20, 4);
  lcd.clear();
  
  tone_ = melody[0];
  beat = beats[0];
  
  duration = 30; // Set up timing
  
  playTone();
  delayMicroseconds(1);
  playTone();
  delayMicroseconds(1);
  playTone();

}



float analog2temp(int raw, uint8_t e2) {

  #ifdef HEATER_0_USES_MAX6675
    if (e2 == 0)
    {
      return 0.25 * raw;
    }
  #endif

  if(heater_ttbl_map[e2] != 0)
  {
    float celsius = 0;
    byte i;  
    short (*tt)[][2] = (short (*)[][2])(heater_ttbl_map[e]);

    raw = (1023 * OVERSAMPLENR) - raw;
    for (i=1; i<heater_ttbllen_map[e]; i++)
    {
      if ((PGM_RD_W((*tt)[i][0]) > raw) && ((float)(PGM_RD_W((*tt)[i][0]) - PGM_RD_W((*tt)[i-1][0])) >0))
      {
        celsius = PGM_RD_W((*tt)[i-1][1]) + 
          (raw - PGM_RD_W((*tt)[i-1][0])) * 
          (float)(PGM_RD_W((*tt)[i][1]) - PGM_RD_W((*tt)[i-1][1])) /
          (float)(PGM_RD_W((*tt)[i][0]) - PGM_RD_W((*tt)[i-1][0]));
        break;
      }
    }

    // Overflow: Set to last value in the table
    if (i == heater_ttbllen_map[e]) celsius = PGM_RD_W((*tt)[i-1][1]);

    return celsius;
  }
  return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
}

unsigned long prevMillis;

void loop () {
  

  
  if (millis() %1000 <500) 
    digitalWrite(LED_PIN, HIGH);
  else
   digitalWrite(LED_PIN, LOW);
  
  if (millis() %1000 <200) {
    
  
    digitalWrite(HEATER_0_PIN, LOW);
    digitalWrite(HEATER_1_PIN, LOW);
    digitalWrite(HEATER_2_PIN, LOW);
    digitalWrite(FAN_PIN, HIGH);
    digitalWrite(FAN2_PIN, LOW);
  } else if (millis() %1000 <400) {
    digitalWrite(HEATER_0_PIN, LOW);
    digitalWrite(HEATER_1_PIN, LOW);
    digitalWrite(HEATER_2_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(FAN2_PIN, HIGH);
   } else if (millis() %1000 <600) {
    digitalWrite(HEATER_0_PIN, HIGH);
    digitalWrite(HEATER_1_PIN, LOW);
    digitalWrite(HEATER_2_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(FAN2_PIN, LOW);
   } else if (millis() %1000 <800) {
    digitalWrite(HEATER_0_PIN, LOW);
    digitalWrite(HEATER_1_PIN, HIGH);
    digitalWrite(HEATER_2_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(FAN2_PIN, LOW);
  } else  {
    digitalWrite(HEATER_0_PIN, LOW);
    digitalWrite(HEATER_1_PIN, LOW);
      digitalWrite(HEATER_2_PIN, HIGH);    
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(FAN2_PIN, LOW);
  }

  
  if (millis() %2000 <1000) {
    digitalWrite(X_DIR_PIN    , HIGH);
    digitalWrite(Y_DIR_PIN    , HIGH);
    digitalWrite(Z_DIR_PIN    , HIGH);
    digitalWrite(E0_DIR_PIN    , HIGH);
    digitalWrite(E1_DIR_PIN    , HIGH);
    digitalWrite(E2_DIR_PIN    , HIGH);
  }
  else {
    digitalWrite(X_DIR_PIN    , LOW);
    digitalWrite(Y_DIR_PIN    , LOW);
    digitalWrite(Z_DIR_PIN    , LOW);
    digitalWrite(E0_DIR_PIN    , LOW);
    digitalWrite(E1_DIR_PIN    , LOW);
    digitalWrite(E2_DIR_PIN    , LOW);
  }
  
  
    digitalWrite(X_STEP_PIN    , HIGH);
    digitalWrite(Y_STEP_PIN    , HIGH);
    digitalWrite(Z_STEP_PIN    , HIGH);
    digitalWrite(E0_STEP_PIN    , HIGH);
    digitalWrite(E1_STEP_PIN    , HIGH);
    digitalWrite(E2_STEP_PIN    , HIGH);
  delay(1);
  
  
  
    
    digitalWrite(X_STEP_PIN    , LOW);
    digitalWrite(Y_STEP_PIN    , LOW);
    digitalWrite(Z_STEP_PIN    , LOW);
    digitalWrite(E0_STEP_PIN    , LOW);
    digitalWrite(E1_STEP_PIN    , LOW);
    digitalWrite(E2_STEP_PIN    , LOW);
    
    if (millis() -prevMillis >100){
    prevMillis=millis();
    

    

    int t = analogRead( TEMP_0_PIN);
    Serial.print("T1 ");
    Serial.print(t);
    Serial.print("/");
    Serial.print(analog2temp(1024 - t,0),0);
    
    lcd.setCursor(0, 0);
    lcd.print("T1 ");
    lcd.print(t);
    lcd.print("  ");
    
    Serial.print(" T2 ");
    t = analogRead( TEMP_1_PIN);
    Serial.print(t);
    Serial.print("/");
    Serial.print(analog2temp(1024 - t,1),0);
    
    lcd.setCursor(10, 0);
    lcd.print("T2 ");
    lcd.print(t);
    lcd.print("  ");
    
    Serial.print(" T3 ");
    t = analogRead( TEMP_2_PIN);
    Serial.print(t);
    Serial.print("/");
    Serial.print(analog2temp(1024 - t,2),0);
    
    lcd.setCursor(0, 1);
    lcd.print("T3 ");
    lcd.print(t);
    lcd.print("  ");
 
    Serial.print(" TC ");
    t = analogRead( TEMP_3_PIN);
    Serial.print(t);
    Serial.print("/");
    Serial.print(analog2temp(1024 - t,2),0);   
    
    lcd.setCursor(10, 1);
    lcd.print("TC ");
    lcd.print(t);
    lcd.print("  ");
    
        Serial.print(" TC2 ");
    t = analogRead( TEMP_4_PIN);
    Serial.print(t);
    Serial.print("/");
    Serial.println(analog2temp(1024 - t,2),0);   
    
    lcd.setCursor(10, 2);
    lcd.print("TC2 ");
    lcd.print(t);
    lcd.print("  ");
    
    #ifdef TESTSD
    
    if (!card.init(SPI_FULL_SPEED, chipSelect)) {
     Serial.println("SD Init failed");
     lcd.setCursor(0, 2);
     lcd.print("SD FAILED");
    }
    else {
     if (!volume.init(card)) {
      Serial.println("SD Init failed");
      lcd.setCursor(0, 2);
      lcd.print("SD FAILED");
     }
     else {
      Serial.println("Files found");
      lcd.setCursor(0, 2);
      lcd.print("SD OK    ");
     }
    }
    
    #endif

    lcd.setCursor(0, 3);
    lcd.print("Time: ");
    lcd.print(millis()/1000);
}



}
