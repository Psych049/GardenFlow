```cpp
/*
 * ESP32 GardenFlow Dashboard - Secure Sensor Data Transmission
 * 
 * Features:
 * - Secure HTTPS communication with Supabase
 * - Modular sensor data structure
 * - Real-time data insertion to sensor_data table
 * - Visual status indicators via onboard LED
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "vivo";
const char* password = "abcdefgh";

// Supabase configuration
const char* supabaseUrl = "https://xarwvnfmhepbbclwkvio.supabase.co";
const char* supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Inhhcnd2bmZtaGVwYmJjbHdrdmlvIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTI5Mzc1ODMsImV4cCI6MjA2ODUxMzU4M30.a9lXOmLPwBYbvLcpoB-N1-Y95augwnWrR-0L2bIyim0";

// Test device and zone IDs (replace with actual UUIDs from your dashboard)
const char* deviceId = "550e8400-e29b-41d4-a716-446655440000";  // Example UUID
const char* zoneId = "550e8400-e29b-41d4-a716-446655440001";   // Example UUID  
const char* userId = "550e8400-e29b-41d4-a716-446655440002";   // Example UUID

// WiFi secure client
WiFiClientSecure secureClient;

// LED pin (built-in LED on GPIO2)
#define LED_BUILTIN 2

// Timing variables
unsigned long lastSensorUpdate = 0;
unsigned long lastLedBlink = 0;
unsigned long sensorInterval = 12000;  // 12 seconds (10-15 range)
unsigned long ledBlinkInterval = 1000;  // 1 second LED blink

// Status tracking
bool ledState = false;
bool wifiConnected = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 GardenFlow Sensor Node Starting ===");
  
  // Initialize LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED initialized on GPIO2");
  
  // Configure secure client for HTTPS (skip SSL cert validation for simplicity)
  secureClient.setInsecure();
  Serial.println("WiFiClientSecure configured");
  
  // Connect to WiFi once in setup
  connectToWiFi();
  
  Serial.println("=== ESP32 initialization complete ===");
  Serial.println("System ready - will send sensor data every 12 seconds");
  Serial.println("LED will blink every second to show device is alive\n");
}

void loop() {
  // Check WiFi status and reconnect if needed
  if (WiFi.status() != WL_CONNECTED && wifiConnected) {
    Serial.println("WiFi connection lost! Attempting reconnection...");
    wifiConnected = false;
    connectToWiFi();
  }
  
  // Blink LED every second to show device is alive
  if (millis() - lastLedBlink >= ledBlinkInterval) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
    lastLedBlink = millis();
  }
  
  // Send sensor data every 12 seconds
  if (millis() - lastSensorUpdate >= sensorInterval) {
    if (wifiConnected) {
      sendSensorDataToSupabase();
    } else {
      Serial.println("Skipping sensor data send - WiFi not connected");
    }
    lastSensorUpdate = millis();
  }
  
  delay(100);  // Small delay to prevent watchdog issues
}

void connectToWiFi() {
  Serial.printf("Connecting to WiFi network: %s\n", ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    // Fast blink while connecting
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\n✓ WiFi connected successfully!");
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Signal strength: %d dBm\n", WiFi.RSSI());
    
    // Success indication - solid LED for 2 seconds
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    wifiConnected = false;
    Serial.println("\n✗ WiFi connection failed!");
    Serial.println("Please check credentials and try again.");
    
    // Error indication - rapid blinks
    for (int i = 0; i < 10; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }
}

// Generate dummy sensor data (replace with actual sensor readings later)
struct SensorData {
  float temperature;
  float humidity;
  float soilMoisture;
  float waterUsage;
};

SensorData generateSensorData() {
  SensorData data;
  
  // Generate realistic dummy values (replace with actual sensor code)
  data.temperature = 20.0 + (random(0, 150) / 10.0);  // 20.0 - 35.0°C
  data.humidity = 40.0 + (random(0, 400) / 10.0);     // 40.0 - 80.0%
  data.soilMoisture = 30.0 + (random(0, 500) / 10.0); // 30.0 - 80.0%
  data.waterUsage = random(0, 50) / 10.0;             // 0.0 - 5.0L
  
  return data;
}

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
// MODULAR SENSOR FUNCTIONS
// Replace these with actual sensor code
// =====================================

// Example function for temperature sensor (replace with DHT22, DS18B20, etc.)
float readTemperature() {
  // TODO: Replace with actual sensor reading
  // return dht.readTemperature();
  return 20.0 + (random(0, 150) / 10.0);  // 20.0 - 35.0°C
}

// Example function for humidity sensor (replace with DHT22, SHT30, etc.)
float readHumidity() {
  // TODO: Replace with actual sensor reading
  // return dht.readHumidity();
  return 40.0 + (random(0, 400) / 10.0);  // 40.0 - 80.0%
}

// Example function for soil moisture sensor (replace with capacitive sensor)
float readSoilMoisture() {
  // TODO: Replace with actual sensor reading
  // int rawValue = analogRead(SOIL_MOISTURE_PIN);
  // return map(rawValue, 0, 4095, 0, 100);
  return 30.0 + (random(0, 500) / 10.0);  // 30.0 - 80.0%
}

// Example function for water usage (replace with flow sensor)
float readWaterUsage() {
  // TODO: Replace with actual sensor reading
  // return flowSensor.getTotalLiters();
  return random(0, 50) / 10.0;  // 0.0 - 5.0L
}

// ===================================== 
// MAIN SENSOR DATA TRANSMISSION
// =====================================
```
