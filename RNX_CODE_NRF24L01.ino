#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN

const byte thisAddress[6] = "R0001";  // Unique address for this receiver

// Output pins
const int ledPins[4] = {3, 4, 5, 6};  // LEDs: pin 3 to 6
const int buzzerPin = 10;             // Buzzer: pin 10

int currentStep = 0;  // Index to track which pin to activate (0 to 3)

void setup() {
  Serial.begin(9600);

  // Initialize LED pins and buzzer
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Set up nRF24L01
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.openReadingPipe(0, thisAddress);
  radio.startListening();

  Serial.println("Receiver R0001 is ready...");
}

void loop() {
  if (radio.available()) {
    int receivedIndex;
    radio.read(&receivedIndex, sizeof(receivedIndex));

    Serial.print("Received index: ");
    Serial.println(receivedIndex);

    if (receivedIndex == 0) {
      // Turn off all LEDs and buzzer
      for (int i = 0; i < 4; i++) {
        digitalWrite(ledPins[i], LOW);
      }
      digitalWrite(buzzerPin, LOW);

      // Turn ON the current LED
      digitalWrite(ledPins[currentStep], HIGH);
      Serial.print("Turning ON pin: ");
      Serial.println(ledPins[currentStep]);

      // If the current pin is pin 3 (index 0), turn ON buzzer for 5 sec
      if (currentStep == 0) {
        digitalWrite(buzzerPin, HIGH);
        Serial.println("Buzzer ON for 5 seconds");
        delay(5000);  // Blocking delay for buzzer
        digitalWrite(buzzerPin, LOW);
        Serial.println("Buzzer OFF");
      }

      // Move to the next step (0 → 1 → 2 → 3 → 0 ...)
      currentStep = (currentStep + 1) % 4;
    }
  }
}