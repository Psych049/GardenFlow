-- Add Devices Directly to Database
-- Run these in your Supabase SQL Editor

-- First, get your user ID from the frontend console:
-- supabase.auth.getUser().then(result => console.log('User ID:', result.data.user.id))

-- Replace 'YOUR_USER_ID_HERE' with your actual user ID below

-- Example 1: Add a basic ESP32 device
INSERT INTO devices (
  name,
  device_id,
  device_type,
  status,
  firmware_version,
  user_id
) VALUES (
  'Garden Monitor 1',
  'ESP32_PLANT_MONITOR_001',
  'esp32',
  'offline',
  'v1.0.0',
  'YOUR_USER_ID_HERE'
);

-- Example 2: Add a device with IP address
INSERT INTO devices (
  name,
  device_id,
  device_type,
  status,
  ip_address,
  firmware_version,
  user_id
) VALUES (
  'Backyard Monitor',
  'ESP32_BACKYARD_001',
  'esp32',
  'offline',
  '192.168.1.100',
  'v1.2.0',
  'YOUR_USER_ID_HERE'
);

-- Example 3: Add multiple devices
INSERT INTO devices (
  name,
  device_id,
  device_type,
  status,
  firmware_version,
  user_id
) VALUES 
  ('Front Garden', 'ESP32_FRONT_001', 'esp32', 'offline', 'v1.0.0', 'YOUR_USER_ID_HERE'),
  ('Back Garden', 'ESP32_BACK_001', 'esp32', 'offline', 'v1.0.0', 'YOUR_USER_ID_HERE'),
  ('Greenhouse', 'ESP32_GREENHOUSE_001', 'esp32', 'offline', 'v1.1.0', 'YOUR_USER_ID_HERE');

-- Example 4: Add device and create API key
WITH new_device AS (
  INSERT INTO devices (
    name,
    device_id,
    device_type,
    status,
    firmware_version,
    user_id
  ) VALUES (
    'Test Device',
    'ESP32_TEST_001',
    'esp32',
    'offline',
    'v1.0.0',
    'YOUR_USER_ID_HERE'
  ) RETURNING id, user_id
)
INSERT INTO api_keys (
  key_value,
  device_id,
  name,
  user_id
) 
SELECT 
  encode(gen_random_bytes(32), 'base64'),
  new_device.id,
  'Test Device API Key',
  new_device.user_id
FROM new_device;

-- Verify devices were added
SELECT 
  id,
  name,
  device_id,
  status,
  firmware_version,
  created_at
FROM devices 
WHERE user_id = 'YOUR_USER_ID_HERE'
ORDER BY created_at DESC; 