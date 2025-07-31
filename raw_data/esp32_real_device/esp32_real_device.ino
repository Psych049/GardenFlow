/*
 * ESP32 Plant Monitoring System - REAL DEVICE VERSION
 * 
 * Configure this for your actual ESP32 device
 * 
 * Pin Configuration:
 * - DHT22: GPIO 4
 * - Soil Moisture Sensor: GPIO 36 (ADC1_CH0)
 * - Relay/Pump Control: GPIO 5
 * - Status LED: GPIO 2
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <WiFiClientSecure.h>

// ========================================
// CONFIGURE THESE SETTINGS FOR YOUR DEVICE
// ========================================

// WiFi Configuration - UPDATE THESE
const char* ssid = "Oppo..";
const char* password = "alphaxyz@1234";

// Supabase Configuration - UPDATE THESE
const char* supabaseUrl = "https://gqzaxkczxcudxbbkudmm.supabase.co";
const char* supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImdxemF4a2N6eGN1ZHhiYmt1ZG1tIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTM3MzcwNzEsImV4cCI6MjA2OTMxMzA3MX0.RR4jib8iRkZG1rqFpH3wuTE82BY5ViJKFR0FVvu5N4U";

// Device Configuration - UPDATE THESE
const String deviceId = "ESP32_01";  // Must match your dashboard device ID
const String deviceName = "ESP32 01"; // Must match your dashboard device name
const String sensorZoneId = "";    // Must match your zone ID (get this from dashboard)

// API Key - Get this from your dashboard after adding device
const char* apiKey = "YOUR_API_KEY_HERE";  // Copy from dashboard API Keys section

// ========================================
// HARDWARE PIN CONFIGURATION
// ========================================

// Pin Definitions - Adjust if your wiring is different
#define DHT_PIN 4
#define SOIL_MOISTURE_PIN 36
#define PUMP_PIN 5
#define STATUS_LED_PIN 2

// Sensor Configuration
#define DHT_TYPE DHT22
#define SOIL_MOISTURE_DRY 4095    // Calibrate these values for your sensor
#define SOIL_MOISTURE_WET 1500    // Dry = 4095, Wet = 1500 (typical)
#define SENSOR_READ_INTERVAL 30000 // 30 seconds
#define COMMAND_CHECK_INTERVAL 10000 // 10 seconds

// ========================================
// GLOBAL VARIABLES
// ========================================

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClientSecure client;
bool pumpRunning = false;
unsigned long lastSensorRead = 0;
unsigned long lastCommandCheck = 0;
unsigned long pumpStartTime = 0;
int pumpDuration = 30; // seconds
bool dhtInitialized = false;

// ========================================
// SETUP FUNCTION
// ========================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("========================================");
  Serial.println("ESP32 Plant Monitor - REAL DEVICE");
  Serial.println("========================================");
  Serial.print("Device ID: ");
  Serial.println(deviceId);
  Serial.print("Device Name: ");
  Serial.println(deviceName);
  Serial.print("Zone ID: ");
  Serial.println(sensorZoneId);
  Serial.println("========================================");
  
  // Initialize pins
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Initialize DHT sensor
  Serial.println("Initializing DHT sensor...");
  dht.begin();
  delay(2000); // Give DHT sensor time to initialize
  dhtInitialized = true;
  
  // Setup WiFi
  setupWiFi();
  
  // Register device with dashboard
  updateDeviceStatus();
  
  Serial.println("ESP32 Plant Monitor initialized successfully!");
  blinkLED(3);
}

// ========================================
// MAIN LOOP
// ========================================

void loop() {
  unsigned long currentTime = millis();
  
  // Read sensors every 30 seconds
  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    Serial.println("Reading sensors...");
    readSensors();
    lastSensorRead = currentTime;
  }
  
  // Check for commands every 10 seconds
  if (currentTime - lastCommandCheck >= COMMAND_CHECK_INTERVAL) {
    checkCommands();
    lastCommandCheck = currentTime;
  }
  
  // Handle pump timing
  if (pumpRunning && (currentTime - pumpStartTime >= pumpDuration * 1000)) {
    Serial.println("Pump duration completed, turning off...");
    controlPump(false);
  }
  
  // Update device status every 5 minutes
  static unsigned long lastStatusUpdate = 0;
  if (currentTime - lastStatusUpdate >= 300000) { // 5 minutes
    updateDeviceStatus();
    lastStatusUpdate = currentTime;
  }
  
  delay(1000); // Small delay to prevent watchdog issues
}

// ========================================
// WIFI SETUP
// ========================================

void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.println(WiFi.RSSI());
    blinkLED(2);
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
    blinkLED(5); // Error indicator
  }
}

// ========================================
// SENSOR READING
// ========================================

void readSensors() {
  if (!dhtInitialized) {
    Serial.println("DHT sensor not initialized!");
    return;
  }
  
  // Read DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Read soil moisture
  int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisture = map(soilMoistureRaw, SOIL_MOISTURE_DRY, SOIL_MOISTURE_WET, 0, 100);
  soilMoisture = constrain(soilMoisture, 0, 100);
  
  // Check for sensor errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT sensor!");
    temperature = 0;
    humidity = 0;
  }
  
  // Print sensor readings
  Serial.println("Sensor Readings:");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.println("%");
  Serial.print("Raw Soil Value: ");
  Serial.println(soilMoistureRaw);
  
  // Send data to dashboard
  sendSensorData(temperature, humidity, soilMoisture);
}

// ========================================
// DATA TRANSMISSION
// ========================================

void sendSensorData(float temperature, float humidity, float soilMoisture) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, cannot send data");
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/device-api";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", apiKey);
  
  // Create JSON payload
  DynamicJsonDocument doc(512);
  doc["action"] = "sensor_data";
  doc["device_id"] = deviceId;
  doc["zone_id"] = sensorZoneId;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["soil_moisture"] = soilMoisture;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.print("Sending data to: ");
  Serial.println(url);
  Serial.print("Payload: ");
  Serial.println(jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("Response: ");
    Serial.println(response);
    
    // Parse response for irrigation command
    DynamicJsonDocument responseDoc(512);
    deserializeJson(responseDoc, response);
    
    if (responseDoc.containsKey("irrigation_needed") && responseDoc["irrigation_needed"]) {
      Serial.println("Irrigation needed! Starting pump...");
      controlPump(true, 30); // Run pump for 30 seconds
    }
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

// ========================================
// COMMAND HANDLING
// ========================================

void checkCommands() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/device-api?action=commands&device_id=" + deviceId;
  
  http.begin(url);
  http.addHeader("X-API-Key", apiKey);
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Commands response: ");
    Serial.println(response);
    
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    
    if (doc.containsKey("commands") && doc["commands"].size() > 0) {
      for (JsonObject command : doc["commands"].as<JsonArray>()) {
        executeCommand(command);
      }
    }
  }
  
  http.end();
}

void executeCommand(JsonObject command) {
  String commandType = command["command_type"];
  String commandId = command["id"];
  
  Serial.print("Executing command: ");
  Serial.println(commandType);
  
  if (commandType == "pump_on") {
    int duration = command["parameters"]["duration"] | 30;
    controlPump(true, duration);
    reportCommandStatus(commandId, "executed");
  } else if (commandType == "pump_off") {
    controlPump(false);
    reportCommandStatus(commandId, "executed");
  } else if (commandType == "restart") {
    reportCommandStatus(commandId, "executed");
    delay(1000);
    ESP.restart();
  } else {
    Serial.print("Unknown command: ");
    Serial.println(commandType);
    reportCommandStatus(commandId, "failed");
  }
}

// ========================================
// PUMP CONTROL
// ========================================

void controlPump(bool turnOn, int duration) {
  if (turnOn) {
    Serial.print("Turning pump ON for ");
    Serial.print(duration);
    Serial.println(" seconds");
    digitalWrite(PUMP_PIN, HIGH);
    pumpRunning = true;
    pumpStartTime = millis();
    pumpDuration = duration;
    blinkLED(1);
  } else {
    Serial.println("Turning pump OFF");
    digitalWrite(PUMP_PIN, LOW);
    pumpRunning = false;
  }
}

// ========================================
// DEVICE STATUS
// ========================================

void updateDeviceStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/device-api";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", apiKey);
  
  DynamicJsonDocument doc(512);
  doc["action"] = "heartbeat";
  doc["device_id"] = deviceId;
  doc["status"] = "online";
  doc["ip_address"] = WiFi.localIP().toString();
  doc["signal_strength"] = WiFi.RSSI();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    Serial.println("Device status updated successfully");
  } else {
    Serial.print("Failed to update device status: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

// ========================================
// UTILITY FUNCTIONS
// ========================================

void reportCommandStatus(String commandId, String status) {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/device-api";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", apiKey);
  
  DynamicJsonDocument doc(256);
  doc["action"] = "command_status";
  doc["command_id"] = commandId;
  doc["status"] = status;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  http.PUT(jsonString);
  http.end();
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(200);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
}

// ========================================
// WIFI EVENT HANDLER
// ========================================

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("Connected to WiFi. IP: ");
      Serial.println(WiFi.localIP());
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi disconnected");
      break;
  }
} 