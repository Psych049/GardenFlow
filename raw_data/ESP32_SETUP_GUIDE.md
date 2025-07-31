# 🌱 ESP32 Plant Monitoring System - Complete Setup Guide

## 📋 **Overview**

This guide will help you set up your ESP32 plant monitoring system with:
- **WiFi connectivity** to your network
- **Sensor data collection** (temperature, humidity, soil moisture)
- **Secure API authentication** with Supabase
- **Real-time data transmission** to your dashboard
- **Remote pump control** from the dashboard
- **Automatic watering** based on soil moisture

## 🔧 **Hardware Requirements**

### **Required Components:**
- **ESP32 Development Board** (ESP32-WROOM-32 or similar)
- **DHT22 Temperature & Humidity Sensor**
- **Capacitive Soil Moisture Sensor**
- **5V Relay Module**
- **12V Water Pump** (or 5V pump depending on your setup)
- **Breadboard and jumper wires**
- **Power supply** (12V for pump, 5V for ESP32)

### **Pin Connections:**
```
ESP32 Pin    → Component
GPIO 4       → DHT22 Data Pin
GPIO 32      → Soil Moisture Sensor (Analog)
GPIO 13      → Relay Module (Pump Control)
GPIO 2       → Status LED (Built-in)
3.3V         → DHT22 VCC
5V           → Relay Module VCC
GND          → All GND pins
```

## 🚀 **Step-by-Step Setup**

### **1. Database Setup**

First, you need to set up your Supabase database:

#### **A. Create a Zone**
1. Go to your dashboard's **Plants** page
2. Create a new zone or note an existing zone ID
3. Copy the zone UUID (you'll need this for ESP32 config)

#### **B. Create a Device**
1. Go to your dashboard's **System** page
2. The system will automatically detect your ESP32 when it connects
3. Note the device ID that gets created

#### **C. Generate API Key**
1. Open your browser console (F12)
2. Copy and paste the `manage_api_keys.js` script
3. Run: `apiKeyManager.listDevices()` to see your devices
4. Run: `apiKeyManager.createApiKey('your-device-id', 'ESP32 Device')`
5. Copy the generated API key

### **2. ESP32 Configuration**

#### **A. Install Required Libraries**
In Arduino IDE, install these libraries:
- **WiFi** (built-in)
- **HTTPClient** (built-in)
- **ArduinoJson** (by Benoit Blanchon)
- **DHT sensor library** (by Adafruit)
- **WiFiClientSecure** (built-in)

#### **B. Update Configuration**
In `esp32_plant_monitor.ino`, update these values:

```cpp
// WiFi Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Supabase Configuration
const char* supabaseUrl = "https://gqzaxkczxcudxbbkudmm.supabase.co";
const char* supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImdxemF4a2N6eGN1ZHhiYmt1ZG1tIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTM3MzcwNzEsImV4cCI6MjA2OTMxMzA3MX0.RR4jib8iRkZG1rqFpH3wuTE82BY5ViJKFR0FVvu5N4U";

// Device Authentication
const char* apiKey = "YOUR_GENERATED_API_KEY";

// Device Configuration
const String deviceId = "ESP32_PLANT_MONITOR_001";
const String deviceName = "Garden Monitor 1";
const String sensorZoneId = "YOUR_ZONE_UUID";
```

#### **C. Calibrate Soil Moisture Sensor**
1. Upload the code to ESP32
2. Open Serial Monitor (115200 baud)
3. The code includes a calibration function
4. Follow the prompts to calibrate your sensor

### **3. Deploy Edge Functions**

#### **A. Deploy Device API Function**
1. Go to Supabase Dashboard → Edge Functions
2. Create new function: `device-api`
3. Copy the code from `supabase/functions/device-api/index.ts`
4. Deploy the function

#### **B. Update Database Schema**
1. Go to Supabase Dashboard → SQL Editor
2. Run the updated schema from `supabase/schema.sql`
3. This creates the API key validation functions

### **4. Testing the System**

#### **A. Initial Test**
1. Power on your ESP32
2. Check Serial Monitor for:
   - WiFi connection status
   - Device registration
   - Sensor readings
   - Data transmission success

#### **B. Dashboard Verification**
1. Go to your dashboard
2. Check **Sensors** page for real-time data
3. Check **System** page for device status
4. Check **Plants** page for zone data

#### **C. Pump Control Test**
1. Go to **Plants** page
2. Click the pump toggle button for your zone
3. Verify the ESP32 receives and executes the command
4. Check Serial Monitor for command execution logs

## 🔍 **Troubleshooting**

### **Common Issues:**

#### **1. WiFi Connection Failed**
- Check SSID and password
- Ensure WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Check signal strength

#### **2. API Key Authentication Failed**
- Verify API key is correctly copied
- Check if API key is active in database
- Ensure device is properly registered

#### **3. Sensor Readings are Wrong**
- Check wiring connections
- Calibrate soil moisture sensor
- Verify DHT22 is properly connected

#### **4. Data Not Appearing in Dashboard**
- Check Supabase Edge Function logs
- Verify zone ID matches
- Check network connectivity

#### **5. Pump Not Responding**
- Check relay wiring
- Verify pump power supply
- Check GPIO pin configuration

### **Debug Commands:**

#### **ESP32 Serial Monitor Commands:**
```
// Check WiFi status
WiFi.status()

// Check sensor readings
dht.readTemperature()
dht.readHumidity()
analogRead(SOIL_MOISTURE_PIN)

// Test pump control
digitalWrite(PUMP_PIN, HIGH)
digitalWrite(PUMP_PIN, LOW)
```

#### **API Testing:**
```javascript
// Test API key
apiKeyManager.testApiKey('your-api-key')

// List devices
apiKeyManager.listDevices()

// List API keys
apiKeyManager.listApiKeys()
```

## 📊 **Monitoring & Maintenance**

### **Regular Checks:**
1. **Daily**: Check dashboard for sensor data
2. **Weekly**: Verify pump functionality
3. **Monthly**: Calibrate soil moisture sensor
4. **Quarterly**: Check for firmware updates

### **Performance Optimization:**
- Adjust `DATA_SEND_INTERVAL` based on your needs
- Monitor battery life if using battery power
- Check WiFi signal strength
- Review sensor calibration periodically

### **Security Best Practices:**
- Rotate API keys regularly
- Use strong WiFi passwords
- Keep firmware updated
- Monitor for unauthorized access

## 🎯 **Advanced Features**

### **Automatic Watering:**
The system can automatically water plants when soil moisture is low:
- Configure moisture threshold in dashboard
- Enable auto-watering mode
- Set watering duration

### **Scheduled Watering:**
Set up watering schedules:
- Go to **Watering Schedule** page
- Create time-based schedules
- Configure frequency and duration

### **Alerts & Notifications:**
The system generates alerts for:
- Low soil moisture
- High temperature
- Pump failures
- Device disconnections

## 📞 **Support**

If you encounter issues:
1. Check this troubleshooting guide
2. Review Serial Monitor logs
3. Check Supabase Edge Function logs
4. Verify all connections and configurations

## 🎉 **Congratulations!**

Your ESP32 Plant Monitoring System is now fully operational! You can:
- Monitor plant conditions in real-time
- Control watering remotely
- Set up automatic watering schedules
- Receive alerts for plant health issues

Happy gardening! 🌱✨ 