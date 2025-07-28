/*
 * ESP32 Plant Monitoring System
 * 
 * Features:
 * - DHT22 temperature and humidity sensor
 * - Capacitive soil moisture sensor
 * - Relay control for water pump
 * - WiFi connectivity
 * - Data transmission to Supabase
 * - Command reception from dashboard
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

// Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* supabaseUrl = "YOUR_SUPABASE_URL";
const char* supabaseAnonKey = "YOUR_SUPABASE_ANON_KEY";
const char* apiKey = "YOUR_API_KEY"; // Generate this in your dashboard

// Device Configuration
const String deviceId = "ESP32_PLANT_MONITOR_001";
const String deviceName = "Garden Monitor 1";
const int sensorZoneId = "ZONE_001"; // Match this with your zone configuration

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

// Function Declarations
void setupWiFi();
void readSensors();
void sendSensorData();
void checkCommands();
void executeCommand(JsonDocument& command);
void controlPump(bool turnOn, int duration = 30);
void updateDeviceStatus();
void blinkLED(int times);

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Initialize sensors
  dht.begin();
  
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
    sendSensorData();
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
  
  // Update device status every 5 minutes
  static unsigned long lastStatusUpdate = 0;
  if (currentTime - lastStatusUpdate >= 300000) { // 5 minutes
    updateDeviceStatus();
    lastStatusUpdate = currentTime;
  }
  
  delay(1000);
}

void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    blinkLED(1);
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Configure SSL client
  client.setInsecure();
}

void readSensors() {
  Serial.println("Reading sensors...");
  
  // Read DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Read soil moisture
  int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisturePercent = map(soilMoistureRaw, SOIL_MOISTURE_DRY, SOIL_MOISTURE_WET, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
  
  // Check for sensor errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT sensor!");
    return;
  }
  
  Serial.printf("Temperature: %.1f°C, Humidity: %.1f%%, Soil Moisture: %.1f%%\n", 
                temperature, humidity, soilMoisturePercent);
  
  // Send data to Supabase
  sendSensorData(temperature, humidity, soilMoisturePercent);
}

void sendSensorData(float temperature, float humidity, float soilMoisture) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/esp32-data";
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
  
  // Create JSON payload
  JsonDocument doc;
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
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
    
    // Parse response for irrigation needs
    JsonDocument responseDoc;
    deserializeJson(responseDoc, response);
    
    if (responseDoc.containsKey("irrigation_needed") && responseDoc["irrigation_needed"]) {
      Serial.println("Irrigation needed - starting pump");
      controlPump(true, 30);
    }
  } else {
    Serial.println("Error sending data: " + http.errorToString(httpResponseCode));
  }
  
  http.end();
}

void checkCommands() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/esp32-commands?device_id=" + deviceId + "&apiKey=" + apiKey;
  
  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Commands response: " + response);
    
    JsonDocument doc;
    deserializeJson(doc, response);
    
    if (doc.containsKey("commands")) {
      JsonArray commands = doc["commands"];
      
      for (JsonObject command : commands) {
        executeCommand(command);
      }
    }
  }
  
  http.end();
}

void executeCommand(JsonDocument& command) {
  String commandType = command["command_type"];
  String commandId = command["id"];
  
  Serial.println("Executing command: " + commandType);
  
  bool success = false;
  
  if (commandType == "pump_on") {
    int duration = command["parameters"]["duration"] | 30;
    controlPump(true, duration);
    success = true;
  } else if (commandType == "pump_off") {
    controlPump(false);
    success = true;
  } else if (commandType == "auto_mode") {
    // Enable auto mode (handled by sensor readings)
    success = true;
  } else if (commandType == "manual_mode") {
    // Disable auto mode
    success = true;
  }
  
  // Report command execution status
  if (success) {
    reportCommandStatus(commandId, "executed");
  } else {
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
    blinkLED(2);
  } else {
    digitalWrite(PUMP_PIN, LOW);
    pumpRunning = false;
    Serial.println("Pump turned OFF");
    blinkLED(1);
  }
}

void reportCommandStatus(String commandId, String status) {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/esp32-commands";
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
  
  JsonDocument doc;
  doc["command_id"] = commandId;
  doc["status"] = status;
  doc["apiKey"] = apiKey;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.PUT(jsonString);
  
  if (httpResponseCode > 0) {
    Serial.println("Command status reported: " + status);
  }
  
  http.end();
}

void updateDeviceStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/device-management";
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
  
  JsonDocument doc;
  doc["device_id"] = deviceId;
  doc["name"] = deviceName;
  doc["device_type"] = "esp32";
  doc["ip_address"] = WiFi.localIP().toString();
  doc["mac_address"] = WiFi.macAddress();
  doc["firmware_version"] = "1.0.0";
  doc["apiKey"] = apiKey;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    Serial.println("Device status updated");
  }
  
  http.end();
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(100);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(100);
  }
}

// WiFi event handler
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("Connected to WiFi and got IP");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi");
      break;
  }
} 