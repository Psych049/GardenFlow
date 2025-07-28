import { useState } from "react";
import { Helmet } from "react-helmet";
import { FiCpu, FiWifi, FiAlertCircle, FiRefreshCw, FiActivity } from "react-icons/fi";

export default function SystemPage() {
  const [refreshing, setRefreshing] = useState(false);
  const [lastRefreshed, setLastRefreshed] = useState(null);

  // Simulate refreshing sensor data
  const handleRefreshData = async () => {
    setRefreshing(true);
    await new Promise(resolve => setTimeout(resolve, 1500));
    setRefreshing(false);
    setLastRefreshed(new Date());
  };

  return (
    <>
      <Helmet>
        <title>System | Smart Garden Watering System</title>
      </Helmet>
      <div className="space-y-6">
        <h1 className="text-3xl font-bold">System</h1>
        <div className="grid gap-6 md:grid-cols-2">
          {/* ESP32 Status Card */}
          <div className="bg-white dark:bg-gray-800 rounded-lg shadow p-6 flex flex-col">
            <div className="mb-4">
              <div className="flex items-center text-lg font-semibold mb-1">
                <FiCpu className="mr-2 h-5 w-5" /> ESP32 Status
              </div>
              <div className="text-sm text-gray-500 dark:text-gray-400">Hardware device status and information</div>
            </div>
            <div className="flex-1 space-y-4">
              <div className="flex items-center justify-between">
                <span className="text-sm">Connection Status:</span>
                <span className="px-2 py-1 rounded bg-green-50 text-green-700 dark:bg-green-900/30 dark:text-green-400 text-xs font-medium">Connected</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm">Signal Strength:</span>
                <div className="flex items-center">
                  <FiWifi className="mr-1 h-4 w-4 text-green-600" />
                  <span>Strong</span>
                </div>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm">Firmware Version:</span>
                <span className="text-sm">v1.2.0</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm">Uptime:</span>
                <span className="text-sm">3d 7h 12m</span>
              </div>
            </div>
            <button className="mt-6 w-full py-2 rounded border border-gray-300 dark:border-gray-600 bg-transparent hover:bg-gray-100 dark:hover:bg-gray-700 transition text-sm font-medium" type="button">
              Restart Device
            </button>
          </div>
          {/* Sensor Status Card */}
          <div className="bg-white dark:bg-gray-800 rounded-lg shadow p-6 flex flex-col">
            <div className="mb-4">
              <div className="flex items-center text-lg font-semibold mb-1">
                <FiActivity className="mr-2 h-5 w-5" /> Sensor Status
              </div>
              <div className="text-sm text-gray-500 dark:text-gray-400">Connected sensors information</div>
            </div>
            <div className="flex-1 space-y-4">
              <div className="flex items-center justify-between">
                <span className="text-sm">Temperature Sensor:</span>
                <span className="px-2 py-1 rounded bg-green-50 text-green-700 dark:bg-green-900/30 dark:text-green-400 text-xs font-medium">Active</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm">Humidity Sensor:</span>
                <span className="px-2 py-1 rounded bg-green-50 text-green-700 dark:bg-green-900/30 dark:text-green-400 text-xs font-medium">Active</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm">Soil Moisture Sensor:</span>
                <span className="px-2 py-1 rounded bg-green-50 text-green-700 dark:bg-green-900/30 dark:text-green-400 text-xs font-medium">Active</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm">Water Flow Meter:</span>
                <span className="px-2 py-1 rounded bg-green-50 text-green-700 dark:bg-green-900/30 dark:text-green-400 text-xs font-medium">Active</span>
              </div>
            </div>
            <button className="mt-6 w-full py-2 rounded border border-gray-300 dark:border-gray-600 bg-transparent hover:bg-gray-100 dark:hover:bg-gray-700 transition text-sm font-medium flex items-center justify-center" type="button" disabled={refreshing} onClick={handleRefreshData}>
              {refreshing ? (
                <>
                  <FiRefreshCw className="mr-2 h-4 w-4 animate-spin" /> Refreshing...
                </>
              ) : (
                <>
                  <FiRefreshCw className="mr-2 h-4 w-4" /> Refresh Sensor Data
                </>
              )}
            </button>
          </div>
        </div>
        {/* Last refreshed info */}
        {lastRefreshed && (
          <p className="text-sm text-gray-500 dark:text-gray-400">Last refreshed: {lastRefreshed.toLocaleString()}</p>
        )}
        {/* System Info */}
        <div className="flex items-start gap-3 bg-yellow-50 dark:bg-yellow-900/20 border-l-4 border-yellow-400 dark:border-yellow-600 rounded-md p-4 mt-4">
          <FiAlertCircle className="h-5 w-5 text-yellow-600 dark:text-yellow-400 mt-1" />
          <div>
            <div className="font-semibold">System Information</div>
            <div className="text-sm">
              <p>This is a simulation of an ESP32-based Smart Garden Watering System.</p>
              <p className="mt-2">In a real deployment, this page would display actual ESP32 device information, sensor readings, and provide controls for firmware updates and diagnostics.</p>
              <p className="mt-2"><strong>Hardware Components:</strong> ESP32, DHT11 temperature sensor, soil moisture sensor, DC 5V water pump</p>
            </div>
          </div>
        </div>
        {/* ESP32 Code Information */}
        <div className="rounded-md bg-gray-100 dark:bg-slate-900 p-4 mt-6">
          <h3 className="mb-2 text-sm font-medium">ESP32 Code Sample</h3>
          <pre className="overflow-auto rounded-md bg-slate-950 p-4 text-xs text-slate-50">
{`// ESP32 Arduino Sample Code
#include <WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

// Pin definitions
#define DHTPIN 4
#define MOISTURE_PIN 32
#define PUMP_PIN 13
#define DHTTYPE DHT11

// Initialize components
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  dht.begin();
  setupWiFi();
  setupMQTT();
}

void loop() {
  // Read sensors
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int moisture = readMoisture();
  
  // Send data to server
  sendSensorData(temperature, humidity, moisture);
  
  // Check for commands
  checkPumpCommands();
  
  delay(10000);  // 10 second interval
}`}
          </pre>
          <p className="mt-2 text-xs text-gray-500 dark:text-gray-400">This is a simplified code example. In a production environment, additional error handling, power management, and failsafe mechanisms would be implemented.</p>
        </div>
      </div>
    </>
  );
} 