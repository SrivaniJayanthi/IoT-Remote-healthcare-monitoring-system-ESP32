#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h> // Library for Wi-Fi connectivity
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for I2C LCD

#define ONE_WIRE_BUS 0 // DS18B20 DQ pin
#define PULSE_SENSOR_PIN 35 // SEN-11574 signal pin

const char* ssid = "Srivani"; // Replace with your Wi-Fi SSID
const char* password = "Srivani1"; // Replace with your Wi-Fi password
const char* server = "api.thingspeak.com";
const char* apiKey = "NCDYTM8GQSTGTSIB"; // Replace with your ThingSpeak Write API Key

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables for BPM calculation
unsigned long lastPrintTime = 0;
unsigned long interval = 15000; // Update ThingSpeak every 15 seconds

WiFiClient client;

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address, 16 columns, 2 rows

void setup() {
  Serial.begin(115200);
  sensors.begin();
  lcd.init();  // Initialize LCD with 16 columns and 2 rows
  lcd.backlight(); // Turn on the backlight

  Serial.println("Initializing...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print the IP address assigned
}

void loop() {
  // Print this message once per loop execution
  static bool firstRun = true;
  if (firstRun) {
    Serial.println("Entering loop...");
    firstRun = false; // Set to false to avoid printing in subsequent runs
  }

  // Reconnect Wi-Fi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected, reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nReconnected to Wi-Fi");
  }

  // Request temperature reading
  sensors.requestTemperatures();
  float temperature = sensors.getTempFByIndex(0) + 4; // Get temperature in Fahrenheit

  // Read raw value from pulse sensor
  int rawValue = analogRead(PULSE_SENSOR_PIN);

  // Calculate BPM based on the raw sensor value
  int bpm = map(rawValue, 0, 4000, 60, 100); // Map raw value to BPM range

  // Ensure BPM is within the reasonable range
  if (bpm < 60) bpm = 60;
  if (bpm > 100) bpm = 100;

  // Print values every 15 seconds and send to ThingSpeak
  if (millis() - lastPrintTime >= interval) {
    lastPrintTime = millis(); // Update last print time

    // Display readings on Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" F, Raw Pulse Sensor Value: ");
    Serial.print(rawValue);
    Serial.print(", BPM: ");
    Serial.println(bpm);

    // Display readings on LCD
    lcd.clear(); // Clear previous readings
    lcd.setCursor(0, 0); // Set cursor to the first row
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" F");
    
    lcd.setCursor(0, 1); // Set cursor to the second row
    lcd.print("BPM: ");
    lcd.print(bpm);

    // Send data to ThingSpeak
    if (client.connect(server, 80)) {
      String postData = "api_key=" + String(apiKey) + "&field1=" + String(temperature) + "&field2=" + String(bpm);
      
      Serial.println("Sending data to ThingSpeak...");
      Serial.println(postData); // Print the data being sent

      client.println("POST /update HTTP/1.1");
      client.println("Host: " + String(server));
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(postData.length());
      client.println();
      client.print(postData);

      // Read and display the server response
      while (client.connected() || client.available()) {
        if (client.available()) {
          String response = client.readStringUntil('\n');
          Serial.println(response); // Print the response for debugging
        }
      }

      client.stop(); // Disconnect
      Serial.println("Data sent to ThingSpeak");
    } else {
      Serial.println("Failed to connect to ThingSpeak");
    }
  }

  delay(10); // Small delay to prevent overwhelming the Serial Monitor
}
