#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ---------------- RF24 setup ----------------
RF24 radio(7, 8); // CE, CSN pins

const byte addresses[4][6] = {
  "R001",
  "R002",
  "R003",
  "R004"
};

// ---------------- Inputs ----------------
const int patternShiftButton = A0;
const int pulseButton = A1;

// ---------------- Outputs ----------------
const int pin2 = 2;
const int pin3 = 3;
const int pin4 = 4;
const int pin5 = 5;

const int pulsePin = 6;
const int dataPin  = 3;
const int clkPin   = 5;

// ---------------- Variables ----------------
int patternState = 0;
int lastPatternBtnState = HIGH;
int lastPulseBtnState = HIGH;

int shiftData[4] = {0, 0, 0, 0};
int currentBit = 0;

void setup() {

  Serial.begin(9600);

  // RF setup
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.stopListening();

  // Inputs
  pinMode(patternShiftButton, INPUT_PULLUP);
  pinMode(pulseButton, INPUT_PULLUP);

  // Outputs
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(pin5, OUTPUT);
  pinMode(pulsePin, OUTPUT);

  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  digitalWrite(pin5, LOW);
  digitalWrite(pulsePin, LOW);
}

void loop() {

  // ---------------- Pattern shift button ----------------
  int currentPatternBtn = digitalRead(patternShiftButton);

  if (currentPatternBtn == LOW && lastPatternBtnState == HIGH) {

    patternState = (patternState + 1) % 4;

    // Update output pattern
    switch (patternState) {
      case 0:
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, LOW);
        digitalWrite(pin5, HIGH);
        break;

      case 1:
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, HIGH);
        digitalWrite(pin5, LOW);
        break;

      case 2:
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, HIGH);
        digitalWrite(pin4, LOW);
        digitalWrite(pin5, LOW);
        break;

      case 3:
        digitalWrite(pin2, HIGH);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, LOW);
        digitalWrite(pin5, LOW);
        break;
    }

    // Shift simulation
    digitalWrite(clkPin, HIGH);
    delay(10);
    digitalWrite(clkPin, LOW);

    currentBit++;
    if (currentBit >= 4) currentBit = 0;

    delay(200); // debounce
  }

  lastPatternBtnState = currentPatternBtn;

  // ---------------- Pulse button (send RF data) ----------------
  int currentPulseBtn = digitalRead(pulseButton);

  if (currentPulseBtn == LOW && lastPulseBtnState == HIGH) {

    // Generate pulse
    digitalWrite(pulsePin, HIGH);
    delay(50);
    digitalWrite(pulsePin, LOW);

    // Send via RF24
    radio.openWritingPipe(addresses[patternState]);

    bool success = radio.write(&patternState, sizeof(patternState));

    Serial.print("Sent Index: ");
    Serial.print(patternState);
    Serial.print(" to ");
    Serial.print((char*)addresses[patternState]);
    Serial.println(success ? " ✔ OK" : " ✖ Failed");

    delay(200); // debounce
  }

  lastPulseBtnState = currentPulseBtn;
}