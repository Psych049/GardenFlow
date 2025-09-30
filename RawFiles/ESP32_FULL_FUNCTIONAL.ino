/*
 * ESP32 GardenFlow - Complete IoT Controller
 * 
 * Fully functional ESP32 code that:
 * - Sends sensor data to Supabase database
 * - Receives and executes actuator commands from dashboard
 * - Handles bidirectional HTTPS communication
 * - Provides comprehensive status monitoring
 * - Supports modular sensor and actuator architecture
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


// Device IDs - You need to replace these with actual UUIDs from your dashboard
// For now, using placeholder values that will need to be updated
const char* deviceId = "esp32-device-001";  // Replace with your actual device UUID
const char* zoneId = "garden-zone-001";    // Replace with your actual zone UUID  
const char* userId = "user-001";           // Replace with your actual user UUID

// =====================================
// HARDWARE CONFIGURATION
// =====================================

// WiFi secure client
WiFiClientSecure secureClient;

// LED pin
#define LED_BUILTIN 2


// Actuator pins - Only water pump is available
#define WATER_PUMP_PIN 16
// Sensor pins
#define DHT_PIN 4
#define SOIL_MOISTURE_PIN 34
#define LIGHT_SENSOR_PIN 35
#define PH_SENSOR_PIN 36
#define FLOW_SENSOR_PIN 5

// =====================================
// TIMING & STATUS
// =====================================

unsigned long lastSensorUpdate = 0;
unsigned long lastCommandCheck = 0;
unsigned long lastLedBlink = 0;
unsigned long sensorInterval = 15000;   // 15 seconds
unsigned long commandInterval = 5000;   // 5 seconds
unsigned long ledBlinkInterval = 1000;  // 1 second

bool ledState = false;
bool wifiConnected = false;
bool systemHealthy = true;

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

struct PendingCommand {
  String commandId;
  String commandType;
  unsigned long executionTime;
  unsigned long duration;
  bool isActive;
};

PendingCommand currentCommand = {"", "", 0, 0, false};

// =====================================
// SETUP FUNCTION
// =====================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n===========================================");
  Serial.println("    ESP32 GardenFlow IoT Controller");
  Serial.println("===========================================");
  
  initializePins();
  
  secureClient.setInsecure();
  Serial.println("✓ WiFiClientSecure configured");
  
  connectToWiFi();
  performSystemCheck();
  
  Serial.println("System Ready:");
  Serial.println("• Sensor data: Every 15 seconds");
  Serial.println("• Command check: Every 5 seconds");
  Serial.println("• Commands: Type 'help' in Serial");
  Serial.println("==========================================\n");
}

// =====================================
// MAIN LOOP
// =====================================

void loop() {
  checkWiFiConnection();
  handleStatusLED();
  
  // Send sensor data
  if (millis() - lastSensorUpdate >= sensorInterval) {
    if (wifiConnected) {
      sendSensorData();
    } else {
      Serial.println("⚠ Skipping sensor data - WiFi disconnected");
    }
    lastSensorUpdate = millis();
  }
  
  // Check for commands
  if (millis() - lastCommandCheck >= commandInterval) {
    if (wifiConnected) {
      checkForCommands();
    }
    lastCommandCheck = millis();
  }
  
  // Handle pump timing
  handlePumpOperation();
  
  // Handle serial commands
  handleSerialCommands();
  
  delay(100);
}

// =====================================
// INITIALIZATION
// =====================================

void initializeHardware() {
  Serial.println("Initializing hardware...");
  
  // LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Pump relay (ACTIVE HIGH for most relay modules)
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);  // Pump OFF
  
  // Sensors (analog pins don't need pinMode)
  Serial.println("✓ Hardware pins initialized");
}

void performSystemCheck() {
  Serial.println("\nSystem check...");
  
  // Test pump briefly
  Serial.println("Testing pump relay...");
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  delay(500);
  digitalWrite(PUMP_RELAY_PIN, LOW);
  Serial.println("✓ Pump relay tested");
  
  // Read initial sensor values
  SensorData data = readSensors();
  if (data.valid) {
    Serial.printf("• Temperature: %.1f°C\n", data.temperature);
    Serial.printf("• Humidity: %.1f%%\n", data.humidity);
    Serial.printf("• Soil Moisture: %.1f%%\n", data.soilMoisture);
  } else {
    Serial.println("⚠ Initial sensor read failed");
  }
  
  Serial.println("✓ System check complete\n");
}

// =====================================
// WIFI & CONNECTION
// =====================================

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED && wifiConnected) {
    Serial.println("\n⚠ WiFi lost! Reconnecting...");
    wifiConnected = false;
    connectToWiFi();
  }
}

void handleStatusLED() {
  if (millis() - lastLedBlink >= ledBlinkInterval) {
    if (wifiConnected) {
      if (pumpRunning) {
        // Fast blink when pump is running
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
        delay(100);
      } else {
        // Normal blink when connected
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
      }
    } else {
      // Very fast blink when disconnected
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
      delay(50);
    }
    lastLedBlink = millis();
  }
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
    Serial.println("\n✓ WiFi connected!");
    Serial.printf("  IP: %s | Signal: %d dBm\n", 
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
    
    // Success indication
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN, HIGH); delay(300);
      digitalWrite(LED_BUILTIN, LOW); delay(300);
    }
  } else {
    wifiConnected = false;
    Serial.println("\n✗ WiFi failed!");
  }
}

// =====================================
// SENSOR READING
// =====================================

SensorData readSensors() {
  SensorData data;
  data.valid = false;
  
  // Read DHT11 (temperature & humidity)
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();
  
  // Check if DHT readings are valid
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("⚠ DHT11 read failed!");
    return data;
  }
  
  // Read soil moisture (analog 0-4095, convert to percentage)
  int soilRaw = analogRead(SOIL_MOISTURE_PIN);
  // Assuming: 4095 = dry (0%), 0 = wet (100%)
  // Adjust these values based on your sensor calibration
  data.soilMoisture = map(soilRaw, 4095, 1500, 0, 100);
  data.soilMoisture = constrain(data.soilMoisture, 0, 100);
  
  data.valid = true;
  return data;
}

// =====================================
// SUPABASE COMMUNICATION
// =====================================

void sendSensorDataToSupabase() {
  Serial.println("\n📊 Sending sensor data...");
  
  SensorData sensorData = readAllSensors();
  
  DynamicJsonDocument doc(1024);
  doc["device_id"] = deviceId;
  doc["zone_id"] = zoneId;
  doc["user_id"] = userId;
  doc["temperature"] = sensorData.temperature;
  doc["humidity"] = sensorData.humidity;
  doc["soil_moisture"] = sensorData.soilMoisture;
  doc["light_level"] = sensorData.lightLevel;
  doc["ph_level"] = sensorData.phLevel;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/rest/v1/sensor_data";
  
  http.begin(secureClient, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", String("Bearer ") + supabaseAnonKey);
  http.addHeader("Prefer", "return=minimal");
  
  int responseCode = http.POST(jsonString);
  
  if (responseCode >= 200 && responseCode < 300) {
    Serial.println("✅ Data sent successfully!");
    Serial.printf("  🌡️ %.1f°C | 💧 %.1f%% | 🌱 %.1f%% | 💡 %d\n", 
                  sensorData.temperature, sensorData.humidity, 
                  sensorData.soilMoisture, sensorData.lightLevel);
  } else {
    Serial.printf("❌ Error: HTTP %d\n", responseCode);
    String response = http.getString();
    if (response.length() > 0) {
      Serial.println("Response: " + response);
    }
  }
  
  http.end();
}

void checkForCommands() {
  Serial.println("\n🔍 Checking for commands...");
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/rest/v1/commands?device_id=eq." + deviceId + "&status=eq.pending&select=*";
  
  http.begin(secureClient, url);
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", String("Bearer ") + supabaseAnonKey);
  
  int responseCode = http.GET();
  
  if (responseCode >= 200 && responseCode < 300) {
    String response = http.getString();
    Serial.printf("Response code: %d\n", responseCode);
    
    if (response.length() > 2) {  // More than just "[]"
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, response);
      
      if (!error && doc.is<JsonArray>() && doc.size() > 0) {
        JsonObject command = doc[0];  // Get first pending command
        
        String commandId = command["id"] | "";
        String commandType = command["command_type"] | "";
        int duration = command["duration"] | 30;
        
        if (commandId.length() > 0 && commandType.length() > 0) {
          Serial.printf("⚙️ New command: %s (ID: %s)\n", commandType.c_str(), commandId.c_str());
          executeCommand(commandId, commandType, duration);
        }
      } else {
        Serial.println("ℹ️ No pending commands");
      }
    } else {
      Serial.println("ℹ️ No commands in queue");
    }
  } else {
    Serial.printf("❌ Command check failed: HTTP %d\n", responseCode);
    String response = http.getString();
    if (response.length() > 0) {
      Serial.println("Error: " + response);
    }
  }
  
  http.end();
}

// =====================================
// COMMAND EXECUTION
// =====================================

void executeCommand(String commandId, String commandType, int duration) {
  Serial.printf("⚡ Executing: %s for %d seconds\n", commandType.c_str(), duration);
  
  // Mark command as in progress
  updateCommandStatus(commandId, "executing", "Command started");
  
  // Set up command execution
  currentCommand.commandId = commandId;
  currentCommand.commandType = commandType;
  currentCommand.executionTime = millis();
  currentCommand.duration = duration * 1000;  // Convert to milliseconds
  currentCommand.isActive = true;
  
  // Start the actuator
  if (commandType == "water_pump") {
    digitalWrite(WATER_PUMP_PIN, HIGH);
    actuators.waterPump = true;
    Serial.println("💦 Water pump ON");
  } else if (commandType == "fan") {
    digitalWrite(FAN_PIN, HIGH);
    actuators.fan = true;
    Serial.println("🌬️ Fan ON");
  } else if (commandType == "heater") {
    digitalWrite(HEATER_PIN, HIGH);
    actuators.heater = true;
    Serial.println("🔥 Heater ON");
  } else if (commandType == "led_strip") {
    digitalWrite(LED_STRIP_PIN, HIGH);
    actuators.ledStrip = true;
    Serial.println("💡 LED strip ON");
  } else if (commandType == "valve") {
    digitalWrite(VALVE_PIN, HIGH);
    actuators.valve = true;
    Serial.println("🗺️ Valve ON");
  } else if (commandType == "read_sensors") {
    // Immediate sensor reading
    sendSensorDataToSupabase();
    updateCommandStatus(commandId, "completed", "Sensor reading completed");
    currentCommand.isActive = false;
    return;
  } else {
    Serial.printf("⚠ Unknown command: %s\n", commandType.c_str());
    updateCommandStatus(commandId, "failed", "Unknown command type");
    currentCommand.isActive = false;
    return;
  }
}

void handleActiveCommand() {
  if (!currentCommand.isActive) return;
  
  // Check if command duration has elapsed
  if (millis() - currentCommand.executionTime >= currentCommand.duration) {
    Serial.printf("✓ Command '%s' completed\n", currentCommand.commandType.c_str());
    
    // Turn off the actuator
    if (currentCommand.commandType == "water_pump") {
      digitalWrite(WATER_PUMP_PIN, LOW);
      actuators.waterPump = false;
      Serial.println("💦 Water pump OFF");
    } else if (currentCommand.commandType == "fan") {
      digitalWrite(FAN_PIN, LOW);
      actuators.fan = false;
      Serial.println("🌬️ Fan OFF");
    } else if (currentCommand.commandType == "heater") {
      digitalWrite(HEATER_PIN, LOW);
      actuators.heater = false;
      Serial.println("🔥 Heater OFF");
    } else if (currentCommand.commandType == "led_strip") {
      digitalWrite(LED_STRIP_PIN, LOW);
      actuators.ledStrip = false;
      Serial.println("💡 LED strip OFF");
    } else if (currentCommand.commandType == "valve") {
      digitalWrite(VALVE_PIN, LOW);
      actuators.valve = false;
      Serial.println("🗺️ Valve OFF");
    }
    
    // Update command status
    updateCommandStatus(currentCommand.commandId, "completed", "Command executed successfully");
    currentCommand.isActive = false;
  }
}

void updateCommandStatus(String commandId, String status, String result) {
  DynamicJsonDocument doc(512);
  doc["status"] = status;
  doc["result"] = result;
  doc["executed_at"] = "now()";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/rest/v1/commands?id=eq." + commandId;
  
  http.begin(secureClient, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", String("Bearer ") + supabaseAnonKey);
  http.addHeader("Prefer", "return=minimal");
  
  int responseCode = http.sendRequest("PATCH", jsonString);
  
  if (responseCode >= 200 && responseCode < 300) {
    Serial.printf("✓ Command status updated: %s\n", status.c_str());
  } else {
    Serial.printf("❌ Failed to update command status: HTTP %d\n", responseCode);
  }
  
  http.end();
}

// =====================================
// ACTUATOR CONTROL
// =====================================

void setAllActuators(bool state) {
  digitalWrite(WATER_PUMP_PIN, state ? HIGH : LOW);
  digitalWrite(FAN_PIN, state ? HIGH : LOW);
  digitalWrite(HEATER_PIN, state ? HIGH : LOW);
  digitalWrite(LED_STRIP_PIN, state ? HIGH : LOW);
  digitalWrite(VALVE_PIN, state ? HIGH : LOW);
  
  actuators.waterPump = state;
  actuators.fan = state;
  actuators.heater = state;
  actuators.ledStrip = state;
  actuators.valve = state;
}

void emergencyStop() {
  Serial.println("⚠ EMERGENCY STOP - All actuators OFF");
  setAllActuators(false);
  currentCommand.isActive = false;
}

// =====================================
// UTILITY & DEBUG FUNCTIONS
// =====================================

void printSystemStatus() {
  Serial.println("\n======== SYSTEM STATUS ========");
  Serial.printf("WiFi: %s\n", wifiConnected ? "✓ Connected" : "✗ Disconnected");
  if (wifiConnected) {
    Serial.printf("IP: %s | Signal: %d dBm\n", 
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
  }
  Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
  Serial.printf("Free memory: %d bytes\n", ESP.getFreeHeap());
  
  Serial.println("\nActuator Status:");
  Serial.printf("💦 Pump: %s | 🌬️ Fan: %s | 🔥 Heater: %s\n", 
                actuators.waterPump ? "ON" : "OFF",
                actuators.fan ? "ON" : "OFF",
                actuators.heater ? "ON" : "OFF");
  Serial.printf("💡 LED: %s | 🗺️ Valve: %s\n", 
                actuators.ledStrip ? "ON" : "OFF",
                actuators.valve ? "ON" : "OFF");
  
  if (currentCommand.isActive) {
    unsigned long remaining = (currentCommand.duration - (millis() - currentCommand.executionTime)) / 1000;
    Serial.printf("\nActive Command: %s (%lu sec remaining)\n", 
                  currentCommand.commandType.c_str(), remaining);
  }
  
  Serial.println("==============================\n");
}

void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "status") {
      printSystemStatus();
    } else if (command == "sensors") {
      SensorData data = readAllSensors();
      Serial.printf("Temp: %.1f°C | Humidity: %.1f%% | Soil: %.1f%%\n", 
                    data.temperature, data.humidity, data.soilMoisture);
      Serial.printf("Light: %d | pH: %.1f | Water: %.2fL\n", 
                    data.lightLevel, data.phLevel, data.waterUsage);
    } else if (command == "send") {
      sendSensorDataToSupabase();
    } else if (command == "commands") {
      checkForCommands();
    } else if (command == "wifi") {
      connectToWiFi();
    } else if (command == "stop") {
      emergencyStop();
    } else if (command == "test") {
      testActuators();
    } else if (command == "help") {
      Serial.println("\nAvailable commands:");
      Serial.println("  status   - Show system status");
      Serial.println("  sensors  - Read all sensors");
      Serial.println("  send     - Send sensor data");
      Serial.println("  commands - Check for commands");
      Serial.println("  wifi     - Reconnect WiFi");
      Serial.println("  stop     - Emergency stop all");
      Serial.println("  test     - Test actuators");
      Serial.println("  help     - Show this help\n");
    } else if (command.length() > 0) {
      Serial.println("Unknown command. Type 'help' for options.");
    }
  }
}