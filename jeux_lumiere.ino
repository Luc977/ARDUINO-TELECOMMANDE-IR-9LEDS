#include <IRremote.h>

int IR_RECEIVE_PIN = 11;
int leds[9] = {2,3,4,5,6,7,8,9,10}; // 0-2 Rouge, 3-5 Jaune, 6-8 Bleu

int rouge[3] = {0,1,2};
int jaune[3] = {3,4,5};
int bleu[3] = {6,7,8};

// Modes toggle
bool modeAlternance = false; // OK
bool modeChenilleD = false; // DROITE
bool modeChenilleG = false; // GAUCHE

unsigned long lastAction = 0;
bool altState = false; // false=BR on, true=J on
int chenillePos = 0;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  for(int i=0; i<9; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
}

void eteindreTout() {
  for(int i=0; i<9; i++) digitalWrite(leds[i], LOW);
}

void loop() {
  if(IrReceiver.decode()) {
    uint16_t cmd = IrReceiver.decodedIRData.command;

    // 1-9 = LED individuelle
    if(cmd >= 0x01 && cmd <= 0x09) {
      modeAlternance = modeChenilleD = modeChenilleG = false;
      int ledIndex = cmd - 1;
      digitalWrite(leds[ledIndex],!digitalRead(leds[ledIndex]));
    }

// Touches 1-9 = toggle LED individuelle avec TES codes
if(cmd == 0x45) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[0],!digitalRead(leds[0])); } // 1
if(cmd == 0x46) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[1],!digitalRead(leds[1])); } // 2
if(cmd == 0x47) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[2],!digitalRead(leds[2])); } // 3
if(cmd == 0x44) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[3],!digitalRead(leds[3])); } // 4
if(cmd == 0x40) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[4],!digitalRead(leds[4])); } // 5
if(cmd == 0x43) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[5],!digitalRead(leds[5])); } // 6
if(cmd == 0x7) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[6],!digitalRead(leds[6])); } // 7
if(cmd == 0x15) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[7],!digitalRead(leds[7])); } // 8
if(cmd == 0x9) { modeAlternance = modeChenilleD = modeChenilleG = false; digitalWrite(leds[8],!digitalRead(leds[8])); } // 9

    // OK = toggle alternance Jaune / Rouge+Bleu
    if(cmd == 0x1C) {
      modeAlternance =!modeAlternance;
      modeChenilleD = modeChenilleG = false;
      altState = false;
      if(!modeAlternance) eteindreTout();
      delay(300);
    }

    // DROITE 0x5A = toggle chenille droite->gauche
    if(cmd == 0x5A) {
      modeChenilleD =!modeChenilleD;
      modeAlternance = modeChenilleG = false;
      chenillePos = 8;
      if(!modeChenilleD) eteindreTout();
      delay(300);
    }

    // GAUCHE 0x08 = toggle chenille gauche->droite
    if(cmd == 0x08) {
      modeChenilleG =!modeChenilleG;
      modeAlternance = modeChenilleD = false;
      chenillePos = 0;
      if(!modeChenilleG) eteindreTout();
      delay(300);
    }

    IrReceiver.resume();
  }

  // Mode alternance OK : 500ms
  if(modeAlternance && millis() - lastAction >= 500) {
    lastAction = millis();
    altState =!altState;

    if(altState) { // Jaune ON
      for(int i=0; i<3; i++) digitalWrite(leds[jaune[i]], HIGH);
      for(int i=0; i<3; i++) digitalWrite(leds[rouge[i]], LOW);
      for(int i=0; i<3; i++) digitalWrite(leds[bleu[i]], LOW);
    } else { // Rouge+Bleu ON
      for(int i=0; i<3; i++) digitalWrite(leds[jaune[i]], LOW);
      for(int i=0; i<3; i++) digitalWrite(leds[rouge[i]], HIGH);
      for(int i=0; i<3; i++) digitalWrite(leds[bleu[i]], HIGH);
    }
  }

  // Mode chenille DROITE : 150ms par LED
  if(modeChenilleD && millis() - lastAction >= 150) {
    lastAction = millis();
    eteindreTout();
    digitalWrite(leds[chenillePos], HIGH);
    chenillePos--;
    if(chenillePos < 0) chenillePos = 8; // reboucle
  }

  // Mode chenille GAUCHE : 150ms par LED
  if(modeChenilleG && millis() - lastAction >= 150) {
    lastAction = millis();
    eteindreTout();
    digitalWrite(leds[chenillePos], HIGH);
    chenillePos++;
    if(chenillePos > 8) chenillePos = 0; // reboucle
  }
}