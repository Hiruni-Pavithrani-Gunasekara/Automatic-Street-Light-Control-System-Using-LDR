
// --- Library for DHT22 Sensor ---
// You will need to install the "DHT sensor library" by Adafruit from the Arduino Library Manager.
#include "DHT.h"

// --- Define Sensor and Component Pins ---
const int DHT_PIN = 8;       // DHT22 Data pin connected to Digital Pin 8
const int PIR_PIN = 3;       // PIR Motion Sensor Output connected to Digital Pin 3
const int LDR_PIN = A0;      // LDR Sensor Analog Output connected to Analog Pin A0
const int LED_PIN = 10;      // LED connected to Digital PWM Pin 10 (Required for Dimming)

// --- Sensor Settings ---
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// --- Thresholds for Logic (ADJUST THESE VALUES based on testing) ---
// High Humidity Threshold (in percentage)
const float HUMIDITY_HIGH_THRESHOLD = 60.0; 

// Low Light Threshold (Analog reading; assuming 0=Dark, 1023=Bright for the module)
const int LDR_LOW_LIGHT_THRESHOLD = 200; 

// --- LED Brightness Levels (0-255) ---
const int LED_BRIGHT = 255;  // Max brightness (Fully ON)
const int LED_DIM = 50;      // Dim brightness 
const int LED_OFF = 0;       // Off

void setup() {
  Serial.begin(9600);
  Serial.println("Smart Environmental Lighting System Initialized.");

  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  
  dht.begin();
}

void loop() {
  // --- 1. Read Sensor Data ---
  float humidity = dht.readHumidity();
  int motionState = digitalRead(PIR_PIN);
  int lightIntensity = analogRead(LDR_PIN);

  // --- 2. Check for Valid DHT Readings ---
  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor! Assuming environment is NOT critical.");
    // If DHT fails, assume humidity is NOT high to prevent false critical triggers
    humidity = 0.0; 
  } 
  
  // --- 3. Determine Environmental Conditions ---
  bool isHumidityHigh = (humidity > HUMIDITY_HIGH_THRESHOLD);
  bool isLightLow = (lightIntensity < LDR_LOW_LIGHT_THRESHOLD);
  
  // Check if the environment is in a "critical" state (Low Light OR High Humidity)
  bool isEnvironmentCritical = isLightLow || isHumidityHigh;

  // --- 4. Debug Output (Optional) ---
  Serial.print("Motion: "); Serial.print(motionState == HIGH ? "YES" : "NO");
  Serial.print(" | Humidity: "); Serial.print(humidity); 
  Serial.print(" | Light: "); Serial.print(lightIntensity);
  Serial.print(" | Environment Critical: "); Serial.print(isEnvironmentCritical ? "YES" : "NO"); Serial.println();
  
  // --- 5. Logic to Control LED Brightness ---
  
  int currentLEDState = LED_OFF; // Default state is OFF

  // --- CONDITION 1: FULLY ON ---
  // FULLY ON: PIR detects motion AND (Low Light OR High Humidity)
  if (motionState == HIGH && isEnvironmentCritical) {
    currentLEDState = LED_BRIGHT;
    Serial.println("-> LED: BRIGHT (Motion + Critical Environment)");
  } 
  
  // --- CONDITION 2: DIM ---
  // DIM: PIR does NOT detect motion AND (Low Light OR High Humidity)
  else if (motionState == LOW && isEnvironmentCritical) {
    currentLEDState = LED_DIM;
    Serial.println("-> LED: DIM (No Motion, but Critical Environment)");
  } 
  
  // --- CONDITION 3: OFF ---
  // OFF: Otherwise (Any condition not covered above)
  // This covers two scenarios: 
  //   1. No Motion AND Environment is NOT critical (Low Light=NO, High Humidity=NO)
  //   2. Motion AND Environment is NOT critical (Your requirement implies this falls to "otherwise off" unless the condition for "DIM" covers it. Based on your wording, we assume it's OFF.)
  else {
    currentLEDState = LED_OFF;
    Serial.println("-> LED: OFF (System Idle or Motion but Environment is Fine)");
  }

  // --- 6. Apply LED State ---
  analogWrite(LED_PIN, currentLEDState);

  // Delay before the next reading
  delay(1000); //Temporary delay (1000ms) used for easy observation and debugging.
  // For real-time simulation,this delay should be increased to 15 minutes.
}