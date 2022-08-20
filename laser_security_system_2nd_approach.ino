#define LED 13
#define STOPBUTTON 4
#define LDRIN 3
#define LDRIN2 2
#define MILLISPERBIT 200

#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
//#include <GSM.h>

#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  9600
#define PHONE_NUMBER  "01780212212"
#define MESSAGE "Breach of boundary detected."

GPRS gprs(PIN_TX, PIN_RX, BAUDRATE); //RX,TX,PWR,BaudRate

long int lastChangeTimeOfLdr1;
long int lastChangeTimeOfLdr2;
bool buttonPressed;

void ldr1Interrupt() {
//  Serial.println("ldr changed");
  Serial.println(millis() - lastChangeTimeOfLdr1);
  lastChangeTimeOfLdr1 = millis(); 
}

void ldr2Interrupt() {
  Serial.println(millis() - lastChangeTimeOfLdr2);
  lastChangeTimeOfLdr2 = millis(); 
}

void buttonInterrupt() {
  buttonPressed = true;
//  Serial.println("Button pressed");
}

void alarm() {
  Serial.println("start to call ...");

  gprs.sendSMS(PHONE_NUMBER, MESSAGE);
  gprs.callUp(PHONE_NUMBER);
  
  int ledState = 1;
  while(!digitalRead(STOPBUTTON)) {
    digitalWrite(LED, ledState);
    delay(500);
    ledState = !ledState;
  }
  Serial.println("alarm should turn off now");
  digitalWrite(LED, 0);
  buttonPressed = false;
  lastChangeTimeOfLdr1 = millis();
  lastChangeTimeOfLdr2 = millis();
}

void setup() {
  gprs.checkPowerUp();
  Serial.begin(9600);
  while (!gprs.init()) {
      delay(1000);
      Serial.println("Initialization failed!");
  }

  while (!gprs.isNetworkRegistered()) {
      delay(1000);
      Serial.println("Network has not registered yet!");
  }
  
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  
  // enabling pins for interrupt
//  pciSetup(15);
  attachInterrupt(digitalPinToInterrupt(LDRIN), ldr1Interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LDRIN2), ldr2Interrupt, CHANGE);
//  attachInterrupt(digitalPinToInterrupt(STOPBUTTON), buttonInterrupt,RISING);
  lastChangeTimeOfLdr1 = millis();
  lastChangeTimeOfLdr2 = millis();
  buttonPressed = false;
}



void loop() {
  long int timeElapsed1 = (millis() - lastChangeTimeOfLdr1);
  long int timeElapsed2 = (millis() - lastChangeTimeOfLdr2);
  if(timeElapsed1 >= 300 || timeElapsed2 >= 300) Serial.println("WHY----------------");
  if(timeElapsed1 > (MILLISPERBIT + 100) || timeElapsed2 > (MILLISPERBIT + 100)) {
    alarm();
  }
}
