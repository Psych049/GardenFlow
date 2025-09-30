/*
 * ESP32 GardenFlow Controller
 * 
 * Hardware Setup:
 * - ESP32 DevKit
 * - DHT11 sensor (GPIO 4)
 * - Soil moisture sensor (GPIO 34, analog)
 * - 5V DC water pump via relay (GPIO 16)
 * 
 * Features:
 * - Sends sensor data to Supabase every 30 seconds
 * - Receives pump commands from dashboard
 * - LED status indicators
 * - Auto-reconnection and error handling
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// =====================================
// CONFIGURATION - UPDATE THESE VALUES
// =====================================

// WiFi credentials
const char* ssid = "narzo";
const char* password = "jeet1111";

// Supabase configuration
const char* supabaseUrl = "https://xarwvnfmhepbbclwkvio.supabase.co";
const char* apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InBid293dmZxbWZscXhmd3V3b3VqIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTcxNTA2ODAsImV4cCI6MjA3MjcyNjY4MH0.cFOYWbiQAQP2yKPHHUDsLxVT1ngpwy0HyJD688Nf1ko";  // Get from your dashboard

// Device identification (get these from your dashboard)
const char* deviceId = "ESP_rose";  // Your device_id string
const char* zoneId = "sk_v4d9inoyuvkqpf2rcffa7n";   // Zone UUID from dashboard

// =====================================
// HARDWARE PINS
// =====================================

// Sensors
#define DHT_PIN 4                 // DHT11 data pin
#define DHT_TYPE DHT11           // DHT11 sensor type
#define SOIL_MOISTURE_PIN 34     // Soil moisture analog pin

// Actuators  
#define PUMP_RELAY_PIN 16        // 5V DC pump relay pin

// Status LED
#define LED_BUILTIN 2            // Built-in LED

// =====================================
// OBJECTS & VARIABLES
// =====================================

// Initialize DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);

// WiFi secure client
WiFiClientSecure secureClient;

// Timing variables
unsigned long lastSensorUpdate = 0;
unsigned long lastCommandCheck = 0;
unsigned long lastLedBlink = 0;
unsigned long sensorInterval = 30000;    // Send data every 30 seconds
unsigned long commandInterval = 10000;   // Check commands every 10 seconds
unsigned long ledBlinkInterval = 1000;   // LED blink every second

// Status tracking
bool ledState = false;
bool wifiConnected = false;
bool pumpRunning = false;
unsigned long pumpStartTime = 0;
unsigned long pumpDuration = 0;

// Sensor data structure
struct SensorData {
  float temperature;
  float humidity;
  float soilMoisture;
  bool valid;
};

// =====================================
// SETUP FUNCTION
// =====================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n===========================================");
  Serial.println("    ESP32 GardenFlow Controller");
  Serial.println("    Hardware: DHT11 + Soil + 5V Pump");
  Serial.println("===========================================");
  
  initializeHardware();
  
  // Configure secure client
  secureClient.setInsecure();
  Serial.println("✓ HTTPS client configured");
  
  // Start DHT sensor
  dht.begin();
  Serial.println("✓ DHT11 sensor initialized");
  
  connectToWiFi();
  performSystemCheck();
  
  Serial.println("System Status:");
  Serial.println("• Sensor readings: Every 30 seconds");
  Serial.println("• Command check: Every 10 seconds");
  Serial.println("• Commands: Type 'help' in Serial Monitor");
  Serial.println("===========================================\n");
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
  
  // Disconnect if already connected
  WiFi.disconnect();
  delay(100);
  
  // Set WiFi mode
  WiFi.mode(WIFI_STA);
  delay(100);
  
  // Start connection
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 60) {  // Increased attempts
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    Serial.print(".");
    attempts++;
    
    // Print detailed status every 10 attempts
    if (attempts % 10 == 0) {
      Serial.printf("\nAttempt %d - WiFi Status: %d\n", attempts, WiFi.status());
      switch(WiFi.status()) {
        case WL_IDLE_STATUS:
          Serial.println("Status: IDLE");
          break;
        case WL_NO_SSID_AVAIL:
          Serial.println("Status: NO SSID AVAILABLE");
          break;
        case WL_SCAN_COMPLETED:
          Serial.println("Status: SCAN COMPLETED");
          break;
        case WL_CONNECTED:
          Serial.println("Status: CONNECTED");
          break;
        case WL_CONNECT_FAILED:
          Serial.println("Status: CONNECT FAILED");
          break;
        case WL_CONNECTION_LOST:
          Serial.println("Status: CONNECTION LOST");
          break;
        case WL_DISCONNECTED:
          Serial.println("Status: DISCONNECTED");
          break;
        default:
          Serial.printf("Status: UNKNOWN (%d)\n", WiFi.status());
      }
      Serial.print("Continuing");
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\n✓ WiFi connected!");
    Serial.printf("  SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("  IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("  Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("  DNS: %s\n", WiFi.dnsIP().toString().c_str());
    Serial.printf("  Signal: %d dBm\n", WiFi.RSSI());
    Serial.printf("  MAC: %s\n", WiFi.macAddress().c_str());
    
    // Success indication
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN, HIGH); delay(300);
      digitalWrite(LED_BUILTIN, LOW); delay(300);
    }
  } else {
    wifiConnected = false;
    Serial.println("\n✗ WiFi connection failed!");
    Serial.printf("Final status: %d\n", WiFi.status());
    
    // Scan for available networks
    Serial.println("\nScanning for WiFi networks...");
    int networkCount = WiFi.scanNetworks();
    
    if (networkCount == 0) {
      Serial.println("No networks found!");
    } else {
      Serial.printf("Found %d networks:\n", networkCount);
      for (int i = 0; i < networkCount; i++) {
        Serial.printf("  %d: %s (Signal: %d dBm, Encryption: %d)\n", 
                      i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.encryptionType(i));
        
        // Check if our target SSID is available
        if (WiFi.SSID(i) == String(ssid)) {
          Serial.printf("  *** Found target network '%s' ***\n", ssid);
        }
      }
    }
    
    // Error indication
    for (int i = 0; i < 10; i++) {
      digitalWrite(LED_BUILTIN, HIGH); delay(50);
      digitalWrite(LED_BUILTIN, LOW); delay(50);
    }
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
  // Assuming: 4095 = dry (0%), 1500 = wet (100%)
  // Adjust these values based on your sensor calibration
  data.soilMoisture = map(soilRaw, 4095, 1500, 0, 100);
  data.soilMoisture = constrain(data.soilMoisture, 0, 100);
  
  data.valid = true;
  return data;
}

// =====================================
// SUPABASE COMMUNICATION
// =====================================

void sendSensorData() {
  Serial.println("\n📊 Reading sensors...");
  
  SensorData sensorData = readSensors();
  if (!sensorData.valid) {
    Serial.println("❌ Sensor read failed, skipping upload");
    return;
  }
  
  Serial.printf("  🌡️ %.1f°C | 💧 %.1f%% | 🌱 %.1f%%\n", 
                sensorData.temperature, sensorData.humidity, sensorData.soilMoisture);
  
  // Create JSON payload for esp32-data endpoint
  DynamicJsonDocument doc(512);
  doc["device_id"] = deviceId;
  doc["zone_id"] = zoneId;
  doc["temperature"] = sensorData.temperature;
  doc["humidity"] = sensorData.humidity;
  doc["soil_moisture"] = sensorData.soilMoisture;
  doc["api_key"] = apiKey;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/esp32-data";
  
  http.begin(secureClient, url);
  http.addHeader("Content-Type", "application/json");
  
  int responseCode = http.POST(jsonString);
  
  if (responseCode >= 200 && responseCode < 300) {
    Serial.println("✅ Data sent successfully!");
  } else {
    Serial.printf("❌ Upload failed: HTTP %d\n", responseCode);
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
  String url = String(supabaseUrl) + "/functions/v1/esp32-commands?device_id=" + deviceId + "&api_key=" + apiKey;
  
  http.begin(secureClient, url);
  
  int responseCode = http.GET();
  
  if (responseCode >= 200 && responseCode < 300) {
    String response = http.getString();
    
    if (response.length() > 10) {  // More than just empty response
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, response);
      
      if (!error && doc["success"] == true) {
        JsonArray commands = doc["commands"];
        
        if (commands.size() > 0) {
          JsonObject command = commands[0];  // Get first command
          
          String commandId = command["id"] | "";
          String commandType = command["command_type"] | "";
          int duration = command["parameters"]["duration"] | 30;
          
          if (commandId.length() > 0 && commandType.length() > 0) {
            Serial.printf("⚡ New command: %s (ID: %s)\n", commandType.c_str(), commandId.c_str());
            executeCommand(commandId, commandType, duration);
          }
        } else {
          Serial.println("ℹ️ No pending commands");
        }
      } else {
        Serial.println("⚠ Command parse error");
      }
    } else {
      Serial.println("ℹ️ No commands in queue");
    }
  } else {
    Serial.printf("❌ Command check failed: HTTP %d\n", responseCode);
  }
  
  http.end();
}

// =====================================
// COMMAND EXECUTION
// =====================================

void executeCommand(String commandId, String commandType, int duration) {
  Serial.printf("⚡ Executing: %s for %d seconds\n", commandType.c_str(), duration);
  
  // Update command status to executing
  updateCommandStatus(commandId, "executing", "Command started");
  
  if (commandType == "pump_on" || commandType == "water") {
    startPump(duration);
    Serial.println("💦 Water pump ON");
  } else if (commandType == "pump_off") {
    stopPump();
    Serial.println("💦 Water pump OFF");
    updateCommandStatus(commandId, "executed", "Pump stopped");
  } else if (commandType == "read_sensors") {
    // Force immediate sensor reading
    sendSensorData();
    updateCommandStatus(commandId, "executed", "Sensor reading completed");
  } else {
    Serial.printf("⚠ Unknown command: %s\n", commandType.c_str());
    updateCommandStatus(commandId, "failed", "Unknown command type");
  }
}

void startPump(int duration) {
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  pumpRunning = true;
  pumpStartTime = millis();
  pumpDuration = duration * 1000;  // Convert to milliseconds
}

void stopPump() {
  digitalWrite(PUMP_RELAY_PIN, LOW);
  pumpRunning = false;
}

void handlePumpOperation() {
  if (pumpRunning && (millis() - pumpStartTime >= pumpDuration)) {
    stopPump();
    Serial.println("⏰ Pump auto-stop (duration completed)");
    // Note: Command status update would need command ID tracking for auto-stop
  }
}

void updateCommandStatus(String commandId, String status, String result) {
  DynamicJsonDocument doc(512);
  doc["command_id"] = commandId;
  doc["status"] = status;
  doc["result"] = result;
  doc["api_key"] = apiKey;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  HTTPClient http;
  String url = String(supabaseUrl) + "/functions/v1/esp32-commands";
  
  http.begin(secureClient, url);
  http.addHeader("Content-Type", "application/json");
  
  int responseCode = http.sendRequest("PUT", jsonString);
  
  if (responseCode >= 200 && responseCode < 300) {
    Serial.printf("✓ Command status updated: %s\n", status.c_str());
  } else {
    Serial.printf("❌ Failed to update command status: HTTP %d\n", responseCode);
  }
  
  http.end();
}

// =====================================
// UTILITY & DEBUG FUNCTIONS
// =====================================

void scanWiFiNetworks() {
  Serial.println("\n=== WiFi Network Scan ===");
  
  int networkCount = WiFi.scanNetworks();
  
  if (networkCount == 0) {
    Serial.println("No networks found!");
  } else {
    Serial.printf("Found %d networks:\n\n", networkCount);
    
    for (int i = 0; i < networkCount; i++) {
      Serial.printf("%2d: %-20s | Signal: %3d dBm | Ch: %2d | Enc: ", 
                    i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i));
      
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:
          Serial.println("Open");
          break;
        case WIFI_AUTH_WEP:
          Serial.println("WEP");
          break;
        case WIFI_AUTH_WPA_PSK:
          Serial.println("WPA");
          break;
        case WIFI_AUTH_WPA2_PSK:
          Serial.println("WPA2");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          Serial.println("WPA/WPA2");
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          Serial.println("WPA2 Enterprise");
          break;
        default:
          Serial.println("Unknown");
      }
      
      // Highlight our target network
      if (WiFi.SSID(i) == String(ssid)) {
        Serial.println("    *** THIS IS YOUR TARGET NETWORK ***");
      }
    }
  }
  
  Serial.println("=========================\n");
}

void printWiFiConfig() {
  Serial.println("\n=== WiFi Configuration ===");
  Serial.printf("Target SSID: '%s'\n", ssid);
  Serial.printf("Password length: %d characters\n", strlen(password));
  Serial.printf("Current status: %d (", WiFi.status());
  
  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      Serial.print("IDLE");
      break;
    case WL_NO_SSID_AVAIL:
      Serial.print("NO SSID AVAILABLE");
      break;
    case WL_SCAN_COMPLETED:
      Serial.print("SCAN COMPLETED");
      break;
    case WL_CONNECTED:
      Serial.print("CONNECTED");
      break;
    case WL_CONNECT_FAILED:
      Serial.print("CONNECT FAILED");
      break;
    case WL_CONNECTION_LOST:
      Serial.print("CONNECTION LOST");
      break;
    case WL_DISCONNECTED:
      Serial.print("DISCONNECTED");
      break;
    default:
      Serial.print("UNKNOWN");
  }
  Serial.println(")");
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Connected SSID: '%s'\n", WiFi.SSID().c_str());
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("DNS: %s\n", WiFi.dnsIP().toString().c_str());
    Serial.printf("Signal Strength: %d dBm\n", WiFi.RSSI());
  }
  
  Serial.printf("ESP32 MAC: %s\n", WiFi.macAddress().c_str());
  Serial.println("=========================\n");
}

void printSystemStatus() {
  Serial.println("\n======== SYSTEM STATUS ========");
  Serial.printf("WiFi: %s\n", wifiConnected ? "✓ Connected" : "✗ Disconnected");
  if (wifiConnected) {
    Serial.printf("IP: %s | Signal: %d dBm\n", 
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
  }
  Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
  Serial.printf("Free memory: %d bytes\n", ESP.getFreeHeap());
  
  Serial.printf("Pump: %s\n", pumpRunning ? "🟢 ON" : "🔴 OFF");
  if (pumpRunning) {
    unsigned long remaining = (pumpDuration - (millis() - pumpStartTime)) / 1000;
    Serial.printf("Remaining: %lu seconds\n", remaining);
  }
  
  SensorData data = readSensors();
  if (data.valid) {
    Serial.printf("Temperature: %.1f°C\n", data.temperature);
    Serial.printf("Humidity: %.1f%%\n", data.humidity);
    Serial.printf("Soil Moisture: %.1f%%\n", data.soilMoisture);
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
      SensorData data = readSensors();
      if (data.valid) {
        Serial.printf("🌡️ Temp: %.1f°C | 💧 Humidity: %.1f%% | 🌱 Soil: %.1f%%\n", 
                      data.temperature, data.humidity, data.soilMoisture);
      } else {
        Serial.println("❌ Sensor read failed");
      }
    } else if (command == "send") {
      sendSensorData();
    } else if (command == "commands") {
      checkForCommands();
    } else if (command == "wifi") {
      connectToWiFi();
    } else if (command == "scan") {
      scanWiFiNetworks();
    } else if (command == "config") {
      printWiFiConfig();
    } else if (command == "pump_on") {
      startPump(30);  // 30 seconds
      Serial.println("💦 Manual pump ON (30s)");
    } else if (command == "pump_off") {
      stopPump();
      Serial.println("💦 Manual pump OFF");
    } else if (command == "help") {
      Serial.println("\nAvailable commands:");
      Serial.println("  status   - Show system status");
      Serial.println("  sensors  - Read all sensors");
      Serial.println("  send     - Send sensor data");
      Serial.println("  commands - Check for commands");
      Serial.println("  wifi     - Reconnect WiFi");
      Serial.println("  scan     - Scan WiFi networks");
      Serial.println("  config   - Show WiFi config");
      Serial.println("  pump_on  - Turn pump ON (30s)");
      Serial.println("  pump_off - Turn pump OFF");
      Serial.println("  help     - Show this help\n");
    } else if (command.length() > 0) {
      Serial.println("Unknown command. Type 'help' for options.");
    }
  }
}