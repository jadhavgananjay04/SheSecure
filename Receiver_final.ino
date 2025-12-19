#include <TinyGPSPlus.h>

// Objects
TinyGPSPlus gps;

// Pin Definitions (Arduino Mega)
#define BUZZER 7
#define GSM Serial2     // GSM Module: RX2=17, TX2=16
#define GPS Serial1     // GPS Module: RX1=19, TX1=18

bool alertActive = false;
unsigned long lastStatusTime = 0;

void setup() {
  Serial.begin(9600);
  GSM.begin(9600);
  GPS.begin(9600);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  Serial.println(F("🚀 SheSecure System Initializing..."));
  delay(2000);

  // --- GSM Startup Test ---
  if (testGSMConnection()) {
    Serial.println(F("✅ GSM Module Ready"));
  } else {
    Serial.println(F("⚠️ GSM Module Not Responding! Check wiring/power."));
  }

  Serial.println(F("📡 Waiting for GPS signal..."));
  delay(2000);
}

void loop() {
  // --- GPS Reading ---
  while (GPS.available() > 0) {
    gps.encode(GPS.read());
  }

  // Display GPS data
  if (gps.location.isUpdated()) {
    Serial.print(F("📍 Latitude: "));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(" | Longitude: "));
    Serial.print(gps.location.lng(), 6);
    Serial.print(F(" | Satellites: "));
    Serial.println(gps.satellites.isValid() ? gps.satellites.value() : 0);
  }

  // --- Automatically trigger emergency sequence once ---
  static bool triggered = false;
  if (!triggered && gps.location.isValid()) {
    Serial.println(F("🚨 Emergency Trigger Initiated!"));
    alertActive = true;
    handleEmergency();
    alertActive = false;
    triggered = true; // ensure only once
  }

  // --- Periodic Status ---
  if (millis() - lastStatusTime > 10000) {
    Serial.print(F("📡 System Status: "));
    Serial.print(alertActive ? "Alert Active" : "Ready");
    Serial.println(F(" | Monitoring GPS..."));
    lastStatusTime = millis();
  }
}

// --- Test GSM Connection ---
bool testGSMConnection() {
  GSM.println(F("AT"));
  return waitForResponse("OK", 2000);
}

// --- Wait for GSM Response ---
bool waitForResponse(const char* resp, unsigned long timeout) {
  unsigned long start = millis();
  String reply = "";
  while (millis() - start < timeout) {
    while (GSM.available()) {
      reply += (char)GSM.read();
      if (reply.indexOf(resp) != -1) return true;
    }
  }
  return false;
}

// --- Handle Emergency ---
void handleEmergency() {
  Serial.println(F("🛰️ Fetching GPS Location..."));
  unsigned long start = millis();
  while (!gps.location.isValid() && millis() - start < 10000) {
    while (GPS.available()) gps.encode(GPS.read());
  }

  if (gps.location.isValid()) {
    Serial.print(F("✅ GPS Fix | Lat: "));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(" | Lon: "));
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println(F("⚠️ GPS not ready — using last known location."));
  }

  sendAlert();
  triggerBuzzer();
}

// --- Send SMS ---
void sendAlert() {
  Serial.println(F("📲 Sending Emergency SMS..."));

  GSM.println(F("AT+CMGF=1")); // Set SMS text mode
  if (!waitForResponse("OK", 2000)) {
    Serial.println(F("⚠️ GSM didn't respond to CMGF command."));
    return;
  }

  GSM.println(F("AT+CMGS=\"+919699939341\"")); // 🔹 Replace with your number
  if (!waitForResponse(">", 3000)) {
    Serial.println(F("⚠️ GSM not ready to send SMS."));
    return;
  }

  GSM.print(F("🚨 EMERGENCY ALERT! 🚨\n"));
  if (gps.location.isValid()) {
    GSM.print(F("Latitude: "));
    GSM.println(gps.location.lat(), 6);
    GSM.print(F("Longitude: "));
    GSM.println(gps.location.lng(), 6);
    GSM.print(F("Google Maps: https://maps.google.com/?q="));
    GSM.print(gps.location.lat(), 6);
    GSM.print(",");
    GSM.println(gps.location.lng(), 6);
  } else {
    GSM.println(F("GPS not available. Unable to fetch location."));
  }

  GSM.println(F("Please respond immediately."));
  GSM.write(26); // CTRL+Z

  if (waitForResponse("OK", 8000)) {
    Serial.println(F("📩 SMS Sent Successfully!"));
  } else {
    Serial.println(F("⚠️ SMS Sending Failed."));
  }
}

// --- Buzzer Pattern ---
void triggerBuzzer() {
  Serial.println(F("🔔 Activating Buzzer Alert..."));
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(400);
    digitalWrite(BUZZER, LOW);
    delay(250);
  }
  Serial.println(F("✅ Buzzer Alert Completed."));
}