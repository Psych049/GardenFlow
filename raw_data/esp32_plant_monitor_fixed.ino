/*
 * ESP32 Plant Monitoring System - FIXED VERSION
 * 
 * Issues Fixed:
 * - DHT sensor connection and reading
 * - Edge Function URLs and API calls
 * - Zone ID handling
 * - Error handling and debugging
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

// WiFi Configuration
const char* ssid = "Oppo..";
const char* password = "alphaxyz@1234";

// Supabase Configuration
const char* supabaseUrl = "https://gqzaxkczxcudxbbkudmm.supabase.co";
const char* supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImdxemF4a2N6eGN1ZHhiYmt1ZG1tIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTM3MzcwNzEsImV4cCI6MjA2OTMxMzA3MX0.RR4jib8iRkZG1rqFpH3wuTE82BY5ViJKFR0FVvu5N4U";

// Device Authentication
const char* apiKey = "IwSjoT2a5eB53roAlmVJqMd8NZS1tuhp";

// Device Configuration
const String deviceId = "ESP32_PLANT_MONITOR_001";
const String deviceName = "Garden Monitor 1";
const String sensorZoneId = "ZONE_001"; // This should match your zone ID

// Pin Definitions
#define DHT_PIN 4
#define SOIL_MOISTURE_PIN 36
#define PUMP_PIN 5
#define STATUS_LED_PIN 2

// Sensor Configuration
#define DHT_TYPE DHT22
#define SOIL_MOISTURE_DRY 4095    // Calibrate these values
#define SOIL_MOISTURE_WET 1500    // for your specific sensor
#define SENSOR_READ_INTERVAL 30000 // 30 seconds
#define COMMAND_CHECK_INTERVAL 10000 // 10 seconds

// Global Variables
DHT dht(DHT_PIN, DHT_TYPE);
WiFiClientSecure client;
bool pumpRunning = false;
unsigned long lastSensorRead = 0;
unsigned long lastCommandCheck = 0;
unsigned long pumpStartTime = 0;
int pumpDuration = 30; // seconds
bool dhtInitialized = false;

// Function Declarations
void setupWiFi();
void readSensors();
void sendSensorData(float temperature, float humidity, float soilMoisture);
void checkCommands();
void executeCommand(JsonObject command);
void controlPump(bool turnOn, int duration = 30);
void reportCommandStatus(String commandId, String status);
void updateDeviceStatus();
void blinkLED(int times);
void WiFiEvent(WiFiEvent_t event);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32 Plant Monitor - Starting up...");
  
  // Initialize pins
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Initialize DHT sensor with delay
  Serial.println("Initializing DHT sensor...");
  dht.begin();
  delay(2000); // Give DHT sensor time to initialize
  dhtInitialized = true;
  
  // Setup WiFi
  setupWiFi();
  
  // Register device
  updateDeviceStatus();
  
  Serial.println("ESP32 Plant Monitor initialized!");
  blinkLED(3);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read sensors every SENSOR_READ_INTERVAL
  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    readSensors();
    lastSensorRead = currentTime;
  }
  
  // Check for commands every COMMAND_CHECK_INTERVAL
  if (currentTime - lastCommandCheck >= COMMAND_CHECK_INTERVAL) {
    checkCommands();
    lastCommandCheck = currentTime;
  }
  
  // Handle pump timing
  if (pumpRunning && (currentTime - pumpStartTime >= pumpDuration * 1000)) {
    controlPump(false);
  }
  
  delay(1000); // Small delay to prevent watchdog issues
}

void setupWiFi() {
  Serial.println("Setting up WiFi...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    blinkLED(2);
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
    blinkLED(5); // Error indicator
  }
}

void readSensors() {
  Serial.println("Reading sensors...");
  
  float temperature = 0;
  float humidity = 0;
  float soilMoisture = 0;
  
  // Read DHT sensor with error handling
  if (dhtInitialized) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read DHT sensor!");
      temperature = 0;
      humidity = 0;
    } else {
      Serial.printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
    }
  } else {
    Serial.println("DHT sensor not initialized!");
  }
  
  // Read soil moisture sensor
  int rawMoisture = analogRead(SOIL_MOISTURE_PIN);
  soilMoisture = map(rawMoisture, SOIL_MOISTURE_DRY, SOIL_MOISTURE_WET, 0, 100);
  soilMoisture = constrain(soilMoisture, 0, 100);
  
  Serial.printf("Soil Moisture: %.1f%% (Raw: %d)\n", soilMoisture, rawMoisture);
  
  // Send data to Supabase
  if (WiFi.status() == WL_CONNECTED) {
    sendSensorData(temperature, humidity, soilMoisture);
  } else {
    Serial.println("WiFi not connected, cannot send data");
  }
}

void sendSensorData(float temperature, float humidity, float soilMoisture) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/esp32-data";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
  
  // Create JSON payload
  DynamicJsonDocument doc(512);
  doc["sensor_id"] = sensorZoneId;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["soil_moisture"] = soilMoisture;
  doc["apiKey"] = apiKey;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("Sending sensor data...");
  Serial.println(jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Sensor data response: " + response);
  } else {
    Serial.printf("Error sending sensor data: %d\n", httpResponseCode);
  }
  
  http.end();
}

void checkCommands() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/esp32-commands?device_id=" + deviceId + "&apiKey=" + apiKey;
  
  http.begin(url);
  http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Commands response: " + response);
    
    // Parse response
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      JsonArray commands = doc["commands"];
      if (commands) {
        for (JsonObject command : commands) {
          executeCommand(command);
        }
      }
    }
  } else {
    Serial.printf("Error checking commands: %d\n", httpResponseCode);
  }
  
  http.end();
}

void executeCommand(JsonObject command) {
  String commandType = command["command_type"];
  String commandId = command["id"];
  
  Serial.printf("Executing command: %s\n", commandType.c_str());
  
  if (commandType == "pump_on") {
    int duration = command["parameters"]["duration"] | 30;
    controlPump(true, duration);
    reportCommandStatus(commandId, "executed");
  } else if (commandType == "pump_off") {
    controlPump(false);
    reportCommandStatus(commandId, "executed");
  } else if (commandType == "auto_mode") {
    // Enable auto mode
    reportCommandStatus(commandId, "executed");
  } else if (commandType == "manual_mode") {
    // Enable manual mode
    reportCommandStatus(commandId, "executed");
  } else {
    Serial.printf("Unknown command type: %s\n", commandType.c_str());
    reportCommandStatus(commandId, "failed");
  }
}

void controlPump(bool turnOn, int duration) {
  if (turnOn) {
    digitalWrite(PUMP_PIN, HIGH);
    pumpRunning = true;
    pumpStartTime = millis();
    pumpDuration = duration;
    Serial.printf("Pump turned ON for %d seconds\n", duration);
    blinkLED(1);
  } else {
    digitalWrite(PUMP_PIN, LOW);
    pumpRunning = false;
    Serial.println("Pump turned OFF");
  }
}

void reportCommandStatus(String commandId, String status) {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/esp32-commands";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
  
  DynamicJsonDocument doc(256);
  doc["command_id"] = commandId;
  doc["status"] = status;
  doc["apiKey"] = apiKey;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.PUT(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.printf("Command status response: %s\n", response.c_str());
  } else {
    Serial.printf("Error reporting command status: %d\n", httpResponseCode);
  }
  
  http.end();
}

void updateDeviceStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/device-management";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
  
  DynamicJsonDocument doc(512);
  doc["device_id"] = deviceId;
  doc["name"] = deviceName;
  doc["status"] = "online";
  doc["firmware_version"] = "1.0.0";
  doc["ip_address"] = WiFi.localIP().toString();
  doc["apiKey"] = apiKey;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Device status response: " + response);
  } else {
    Serial.printf("Error updating device status: %d\n", httpResponseCode);
  }
  
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

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected, IP: " + WiFi.localIP().toString());
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi disconnected");
      break;
  }
} 