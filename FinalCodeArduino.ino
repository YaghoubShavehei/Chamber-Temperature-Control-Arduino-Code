#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
//#include <ezButton.h>

#define THERMISTOR_PIN A0 // Analog pin connected to the thermistor
#define SERIES_RESISTOR 100000.0 // Resistance value of the series resistor (in ohms)
#define NOMINAL_RESISTANCE 100000.0 // Nominal resistance of the thermistor at 25 degrees Celsius (in ohms)
#define TEMPERATURE_NOMINAL 25.0 // Temperature at nominal resistance (in degrees Celsius)
#define B_COEFFICIENT 3850.0 // Beta coefficient of the thermistor

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ROTARY_PIN1 2 // CLK pin of rotary encoder
#define ROTARY_PIN2 3 // DT pin of rotary encoder
#define BUTTON_PIN 4  // Switch pin of rotary encoder
#define BUZZER_PIN 5 // buzzer pin for beep sound alarm
#define HEATER_PIN 6 // heater pin for on and off quartz heater SSR
#define FAN_PIN_1 7 // SSR pin for fan 1 (to be replaced with correct pin number)
#define FAN_PIN_2 8 // SSR pin for fan 2 (to be replaced with correct pin number)
// #define MICRO_PIN 9 // Micro Switch Pin for Door Opeiing mode 

//ezButton MicroSwitch(MICRO_PIN);
volatile bool isFan1On = false;
volatile bool MenuStatus = false;
volatile bool ItemStatus = false;
volatile bool buttonPressed = false;
volatile int lastButtonState = LOW;
volatile int buttonState;
unsigned long lastDebounceTime = 0;
unsigned long WaitTime =0;
unsigned long FanTime =0;
unsigned long debounceDelay =50;

int MenuItem =1;
int lastRotaryState;
int rotaryState;
int CurrentTemp=80;
int TempLimit=5;
int FanSwitchLimit=60;
int rotaryPosition=1;
int ChambTemp=0;
unsigned int MinToMili=0;
int MicroState;
void setup() {
    Serial.begin(9600);
    attachInterrupt(digitalPinToInterrupt(ROTARY_PIN1), EncoderRotate, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ROTARY_PIN2), EncoderRotate, CHANGE);
//    tone(8, 700, 2000);
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(FAN_PIN_1, OUTPUT);
    pinMode(FAN_PIN_2, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
//    pinMode(MICRO_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
//    MicroSwitch.setDebounceTime(50);
//    pinMode(8, OUTPUT);
    // Show image buffer on the display hardware.
    // Since the buffer is intialized with an Adafruit splashscreen
    // internally, this will display the splashscreen.
    delay(250); // wait for the OLED to power up
    display.begin(i2c_Address, true); // Address 0x3C default
    //display.setContrast (0); // dim display
    display.display();
    delay(10);
//    StartText();
//    delay(2000);
//    display.clearDisplay();
}

void loop() {
  PushButton();
  // Tempreture detect and turn on Heater_SSR
  ChambTemp=Temprature();
  if ((CurrentTemp-TempLimit)>ChambTemp){
    digitalWrite(HEATER_PIN,HIGH);
  }
  if ((CurrentTemp+TempLimit)<ChambTemp){
    digitalWrite(HEATER_PIN,LOW);
  }
  //Fan Switch Parts:
  if ((millis()-FanTime)>(FanSwitchLimit*MinToMili)){
    switchFans();
    FanTime=millis();
    MinToMili=60000;
  }
  
  //Micro switch start and stop parts 
//  MicroState=digitalRead(MICRO_PIN);
  
  //sensor failure detect and off Automatically
  if ((130 < ChambTemp) || (15 > ChambTemp)){
    digitalWrite(HEATER_PIN,LOW);
    digitalWrite(FAN_PIN_1,LOW);
    digitalWrite(FAN_PIN_2,LOW);
    digitalWrite(BUZZER_PIN,HIGH);
  }
  OledMenu();
}

// first function for push button reaction in multipe pages.
void PushButton(){
  if ((millis()-WaitTime)>10000 && MenuStatus){
    MenuStatus=false;
    ItemStatus=false;
//    Serial.println("First page");
  }
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        buttonPressed = true;
      }
    }
  }

  lastButtonState = reading;
  if (buttonPressed) {
//  Serial.println("Button pressed!");
  WaitTime=millis();
  buttonPressed = false;
  if (!MenuStatus){
    MenuStatus=true;
    MenuItem=1;
//    Serial.println("main menu");
  } else if (MenuStatus && !ItemStatus){
   ItemStatus=true;
   if (MenuItem==4){
     CurrentTemp=80;
     TempLimit=5;
     FanSwitchLimit=60;
   }
//   Serial.println("second menu");
  }else if (MenuStatus && ItemStatus){
    MenuStatus=false;
    ItemStatus=false;
    MenuItem=1;
//    Serial.println("First Page");
  }
  }
}

// second fuction for encoder rotation
void EncoderRotate(){
  // Read the rotary encoder without interrupts
  rotaryState = digitalRead(ROTARY_PIN1);
  if (rotaryState != lastRotaryState) {
    WaitTime=millis();
    if (digitalRead(ROTARY_PIN2) != rotaryState) {
      if(MenuStatus && !ItemStatus && MenuItem<4){
      MenuItem++;
      }
      if(MenuStatus && ItemStatus){
      switch(MenuItem){
        case 1:
        if ((CurrentTemp+TempLimit)<120){
          CurrentTemp++;
        }
        break;
        case 2:
        if (TempLimit<20){
          TempLimit++;
        }
        break;
        case 3:
        if (FanSwitchLimit<240){
          FanSwitchLimit++;
        }
        break;
        case 4:
        break;
      }
      }
    } else {
      if(MenuStatus && !ItemStatus && MenuItem>1){
      MenuItem--;
      }
      if(MenuStatus && ItemStatus){
      switch(MenuItem){
        case 1:
        if ((CurrentTemp-TempLimit)>20){
          CurrentTemp--;
        }
        break;
        case 2:
        if (TempLimit>3){
          TempLimit--;
        }
        break;
        case 3:
        if (FanSwitchLimit>1){
          FanSwitchLimit--;
        }
        break;
        case 4:
        break;
      }
      }
    }
//    Serial.print("Menu: ");
//    Serial.print(MenuItem);
//    Serial.print(",Temp: ");
//    Serial.print(CurrentTemp);
//    Serial.print(",Limit: ");
//    Serial.print(TempLimit);
//    Serial.print(",Fan: ");
//    Serial.println(FanSwitchLimit);
  }

    lastRotaryState = rotaryState;
}
void StartText(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(30, 10);
//    display.setTextColor(SH110X_BLACK,SH110X_WHITE);
    display.println("ARIWA");
    display.setCursor(5, 40);
//    display.setTextColor(SH110X_WHITE);
    display.println("3DPrinter");
    display.display();
}
void OledMenu(){
  if (!MenuStatus){
    display.clearDisplay();
    display.setTextSize(2);
//    display.setFont(SH110X_ArialMT_Plain_16);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 10);
    display.println("Temprature");
    display.setTextSize(3);
    display.setCursor(30, 40);
    display.print(ChambTemp);
    display.setCursor(65, 40);
    display.print(" C");
    display.setTextSize(2);
    display.setCursor(69, 30);
    display.print("o");
    display.display();
  }
  if (MenuStatus && !ItemStatus){
    switch (MenuItem){
      case 1:
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("MainMenu:");
      display.setTextSize(1.5);
      display.setCursor(10, 40);
      display.println(">Temprature Set");
      display.display();
      break;
      case 2:
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("MainMenu:");
      display.setTextSize(1.5);
      display.setCursor(10, 40);
      display.println(">Temp Limit Set");
      display.display();
      break;
      case 3:
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("MainMenu:");
      display.setTextSize(1.5);
      display.setCursor(10, 40);
      display.println(">Fan Interval Set");
      display.display();
      break;
      case 4:
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("MainMenu:");
      display.setTextSize(1.5);
      display.setCursor(10, 40);
      display.println(">Defualt Set");
      display.display();
      break;
    }
  }
  if (MenuStatus && ItemStatus){
    switch (MenuItem){
      case 1:
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("ChamTempSet:");
      display.setTextSize(2);
      display.setCursor(10, 40);
      display.println(CurrentTemp);
      display.display();
      break;
      case 2:
      display.clearDisplay();
      display.setTextSize(1.5);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("LimitTimeSet:");
      display.setTextSize(2);
      display.setCursor(10, 40);
      display.println(TempLimit);
      display.display();
      break;
      case 3:
      display.clearDisplay();
      display.setTextSize(1.5);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("Time Interval:");
      display.setTextSize(2);
      display.setCursor(10, 40);
      display.println(FanSwitchLimit);
      display.display();
      break;
      case 4:
      display.clearDisplay();
      display.setTextSize(1.5);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("Reset:");
      display.setTextSize(1);
      display.setCursor(10, 40);
      display.println("Reset Complete");
      display.display();
      break;
    }
  }
}
int Temprature(){
   // Read the voltage across the thermistor
  int rawADC = analogRead(THERMISTOR_PIN);
  float voltage = rawADC * (5.0 / 1023.0);
  // Calculate the resistance of the thermistor using voltage divider equation
  float thermistorResistance = SERIES_RESISTOR * ((5.0 / voltage) - 1.0);

  // Calculate temperature using Steinhart-Hart equation
  float steinhart;
  steinhart = thermistorResistance / NOMINAL_RESISTANCE;      // (R/Ro)
  steinhart = log(steinhart);                                   // ln(R/Ro)
  steinhart /= B_COEFFICIENT;                                    // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                                  // Invert
  steinhart -= 273.15;                                          // Convert to Celsius
  return steinhart;
}
void switchFans() {
  if (isFan1On) {
    digitalWrite(FAN_PIN_1, LOW);
    digitalWrite(FAN_PIN_2, HIGH);
    isFan1On = false;
  } else {
    digitalWrite(FAN_PIN_1, HIGH);
    digitalWrite(FAN_PIN_2, LOW);
    isFan1On = true;
  }
}
