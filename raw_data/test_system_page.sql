-- Test SystemPage Functionality
-- Run this after setting up the database to test device management

-- 1. Check if we have any devices
SELECT COUNT(*) as device_count FROM devices;

-- 2. Show device structure
SELECT 
  id,
  name,
  device_id,
  device_type,
  status,
  last_seen,
  firmware_version,
  ip_address
FROM devices
ORDER BY created_at DESC
LIMIT 5;

-- 3. Test device creation (this would be done through the frontend)
-- The frontend will handle user authentication and device creation

-- 4. Check commands table structure
SELECT 
  id,
  device_id,
  command_type,
  parameters,
  status,
  created_at
FROM commands
ORDER BY created_at DESC
LIMIT 5;

-- 5. Test API key validation function
SELECT validate_api_key('test_key') as is_valid;

-- 6. Test device lookup function
SELECT get_device_from_api_key('test_key') as device_id;

-- 7. Show system status
SELECT 
  'System Status' as status,
  'Database is ready for SystemPage integration' as details;

-- 8. Test RLS policies for devices
-- This should work if user is authenticated
SELECT COUNT(*) as user_devices FROM devices;

-- 9. Show final verification
SELECT 
  'SystemPage test completed!' as status,
  'You can now test device management in the frontend' as details; 