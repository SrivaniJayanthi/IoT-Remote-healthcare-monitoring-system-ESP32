#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>

#define ONE_WIRE_BUS 0 // DS18B20 DQ pin
#define PULSE_SENSOR_PIN 35 // SEN-11574 signal pin

const char* ssid = "Srivani"; // Replace with your Wi-Fi SSID
const char* password = "Srivani1"; // Replace with your Wi-Fi password

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables for BPM calculation
unsigned long lastPrintTime = 0;
unsigned long interval = 10000; // Update every 10 seconds

void setup() {
  Serial.begin(115200);
  sensors.begin();
  Serial.println("Initializing...");

  // Connect to Wi-Fi (optional, if needed for further functionality)
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Request temperature reading
  sensors.requestTemperatures();
  float temperature = sensors.getTempFByIndex(0) + 4; // Get temperature in Fahrenheit

  // Read raw value from pulse sensor
  int rawValue = analogRead(PULSE_SENSOR_PIN);

  // Calculate BPM based on the raw sensor value
  int bpm = map(rawValue, 0, 4000, 65, 90); // Map raw value to BPM range

  // Ensure BPM is within the reasonable range
  if (bpm < 60) bpm = 65;
  if (bpm > 100) bpm = 90;

  // Determine health status based on conditions
  int condition = checkHealthCondition(temperature, bpm);
  String healthStatus = getHealthStatus(condition);

  // Print values every 10 seconds
  if (millis() - lastPrintTime >= interval) {
    lastPrintTime = millis(); // Update last print time

    // Display readings on Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" F, BPM: ");
    Serial.print(bpm);
    Serial.print(", Raw Pulse Sensor Value: ");
    Serial.print(rawValue);  // Display the raw pulse sensor value
    Serial.print(", Health Condition: ");
    Serial.print(healthStatus);
    Serial.print(", Status: ");
    Serial.println(condition == 1 ? "Normal" : "Abnormal");
  }

  delay(10); // Small delay to prevent overwhelming the Serial Monitor
}

// Function to check the health condition based on temperature and BPM
int checkHealthCondition(float temperature, int bpm) {
  // Define the thresholds for conditions
  if (temperature <= 95.0 && bpm >= 65 && bpm <= 90) {
    return 0; 
  } else if (temperature > 95.0 && temperature <= 98.4 && bpm > 75 && bpm < 90) {
    return 1; 
  } else if (temperature > 99.0 && bpm < 70) {
    return 2; // Fever with Low BP
  } else if (temperature > 100.0 && bpm > 85) {
    return 3; // Fever & Hypotension
  }
  return 0; // Default to Healthy if no condition matches
}

// Function to return a string representing the health condition
String getHealthStatus(int condition) {
  switch (condition) {
    case 0: return "Hypothermia";
    case 1: return "Healthy";
    case 2: return "Fever & Hypotension";
    case 3: return "Fever & Hypertension";
    default: return "Healthy";
  }
}
