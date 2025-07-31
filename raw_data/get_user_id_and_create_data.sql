-- Get User ID and Create Data Manually
-- Run this after manual_database_setup.sql

-- Step 1: Get your user ID
-- Go to your frontend app, open browser dev tools (F12), go to Console, and run:
-- supabase.auth.getUser().then(result => console.log('User ID:', result.data.user.id))
-- Copy the user ID from the console output

-- Step 2: Replace 'YOUR_USER_ID_HERE' with your actual user ID below
-- Example: '12345678-1234-1234-1234-123456789abc'

-- Step 3: Create test data (replace YOUR_USER_ID_HERE with your actual user ID)
INSERT INTO zones (name, description, soil_type, moisture_threshold, pump_on, user_id)
VALUES ('Test Zone', 'Test zone for debugging', 'Loamy', 40, false, 'YOUR_USER_ID_HERE')
ON CONFLICT DO NOTHING;

INSERT INTO devices (name, device_id, device_type, status, user_id)
VALUES ('Test ESP32', 'ESP32_TEST_001', 'esp32', 'online', 'YOUR_USER_ID_HERE')
ON CONFLICT DO NOTHING;

INSERT INTO watering_schedules (zone_id, name, cron_expression, duration, is_active, user_id)
VALUES (
  (SELECT id FROM zones WHERE user_id = 'YOUR_USER_ID_HERE' LIMIT 1),
  'Test Schedule',
  '0 7 * * *',
  30,
  true,
  'YOUR_USER_ID_HERE'
) ON CONFLICT DO NOTHING;

INSERT INTO sensor_data (zone_id, temperature, humidity, soil_moisture, user_id)
VALUES (
  (SELECT id FROM zones WHERE user_id = 'YOUR_USER_ID_HERE' LIMIT 1),
  25.5,
  60.0,
  45.0,
  'YOUR_USER_ID_HERE'
) ON CONFLICT DO NOTHING;

-- Step 4: Verify the data was created
SELECT 'Zones' as table_name, COUNT(*) as count FROM zones WHERE user_id = 'YOUR_USER_ID_HERE'
UNION ALL
SELECT 'Devices' as table_name, COUNT(*) as count FROM devices WHERE user_id = 'YOUR_USER_ID_HERE'
UNION ALL
SELECT 'Watering Schedules' as table_name, COUNT(*) as count FROM watering_schedules WHERE user_id = 'YOUR_USER_ID_HERE'
UNION ALL
SELECT 'Sensor Data' as table_name, COUNT(*) as count FROM sensor_data WHERE user_id = 'YOUR_USER_ID_HERE';

-- Step 5: Test join query
SELECT 
  ws.name as schedule_name,
  z.name as zone_name,
  z.soil_type
FROM watering_schedules ws
LEFT JOIN zones z ON ws.zone_id = z.id
WHERE ws.user_id = 'YOUR_USER_ID_HERE'
ORDER BY ws.created_at DESC; 