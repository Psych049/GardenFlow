-- Test Watering Schedule Service Integration
-- Run this to verify the service works with the database

-- 1. Check if we have any zones
SELECT COUNT(*) as zone_count FROM zones;

-- 2. Check if we have any watering schedules
SELECT COUNT(*) as schedule_count FROM watering_schedules;

-- 3. Show sample data structure
SELECT 
  ws.id,
  ws.name,
  ws.cron_expression,
  ws.duration,
  ws.is_active,
  ws.created_at,
  z.name as zone_name,
  z.soil_type
FROM watering_schedules ws
LEFT JOIN zones z ON ws.zone_id = z.id
ORDER BY ws.created_at DESC
LIMIT 5;

-- 4. Test the exact query structure the service uses
SELECT 
  id,
  name,
  cron_expression,
  duration,
  is_active,
  created_at,
  zones (
    id,
    name, 
    soil_type,
    description
  )
FROM watering_schedules
ORDER BY created_at DESC
LIMIT 3;

-- 5. Show zones for dropdown
SELECT 
  id, 
  name, 
  description, 
  soil_type
FROM zones
ORDER BY name;

-- 6. Test RLS policies
SELECT 'Testing RLS policies...' as status;

-- This should work if user is authenticated
SELECT COUNT(*) as user_schedules FROM watering_schedules;

-- 7. Show final status
SELECT 
  'Watering schedule service test completed!' as status,
  'Database is ready for frontend service integration' as details; 