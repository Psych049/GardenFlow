# Plant Monitoring System - Complete Setup Guide

## System Architecture

```
ESP32 + Sensors → Supabase Backend → React Dashboard
     ↓                    ↓              ↓
  Data Collection    Data Storage    Data Visualization
  Pump Control       Analytics       User Interface
```

## 1. Hardware Requirements

### ESP32 Components
- **ESP32 Development Board** (ESP32-WROOM-32)
- **DHT22 Temperature & Humidity Sensor**
- **Capacitive Soil Moisture Sensor**
- **5V Relay Module** (for pump control)
- **12V Water Pump** (submersible or inline)
- **Breadboard & Jumper Wires**
- **Power Supply** (12V for pump, 5V for ESP32)

### Pin Connections
```
ESP32 Pin    →    Component
GPIO 4       →    DHT22 Data
GPIO 36      →    Soil Moisture Sensor (ADC1_CH0)
GPIO 5       →    Relay Module (Pump Control)
GPIO 2       →    Status LED
3.3V         →    DHT22 VCC
5V           →    Relay Module VCC
GND          →    All GND pins
```

## 2. Supabase Setup

### 2.1 Create Supabase Project
1. Go to [supabase.com](https://supabase.com)
2. Create a new project
3. Note your project URL and anon key

### 2.2 Database Schema Setup
1. Go to SQL Editor in your Supabase dashboard
2. Copy and paste the entire content of `supabase/schema.sql`
3. Execute the script to create all tables and functions

### 2.3 Edge Functions Deployment
Deploy the following Edge Functions:

#### esp32-data
```bash
supabase functions deploy esp32-data
```

#### esp32-commands
```bash
supabase functions deploy esp32-commands
```

#### device-management
```bash
supabase functions deploy device-management
```

#### simulate-sensor-data (optional)
```bash
supabase functions deploy simulate-sensor-data
```

## 3. ESP32 Setup

### 3.1 Install Required Libraries
In Arduino IDE, install these libraries:
- **WiFi** (built-in)
- **HTTPClient** (built-in)
- **ArduinoJson** (by Benoit Blanchon)
- **DHT sensor library** (by Adafruit)
- **WiFiClientSecure** (built-in)

### 3.2 Configure ESP32 Code
1. Open `esp32_plant_monitor.ino`
2. Update the configuration section:

```cpp
// Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* supabaseUrl = "YOUR_SUPABASE_URL";
const char* supabaseAnonKey = "YOUR_SUPABASE_ANON_KEY";
const char* apiKey = "YOUR_API_KEY"; // Generate this in dashboard

// Device Configuration
const String deviceId = "ESP32_PLANT_MONITOR_001";
const String deviceName = "Garden Monitor 1";
const String sensorZoneId = "ZONE_001"; // Match with dashboard zone
```

### 3.3 Calibrate Soil Moisture Sensor
1. Place sensor in dry soil, note the reading
2. Place sensor in wet soil, note the reading
3. Update these values in the code:

```cpp
#define SOIL_MOISTURE_DRY 4095    // Your dry reading
#define SOIL_MOISTURE_WET 1500    // Your wet reading
```

### 3.4 Upload Code
1. Connect ESP32 to computer
2. Select correct board and port in Arduino IDE
3. Upload the code
4. Open Serial Monitor (115200 baud) to see debug output

## 4. Dashboard Setup

### 4.1 Environment Variables
Create `.env` file in your project root:

```env
VITE_SUPABASE_URL=your_supabase_url
VITE_SUPABASE_ANON_KEY=your_supabase_anon_key
```

### 4.2 Install Dependencies
```bash
npm install
```

### 4.3 Start Development Server
```bash
npm run dev
```

## 5. API Key Generation

### 5.1 Create API Key
1. Log into your dashboard
2. Go to Settings → API Keys
3. Create a new API key
4. Copy the generated key to your ESP32 code

### 5.2 API Key Format
The API key should be a secure random string, for example:
```
sk_plant_monitor_abc123def456ghi789
```

## 6. Zone Configuration

### 6.1 Create Zones
1. In your dashboard, go to Plants → Zones
2. Create zones for each plant area
3. Note the zone IDs and match them with ESP32 sensor IDs

### 6.2 Zone Setup Example
```
Zone Name: Vegetable Garden
Soil Type: Loam soil
Sensor ID: ZONE_001
Moisture Level: 35%
```

## 7. Testing the System

### 7.1 ESP32 Testing
1. Power on ESP32
2. Check Serial Monitor for:
   - WiFi connection status
   - Sensor readings
   - Data transmission confirmations

### 7.2 Dashboard Testing
1. Open dashboard in browser
2. Check if sensor data appears
3. Test manual pump control
4. Verify alerts are generated

### 7.3 Pump Testing
1. Test manual pump activation from dashboard
2. Verify auto-watering triggers when moisture is low
3. Check pump duration settings

## 8. Troubleshooting

### 8.1 ESP32 Issues
- **WiFi Connection**: Check SSID and password
- **Sensor Readings**: Verify wiring and power
- **Data Transmission**: Check Supabase URL and API key
- **Pump Control**: Verify relay wiring and power supply

### 8.2 Dashboard Issues
- **Authentication**: Check Supabase credentials
- **Data Display**: Verify database connections
- **Real-time Updates**: Check WebSocket connections

### 8.3 Database Issues
- **RLS Policies**: Ensure proper user permissions
- **Triggers**: Check if alerts are being generated
- **API Functions**: Verify Edge Function deployments

## 9. Advanced Configuration

### 9.1 Multiple ESP32 Devices
1. Create unique device IDs for each ESP32
2. Configure different zones for each device
3. Update API keys for each device

### 9.2 Custom Watering Schedules
1. Use cron expressions for scheduling
2. Configure different durations for different zones
3. Set up seasonal adjustments

### 9.3 Alert Configuration
1. Customize moisture thresholds
2. Set up email/SMS notifications
3. Configure alert severity levels

## 10. Security Considerations

### 10.1 API Key Security
- Keep API keys secure and private
- Rotate keys regularly
- Use different keys for different devices

### 10.2 Network Security
- Use WPA3 WiFi if available
- Consider VPN for remote access
- Monitor network traffic

### 10.3 Physical Security
- Secure ESP32 and sensors from weather
- Protect pump and wiring
- Consider backup power supply

## 11. Maintenance

### 11.1 Regular Tasks
- Clean soil moisture sensors monthly
- Check pump for debris
- Update firmware when available
- Monitor battery levels (if applicable)

### 11.2 Seasonal Adjustments
- Adjust watering schedules for seasons
- Update moisture thresholds
- Check sensor calibration

## 12. Support

For issues and questions:
1. Check the troubleshooting section
2. Review ESP32 Serial Monitor output
3. Check Supabase logs
4. Verify all connections and configurations

## System Features

✅ **Real-time sensor monitoring**
✅ **Automatic watering based on moisture levels**
✅ **Manual pump control from dashboard**
✅ **Scheduled watering**
✅ **Alert system for critical conditions**
✅ **Multi-zone support**
✅ **Device management**
✅ **Data analytics and visualization**
✅ **Mobile-responsive dashboard**
✅ **Secure API authentication**

Your plant monitoring system is now ready to keep your plants healthy and thriving! 🌱 