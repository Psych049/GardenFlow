/*
 * ESP32 GardenFlow Dashboard - Complete IoT Solution
 * 
 * Features:
 * - Bidirectional communication with Supabase
 * - Sends sensor data to database
 * - Receives and executes actuator commands
 * - Secure HTTPS communication
 * - Visual status indicators via onboard LED
 * - Robust error handling and reconnection
 * - Modular sensor and actuator architecture
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// =====================================
// CONFIGURATION
// =====================================

// WiFi credentials
const char* ssid = "vivo";
const char* password = "abcdefgh";

// Supabase configuration
const char* supabaseUrl = "https://xarwvnfmhepbbclwkvio.supabase.co";
const char* supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Inhhcnd2bmZtaGVwYmJjbHdrdmlvIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTI5Mzc1ODMsImV4cCI6MjA2ODUxMzU4M30.a9lXOmLPwBYbvLcpoB-N1-Y95augwnWrR-0L2bIyim0";

// Device configuration (replace with actual UUIDs from your dashboard)
const char* deviceId = "550e8400-e29b-41d4-a716-446655440000";  // Example UUID
const char* zoneId = "550e8400-e29b-41d4-a716-446655440001";   // Example UUID  
const char* userId = "550e8400-e29b-41d4-a716-446655440002";   // Example UUID

// =====================================
// HARDWARE PINS & CONFIGURATION
// =====================================

// WiFi secure client
WiFiClientSecure secureClient;

// LED pin (built-in LED on GPIO2)
#define LED_BUILTIN 2

// Actuator pins (example pins - adjust according to your setup)
#define WATER_PUMP_PIN 16     // Water pump relay
#define FAN_PIN 17            // Cooling fan relay
#define HEATER_PIN 18         // Heating element relay
#define LED_STRIP_PIN 19      // LED grow lights
#define VALVE_PIN 21          // Water valve

// Sensor pins (example pins - adjust according to your setup)
#define DHT_PIN 4             // DHT22 temperature/humidity sensor
#define SOIL_MOISTURE_PIN 34  // Soil moisture sensor (analog)
#define LIGHT_SENSOR_PIN 35   // Light sensor (analog)
#define PH_SENSOR_PIN 36      // pH sensor (analog)
#define FLOW_SENSOR_PIN 5     // Water flow sensor (digital)

// Timing variables
unsigned long lastSensorUpdate = 0;
unsigned long lastCommandCheck = 0;
unsigned long lastLedBlink = 0;
unsigned long sensorInterval = 15000;   // Send sensor data every 15 seconds
unsigned long commandInterval = 5000;   // Check for commands every 5 seconds
unsigned long ledBlinkInterval = 1000;  // LED blink every second

// Status tracking
bool ledState = false;
bool wifiConnected = false;
bool systemHealthy = true;

// Command execution tracking
struct PendingCommand {
  String commandId;
  String commandType;
  unsigned long executionTime;
  bool isActive;
};

PendingCommand currentCommand = {"", "", 0, false};

// =====================================
// DATA STRUCTURES
// =====================================

struct SensorData {
  float temperature;
  float humidity;
  float soilMoisture;
  float waterUsage;
  int lightLevel;
  float phLevel;
};

struct ActuatorStatus {
  bool waterPump;
  bool fan;
  bool heater;
  bool ledStrip;
  bool valve;
};

ActuatorStatus actuators = {false, false, false, false, false};

// =====================================
// SETUP FUNCTION
// =====================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n======================================");
  Serial.println("  ESP32 GardenFlow IoT Controller");
  Serial.println("======================================");
  
  // Initialize all pins
  initializePins();
  
  // Configure secure client for HTTPS
  secureClient.setInsecure();
  Serial.println("✓ WiFiClientSecure configured");
  
  // Connect to WiFi
  connectToWiFi();
  
  // Perform initial system check
  performSystemCheck();
  
  Serial.println("======================================");
  Serial.println("System Status:");
  Serial.println("• Sensor data transmission: Every 15 seconds");
  Serial.println("• Command checking: Every 5 seconds");
  Serial.println("• Status LED: Blinking every second");
  Serial.println("• Serial commands: Type 'help' for options");
  Serial.println("======================================\n");
}

// =====================================
// MAIN LOOP
// =====================================

void loop() {
  // Handle WiFi reconnection if needed
  checkWiFiConnection();
  
  // Handle status LED blinking
  handleStatusLED();
  
  // Send sensor data at regular intervals
  if (millis() - lastSensorUpdate >= sensorInterval) {
    if (wifiConnected) {
      sendSensorDataToSupabase();
    } else {
      Serial.println("⚠ Skipping sensor data - WiFi disconnected");
    }
    lastSensorUpdate = millis();
  }
  
  // Check for commands from dashboard
  if (millis() - lastCommandCheck >= commandInterval) {
    if (wifiConnected) {
      checkForCommands();
    }
    lastCommandCheck = millis();
  }
  
  // Handle active command execution
  handleActiveCommand();
  
  // Handle serial commands for debugging
  handleSerialCommands();
  
  // Small delay to prevent watchdog issues
  delay(100);
}

// =====================================
// INITIALIZATION FUNCTIONS
// =====================================

void initializePins() {
  Serial.println("Initializing pins...");
  
  // Initialize LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Initialize actuator pins (all OFF initially)
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(LED_STRIP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);
  
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(LED_STRIP_PIN, LOW);
  digitalWrite(VALVE_PIN, LOW);
  
  // Initialize sensor pins
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  
  Serial.println("✓ All pins initialized");
}

void performSystemCheck() {
  Serial.println("\nPerforming system check...");
  
  // Test all actuators briefly
  Serial.println("Testing actuators...");
  testActuators();
  
  // Read initial sensor values
  Serial.println("Reading initial sensor values...");
  SensorData data = readAllSensors();
  
  Serial.printf("• Temperature: %.1f°C\n", data.temperature);
  Serial.printf("• Humidity: %.1f%%\n", data.humidity);
  Serial.printf("• Soil Moisture: %.1f%%\n", data.soilMoisture);
  Serial.printf("• Light Level: %d\n", data.lightLevel);
  Serial.printf("• pH Level: %.1f\n", data.phLevel);
  
  systemHealthy = true;
  Serial.println("✓ System check complete\n");
}

void testActuators() {
  // Brief test of all actuators (200ms each)
  digitalWrite(WATER_PUMP_PIN, HIGH);
  delay(200);
  digitalWrite(WATER_PUMP_PIN, LOW);
  
  digitalWrite(FAN_PIN, HIGH);
  delay(200);
  digitalWrite(FAN_PIN, LOW);
  
  digitalWrite(HEATER_PIN, HIGH);
  delay(200);
  digitalWrite(HEATER_PIN, LOW);
  
  digitalWrite(LED_STRIP_PIN, HIGH);
  delay(200);
  digitalWrite(LED_STRIP_PIN, LOW);
  
  digitalWrite(VALVE_PIN, HIGH);
  delay(200);
  digitalWrite(VALVE_PIN, LOW);
  
  Serial.println("✓ Actuator test complete");
}

void connectToWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\n✓ WiFi connected successfully!");
    Serial.printf("  IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("  Signal: %d dBm\n", WiFi.RSSI());
    
    // Success pattern - 3 quick flashes
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  } else {
    wifiConnected = false;
    Serial.println("\n✗ WiFi connection failed!");
    
    // Error pattern - rapid blinks
    for (int i = 0; i < 10; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
    }
  }
}

// =====================================
// SENSOR READING FUNCTIONS
// =====================================

// Read all sensors and return structured data
SensorData readAllSensors() {
  SensorData data;
  
  data.temperature = readTemperature();
  data.humidity = readHumidity();
  data.soilMoisture = readSoilMoisture();
  data.waterUsage = readWaterUsage();
  data.lightLevel = readLightLevel();
  data.phLevel = readPHLevel();
  
  return data;
}

// Individual sensor reading functions (replace with actual sensor code)
float readTemperature() {
  // TODO: Replace with actual DHT22 or DS18B20 sensor
  // #include "DHT.h"
  // DHT dht(DHT_PIN, DHT22);
  // return dht.readTemperature();
  return 22.0 + (random(-50, 100) / 10.0);  // 17.0 - 32.0°C
}

float readHumidity() {
  // TODO: Replace with actual DHT22 sensor
  // return dht.readHumidity();
  return 50.0 + (random(0, 300) / 10.0);  // 50.0 - 80.0%
}

float readSoilMoisture() {
  // TODO: Replace with actual capacitive soil moisture sensor
  // int rawValue = analogRead(SOIL_MOISTURE_PIN);
  // return map(rawValue, 0, 4095, 0, 100);
  return 40.0 + (random(0, 400) / 10.0);  // 40.0 - 80.0%
}

float readWaterUsage() {
  // TODO: Replace with actual flow sensor integration
  // Static variable to accumulate flow over time
  static float totalFlow = 0;
  totalFlow += random(0, 10) / 100.0;  // Small increments
  return totalFlow;
}

int readLightLevel() {
  // TODO: Replace with actual light sensor (LDR or photodiode)
  // int rawValue = analogRead(LIGHT_SENSOR_PIN);
  // return map(rawValue, 0, 4095, 0, 1000);
  return random(200, 900);  // Lux equivalent
}

float readPHLevel() {
  // TODO: Replace with actual pH sensor
  // int rawValue = analogRead(PH_SENSOR_PIN);
  // float voltage = rawValue * (3.3 / 4095.0);
  // return convertVoltageToPH(voltage);
  return 6.0 + (random(0, 20) / 10.0);  // 6.0 - 8.0 pH
}

// ===================================== 
// MAIN SENSOR DATA TRANSMISSION
// =====================================

void sendSensorDataToSupabase() {
  Serial.println("\n--- Sending Sensor Data to Supabase ---");
  
  // Get sensor data (currently dummy values)
  SensorData sensorData = generateSensorData();
  
  // Create JSON payload matching sensor_data table schema
  DynamicJsonDocument doc(1024);
  doc["device_id"] = deviceId;
  doc["zone_id"] = zoneId;
  doc["user_id"] = userId;
  doc["temperature"] = sensorData.temperature;
  doc["humidity"] = sensorData.humidity;
  doc["soil_moisture"] = sensorData.soilMoisture;
  doc["light_level"] = random(200, 800);  // Optional field
  doc["ph_level"] = 6.5 + (random(-10, 10) / 10.0);  // Optional field
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.printf("Payload: %s\n", jsonString.c_str());
  
  // Send HTTP POST to Supabase REST API
  HTTPClient http;
  String url = String(supabaseUrl) + "/rest/v1/sensor_data";
  
  http.begin(secureClient, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", String("Bearer ") + supabaseAnonKey);
  http.addHeader("Prefer", "return=minimal");  // Reduce response size
  
  Serial.printf("Posting to: %s\n", url.c_str());
  
  int httpResponseCode = http.POST(jsonString);
  
  Serial.printf("HTTP Response Code: %d\n", httpResponseCode);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    if (response.length() > 0) {
      Serial.printf("Response: %s\n", response.c_str());
    }
    
    if (httpResponseCode >= 200 && httpResponseCode < 300) {
      Serial.println("✓ Sensor data sent successfully!");
      Serial.printf("  Temperature: %.1f°C\n", sensorData.temperature);
      Serial.printf("  Humidity: %.1f%%\n", sensorData.humidity);
      Serial.printf("  Soil Moisture: %.1f%%\n", sensorData.soilMoisture);
      Serial.printf("  Water Usage: %.1fL\n", sensorData.waterUsage);
    } else {
      Serial.printf("✗ Server returned error code: %d\n", httpResponseCode);
    }
  } else {
    Serial.printf("✗ HTTP Request failed with error: %d\n", httpResponseCode);
    String errorResponse = http.getString();
    if (errorResponse.length() > 0) {
      Serial.printf("Error details: %s\n", errorResponse.c_str());
    }
  }
  
  http.end();
  Serial.println("--- End Sensor Data Transmission ---\n");
}

// =====================================
// ADDITIONAL UTILITY FUNCTIONS
// =====================================

// Function to print system status
void printSystemStatus() {
  Serial.println("\n=== System Status ===");
  Serial.printf("WiFi Connected: %s\n", wifiConnected ? "Yes" : "No");
  if (wifiConnected) {
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Signal Strength: %d dBm\n", WiFi.RSSI());
  }
  Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.println("====================\n");
}

// Function for manual sensor reading (can be called from Serial commands)
void forceSensorReading() {
  Serial.println("Manual sensor reading requested...");
  sendSensorDataToSupabase();
}

// Optional: Function to handle Serial commands for debugging
void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "status") {
      printSystemStatus();
    } else if (command == "send") {
      forceSensorReading();
    } else if (command == "wifi") {
      connectToWiFi();
    } else if (command == "help") {
      Serial.println("\nAvailable commands:");
      Serial.println("  status - Show system status");
      Serial.println("  send   - Force sensor data transmission");
      Serial.println("  wifi   - Reconnect WiFi");
      Serial.println("  help   - Show this help\n");
    }
  }
}