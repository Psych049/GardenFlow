# Real ESP32 Device Setup Guide

## 🚀 **Complete Setup for Your Real ESP32 Device**

This guide will help you connect your actual ESP32 device to your dashboard.

---

## 📋 **Step 1: Add Device to Dashboard**

### 1.1 Get Your User ID
1. Open your dashboard in browser
2. Open Developer Tools (F12) → Console
3. Run this command:
   ```javascript
   supabase.auth.getUser().then(result => console.log('User ID:', result.data.user.id))
   ```
4. Copy the user ID that appears

### 1.2 Add Device Through Dashboard
1. Go to **System** page in your dashboard
2. Click **"Add Device"** button
3. Fill in the form:
   - **Device Name**: "My Real ESP32"
   - **Device ID**: "ESP32_REAL_001" (use this exact ID)
   - **Firmware Version**: "v1.0.0"
4. Click **"Add Device"**

### 1.3 Get Your API Key
1. After adding the device, go to **Supabase Dashboard**
2. Navigate to **Table Editor** → **api_keys**
3. Find your device's API key
4. Copy the `key_value` (it's a long string)

---

## 🔧 **Step 2: Configure ESP32 Code**

### 2.1 Update Configuration
Open `esp32_real_device.ino` and update these settings:

```cpp
// WiFi Configuration - UPDATE THESE
const char* ssid = "YOUR_WIFI_NAME";           // Your WiFi name
const char* password = "YOUR_WIFI_PASSWORD";   // Your WiFi password

// Device Configuration - MUST MATCH DASHBOARD
const String deviceId = "ESP32_REAL_001";      // Must match dashboard device ID
const String deviceName = "My Real ESP32";     // Must match dashboard device name
const String sensorZoneId = "ZONE_001";        // Must match your zone ID

// API Key - PASTE YOUR API KEY HERE
const char* apiKey = "YOUR_API_KEY_HERE";      // Paste the API key from Step 1.3
```

### 2.2 Hardware Connections
Connect your sensors to these pins:

| Component | ESP32 Pin | Connection |
|-----------|-----------|------------|
| DHT22 | GPIO 4 | VCC → 3.3V, GND → GND, DATA → GPIO 4 |
| Soil Moisture | GPIO 36 | VCC → 3.3V, GND → GND, SIG → GPIO 36 |
| Relay/Pump | GPIO 5 | VCC → 5V, GND → GND, IN → GPIO 5 |
| Status LED | GPIO 2 | Anode → GPIO 2, Cathode → GND (with 220Ω resistor) |

### 2.3 Calibrate Soil Moisture Sensor
1. Upload the code to ESP32
2. Open Serial Monitor (115200 baud)
3. Check the "Raw Soil Value" readings:
   - **Dry soil**: Should be around 4095
   - **Wet soil**: Should be around 1500
4. Update these values in the code if needed:
   ```cpp
   #define SOIL_MOISTURE_DRY 4095    // Your dry reading
   #define SOIL_MOISTURE_WET 1500    // Your wet reading
   ```

---

## 📦 **Step 3: Install Required Libraries**

In Arduino IDE, install these libraries:
1. **WiFi** (built-in)
2. **HTTPClient** (built-in)
3. **ArduinoJson** (by Benoit Blanchon)
4. **DHT sensor library** (by Adafruit)
5. **WiFiClientSecure** (built-in)

---

## 🔌 **Step 4: Upload and Test**

### 4.1 Upload Code
1. Connect ESP32 to computer
2. Select correct board and port in Arduino IDE
3. Upload `esp32_real_device.ino`

### 4.2 Monitor Serial Output
Open Serial Monitor (115200 baud) and look for:

```
========================================
ESP32 Plant Monitor - REAL DEVICE
========================================
Device ID: ESP32_REAL_001
Device Name: My Real ESP32
Zone ID: ZONE_001
========================================
Initializing DHT sensor...
Connecting to WiFi: YOUR_WIFI_NAME
WiFi connected successfully!
IP Address: 192.168.1.xxx
Signal Strength: -45
Device status updated successfully
ESP32 Plant Monitor initialized successfully!
```

### 4.3 Expected Behavior
- **LED blinks 3 times** on startup
- **LED blinks 2 times** when WiFi connects
- **Sensor readings** every 30 seconds
- **Status updates** every 5 minutes

---

## ✅ **Step 5: Verify Dashboard Integration**

### 5.1 Check Device Status
1. Go to **System** page in dashboard
2. Your device should appear in the device list
3. Status should show as **"online"**
4. IP address should be displayed

### 5.2 Check Sensor Data
1. Go to **Dashboard** page
2. You should see real sensor readings
3. Temperature, humidity, and soil moisture should update
4. Charts should show live data

### 5.3 Test Commands
1. In **System** page, click **"Restart Device"**
2. ESP32 should restart (LED will blink)
3. Check Serial Monitor for restart confirmation

---

## 🔧 **Step 6: Troubleshooting**

### Common Issues:

#### **WiFi Connection Failed**
- Check WiFi name and password
- Ensure ESP32 is within WiFi range
- Try restarting ESP32

#### **DHT Sensor Not Reading**
- Check wiring (VCC, GND, DATA)
- Ensure DHT22 is properly connected to GPIO 4
- Try powering DHT22 from 3.3V instead of 5V

#### **Soil Moisture Sensor Issues**
- Check wiring connections
- Calibrate sensor values
- Ensure sensor is properly inserted in soil

#### **Dashboard Not Showing Data**
- Verify device ID matches exactly
- Check API key is correct
- Ensure Edge Functions are deployed

#### **Commands Not Working**
- Check device is online in dashboard
- Verify API key permissions
- Check Serial Monitor for command responses

---

## 📊 **Step 7: Advanced Configuration**

### 7.1 Adjust Reading Intervals
```cpp
#define SENSOR_READ_INTERVAL 30000 // 30 seconds
#define COMMAND_CHECK_INTERVAL 10000 // 10 seconds
```

### 7.2 Customize Pump Duration
```cpp
int pumpDuration = 30; // seconds
```

### 7.3 Add More Sensors
You can add additional sensors by:
1. Defining new pins
2. Adding sensor reading functions
3. Including data in the JSON payload

---

## 🎯 **Success Indicators**

Your setup is working when:

✅ **ESP32 connects to WiFi**  
✅ **Device appears online in dashboard**  
✅ **Sensor data appears in dashboard**  
✅ **Commands work from dashboard**  
✅ **Pump responds to commands**  
✅ **No errors in Serial Monitor**  

---

## 🆘 **Need Help?**

If you encounter issues:

1. **Check Serial Monitor** for error messages
2. **Verify all connections** are correct
3. **Test WiFi connection** separately
4. **Check dashboard device status**
5. **Verify API key** is correct

Your real ESP32 device should now be fully integrated with your dashboard! 🌱💧 