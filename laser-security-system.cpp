
#define led 13
#define stopButton 2
#define ldrInput 12
#define millisPerBit 500

int interrupted = 0;
unsigned long alarmStartTime;
const int interval = 500;
int ledState = LOW;
unsigned char pattern = 0b11001100;
unsigned char allPatterns[8];

void stopAlarm() {
  Serial.println("INTR called");
  interrupted = 0;
  ledState = LOW;
  digitalWrite(led, ledState);
}
void setup(){
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(ldrInput, INPUT);
  pinMode(stopButton, INPUT);

//  for(int i = 0; i < 8; i++) {
//    // left shift and rotate i times 
//    allPatterns[i] = (pattern >> i) | (pattern << (8 - i));
//    Serial.println(allPatterns[i]);
//  }
  allPatterns[0] = allPatterns[1] = allPatterns[2] = allPatterns[3] = 25;
  
  allPatterns[4] = allPatterns[5] = allPatterns[6] = allPatterns[7] = 230;
}

int initialize() {
  // check if the laser is in on state
  unsigned char startBit;
  int in = !digitalRead(ldrInput);
  if(in) {
    // if yes then wait for off state
    startBit = 0;
    in = !digitalRead(ldrInput);
    while(in) {
      Serial.println("On");
      in = !digitalRead(ldrInput);
    }
  } else {
    // if no then wait for on state
    startBit = 1;
    in = !digitalRead(ldrInput);
    while(in) {
      Serial.println("OFF");
      in = !digitalRead(ldrInput);
    }
  }

  // catch the pattern
  unsigned char foundPattern;
  foundPattern = startBit;
  for(int i = 1; i < 8; i++) {
    delay(millisPerBit);
    int in = !digitalRead(ldrInput);
    foundPattern = (foundPattern << 1) | in;
  }

  Serial.println("pattern read. Found pattern");
  Serial.println(foundPattern);
  for(int i = 0; i < 8; i++) {
    if(foundPattern == allPatterns[i])
      return i;
  }

  return -1;
}
void loop(){
//  if(digitalRead(12)){
//
//     interrupted = 1;
//     ledState = HIGH;
//     digitalWrite(led, ledState);
//     alarmStartTime = millis();
//  }
  
  int i = initialize();
  Serial.print("pattern index ");
  Serial.println(i);
  while(i < 8) {
    unsigned char expectedBit = (pattern >> (7 - i)) & 1;
    Serial.println("expected: ");
    Serial.println(expectedBit);
    long int timeBeforeStartOfCurrentBit = millis();
    while(millis() - timeBeforeStartOfCurrentBit < millisPerBit) {
      int in = !digitalRead(ldrInput);
      if(in == expectedBit) {
        // do nothing
      } else {
        Serial.println("found ");
        Serial.println(!digitalRead(ldrInput));
        Serial.println("Didn't match");
        interrupted = 1;
        break;
      }
    }

    if(interrupted)
      break;
    i++;
  }

  if(interrupted) {
    Serial.println("in if");
    unsigned long currentTime = millis();
    unsigned long alarmTempTime = alarmStartTime;
    Serial.print("currentTime = ");
    Serial.println(currentTime);
//    int countBeep = 0;
//    while(interrupted && countBeep <= 30) {
    while(interrupted) {
      currentTime = millis();
//      Serial.println("In While");
      if(currentTime - alarmTempTime >= 500) {
//        Serial.println(
        alarmTempTime = currentTime;
//        currentTime = millis();
        if(ledState == LOW) {
          
          ledState = HIGH;
        }
        else ledState = LOW;
        
//        countBeep++;
      }
      digitalWrite(led, ledState);

      if(digitalRead(stopButton)) {
        Serial.println("aa");
        break;
      }
    }

    ledState = LOW;
    digitalWrite(led, ledState);
    interrupted = 0;
  }
}

