# Quick Start Guide - Get Your ESP32 Connected

## 🚀 **Step 1: Create Your First Zone**

Since you don't have any zones yet, you need to create one first:

### **Option A: Use the Quick Create Button**
1. Go to **System** page
2. Click **"Add Device"**
3. You'll see a **"Create Default Zone"** button (green button)
4. Click it to create a default zone automatically
5. The dropdown will then appear with your new zone

### **Option B: Create Zone Manually**
1. Go to **Plants** page
2. Click **"Add Zone"**
3. Fill in:
   - **Name**: "Garden Zone 1"
   - **Description**: "Main garden area"
   - **Soil Type**: "Loamy"
   - **Moisture Threshold**: 40
4. Click **"Add Zone"**
5. Go back to **System** page

## 📱 **Step 2: Add Your Device**

1. In **System** page, click **"Add Device"**
2. Fill in the form:
   - **Device Name**: "My ESP32"
   - **Device ID**: "ESP32_REAL_001"
   - **Firmware Version**: "v1.0.0"
   - **Zone**: Select your zone from dropdown
3. Click **"Add Device"**

## 🔑 **Step 3: Get Your API Key**

After adding the device:
1. **Scroll down** on the System page
2. Look for **"Device Configuration"** section
3. Find your **API Key** with a **"Copy"** button
4. Click **"Copy"** to copy the API key

## 🔧 **Step 4: Configure ESP32**

1. Open `esp32_real_device.ino`
2. Update these lines:
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   const String deviceId = "ESP32_REAL_001";  // From dashboard
   const char* apiKey = "your-copied-api-key"; // From dashboard
   ```
3. Upload to ESP32

## ✅ **You're Done!**

Your ESP32 will now:
- Connect to WiFi
- Send sensor data to dashboard
- Appear as "online" in System page
- Show real data in Dashboard page

## 🆘 **If You Still Don't See the Dropdown:**

1. **Check browser console** (F12) for errors
2. **Refresh the page**
3. **Try creating zone manually** from Plants page
4. **Check if you're logged in** properly

The zone dropdown will appear once you have at least one zone created! 🌱💧 