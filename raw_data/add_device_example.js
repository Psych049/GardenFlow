// Example: How to Add Devices Programmatically
// This shows different ways to add devices to your dashboard

import { DeviceService } from '../src/services/deviceService.js';

// Method 1: Add a basic ESP32 device
async function addBasicDevice() {
  try {
    const device = await DeviceService.createDevice({
      name: "Garden Monitor 1",
      device_id: "ESP32_PLANT_MONITOR_001",
      firmware_version: "v1.0.0"
    });
    
    console.log('Device added:', device);
  } catch (error) {
    console.error('Error adding device:', error);
  }
}

// Method 2: Add a device with more details
async function addDetailedDevice() {
  try {
    const device = await DeviceService.createDevice({
      name: "Backyard Garden Monitor",
      device_id: "ESP32_BACKYARD_001",
      device_type: "esp32",
      status: "offline", // Will be updated when device connects
      ip_address: "192.168.1.100",
      mac_address: "AA:BB:CC:DD:EE:FF",
      firmware_version: "v1.2.0"
    });
    
    console.log('Detailed device added:', device);
  } catch (error) {
    console.error('Error adding device:', error);
  }
}

// Method 3: Add multiple devices at once
async function addMultipleDevices() {
  const devices = [
    {
      name: "Front Garden Monitor",
      device_id: "ESP32_FRONT_001",
      firmware_version: "v1.0.0"
    },
    {
      name: "Back Garden Monitor", 
      device_id: "ESP32_BACK_001",
      firmware_version: "v1.0.0"
    },
    {
      name: "Greenhouse Monitor",
      device_id: "ESP32_GREENHOUSE_001", 
      firmware_version: "v1.1.0"
    }
  ];

  for (const deviceData of devices) {
    try {
      const device = await DeviceService.createDevice(deviceData);
      console.log(`Added device: ${device.name}`);
    } catch (error) {
      console.error(`Error adding ${deviceData.name}:`, error);
    }
  }
}

// Method 4: Add device and immediately send a command
async function addDeviceAndCommand() {
  try {
    // First add the device
    const device = await DeviceService.createDevice({
      name: "Test Garden Monitor",
      device_id: "ESP32_TEST_001",
      firmware_version: "v1.0.0"
    });
    
    // Then send a restart command
    const command = await DeviceService.sendCommand(device.id, 'restart', {
      reason: 'Initial setup',
      force: true
    });
    
    console.log('Device added and restart command sent:', { device, command });
  } catch (error) {
    console.error('Error:', error);
  }
}

// Export functions for use
export {
  addBasicDevice,
  addDetailedDevice,
  addMultipleDevices,
  addDeviceAndCommand
}; 