#include <RCSwitch.h>

// Pin Definitions
const int RF_TRANSMITTER_PIN = 4;
const int BUTTON_PIN = 13;
const int LED_PIN = 2;

// RF Code
const unsigned long TRANSMIT_CODE = 12345;

RCSwitch transmitter;

// State variable
bool buttonWasPressed = false;

void setup() {
  Serial.begin(9600);

  transmitter.enableTransmit(RF_TRANSMITTER_PIN);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Shesecure System Ready....!!");
  Serial.println("🚨 Panic Button Ready");
}

void loop() {
  bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  // Detect button press only once
  if (buttonPressed && !buttonWasPressed) {
    Serial.println("🆘 Button Pressed! Sending RF Signal");
    Serial.println("💡 LED Blinking...");

    digitalWrite(LED_PIN, HIGH);

    // Send RF signal (short pulses, no long delay)
    for (int i = 0; i < 5; i++) {
      transmitter.send(TRANSMIT_CODE, 24);
      delay(80); 
    }

    digitalWrite(LED_PIN, LOW);
    buttonWasPressed = true;
  }

  // Reset when button released
  if (!buttonPressed) {
    buttonWasPressed = false;
  }
}