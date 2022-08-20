#define LED 13
#define STOPBUTTON 2
#define LDRIN 3
#define LDRIN2 4
#define MILLISPERBIT 200

#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  9600
#define PHONE_NUMBER  "01780212212"

GPRS gprs(PIN_TX, PIN_RX, BAUDRATE); //RX,TX,PWR,BaudRate

long int lastChangeTime;
bool buttonPressed;

void ldrInterrupt() {
//  Serial.println("ldr changed");
  Serial.println(millis() - lastChangeTime);
   lastChangeTime = millis(); 
}

void buttonInterrupt() {
  buttonPressed = true;
//  Serial.println("Button pressed");
}

void alarm() {
  Serial.println("start to call ...");
  gprs.callUp(PHONE_NUMBER);
  
  int ledState = 1;
  while(!buttonPressed) {
    digitalWrite(LED, ledState);
    delay(500);
    ledState = !ledState;
  }
  Serial.println("alarm should turn off now");
  digitalWrite(LED, 0);
  buttonPressed = false;
  lastChangeTime = millis();
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
  attachInterrupt(digitalPinToInterrupt(LDRIN), ldrInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LDRIN2), ldrInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(STOPBUTTON), buttonInterrupt,RISING);
  lastChangeTime = millis();
  buttonPressed = false;
}



void loop() {
  long int timeElapsed = (millis() - lastChangeTime);
  if(timeElapsed >= 300) Serial.println("WHY----------------");
  if(timeElapsed > (MILLISPERBIT + 100)) {
    
    alarm();
  }
}
