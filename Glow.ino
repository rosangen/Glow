#include <NewRemoteReceiver.h>   // https://bitbucket.org/fuzzillogic/433mhzforarduino/wiki/Home
#define NEXA_ADDRESS 5519106
#define GLOW_UNIT 4
#define RELAY_UNIT 8
#define RELAY_PIN 13

int ledCandle[] = {9,10,11};   // choose the candle pin for the LED
int maxCount = 65;   // counter for glowing table
int count[3];
int dim[3], newDim[] = {10,10,10};
byte value[3], newValue[3];
unsigned long timeBlink[3];
byte bilnkTime[] = {5,5,5};
byte newBilnkTime[] = {5,5,5};  // Milli sec
byte glowTable[] = {100,110,115,120,130,130,135,135,140,135,130,130,130,130,133,131,120,110,105,100,100,100,100,110,115,110,108,105,106,107,110,100,108,108,105,105,100,100,110,120,120,130,130,130,140,140,140,135,135,135,135,140,140,140,130,120,120,110,108,100,108,110,105,102,100,100}; 
volatile boolean glowOn = true;
volatile boolean relay = false;
volatile unsigned long addressRecived;
volatile byte unitRecived;

// Callback function is called only when a valid code is received.
void reciveCode(NewRemoteCode receivedCode) {
  // Note: interrupts are disabled. You can re-enable them if needed.

  unsigned long address = receivedCode.address;
  byte unit = receivedCode.unit;

  if ( address == NEXA_ADDRESS && unit == GLOW_UNIT ) {
    switch (receivedCode.switchType) {
      case NewRemoteCode::off:
        glowOn = false;
        break;
      case NewRemoteCode::on:
        glowOn = true;
        break;
    }
  }

  if ( address == NEXA_ADDRESS && unit == RELAY_UNIT ) {
    switch (receivedCode.switchType) {
      case NewRemoteCode::off:
        relay = false;
        break;
      case NewRemoteCode::on:
        relay = true;
        break;
    }
  }
  

  addressRecived = address; 
  unitRecived = unit;
}

void glow(byte led) {
  unsigned long time = millis();
  if ( time - timeBlink[led] > bilnkTime[led] ){
    timeBlink[led] = time;

    if ( !glowOn ) newDim[led] = 0;

    if ( newValue[led] == value[led] ) {
      newValue[led] = glowTable[count[led]] * dim[led]/100;
      count[led]++;
      if(count[led] > maxCount ) count[led] = 0;
    }
    if ( newValue[led] < value[led] ) value[led]--;
    if ( newValue[led] > value[led] ) value[led]++;

    analogWrite(ledCandle[led], value[led]);

    if ( newDim[led] < dim[led] ) dim[led]--;
    if ( newDim[led] > dim[led] ) dim[led]++;
    if ( (random(1000) == 1 && newDim[led] == dim[led]) || (random(200) == 1 && dim[led] == 1) ) {
      newDim[led] = random(5,50) ;
      Serial.print(F("New Dim:"));
      Serial.println(newDim[led]);
    }

    if ( newBilnkTime[led] < bilnkTime[led] ) bilnkTime[led]--;
    if ( newBilnkTime[led] > bilnkTime[led] ) bilnkTime[led]++;
    if ( random(1000) == 1 && newBilnkTime[led] == bilnkTime[led]) {
      newBilnkTime[led] = random(10,20) ;
      Serial.print(F("New blink:"));
      Serial.println(newBilnkTime[led]);
    }

    // Wind
    if ( random(2000) == 10 ) {
      newDim[led] = 1;
      newBilnkTime[led] = 1;
      Serial.print(F("wind:"));
    }
  }
}


void setup() {
  Serial.begin(57600);
  Serial.println(F("Glow!"));

  randomSeed(analogRead(0));

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  pinMode(ledCandle[0], OUTPUT);
  pinMode(ledCandle[1], OUTPUT);
  pinMode(ledCandle[2], OUTPUT);

  NewRemoteReceiver::init(0, 2, reciveCode);
}

void loop(){
  glow(0);
  glow(1);
  glow(2);

  if ( addressRecived ) {
    digitalWrite(RELAY_PIN, !relay);
    
    Serial.print(F("Recived address: "));
    Serial.print(addressRecived);
    Serial.print(F(" unit: "));
    Serial.println(unitRecived);
    
    addressRecived = 0;
  }
}

