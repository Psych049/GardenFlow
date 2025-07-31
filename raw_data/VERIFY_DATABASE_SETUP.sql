-- VERIFY DATABASE SETUP
-- Run this after running the complete database setup to verify everything is working

-- Step 1: Check all tables exist
SELECT 
  'TABLE VERIFICATION' as check_type,
  table_name,
  'EXISTS' as status
FROM information_schema.tables 
WHERE table_schema = 'public' 
  AND table_name IN ('zones', 'devices', 'sensor_data', 'watering_controls', 'watering_schedules', 'commands', 'api_keys', 'alerts', 'soil_types', 'audit_logs')
ORDER BY table_name;

-- Step 2: Check table structures
SELECT 
  'TABLE STRUCTURE' as check_type,
  table_name,
  COUNT(*) as column_count,
  STRING_AGG(column_name, ', ' ORDER BY ordinal_position) as columns
FROM information_schema.columns 
WHERE table_schema = 'public' 
  AND table_name IN ('zones', 'devices', 'sensor_data', 'watering_controls', 'watering_schedules', 'commands', 'api_keys', 'alerts', 'soil_types', 'audit_logs')
GROUP BY table_name
ORDER BY table_name;

-- Step 3: Check foreign key constraints
SELECT 
  'FOREIGN KEYS' as check_type,
  tc.table_name,
  kcu.column_name,
  ccu.table_name AS foreign_table_name,
  ccu.column_name AS foreign_column_name
FROM information_schema.table_constraints AS tc 
JOIN information_schema.key_column_usage AS kcu ON tc.constraint_name = kcu.constraint_name
JOIN information_schema.constraint_column_usage AS ccu ON ccu.constraint_name = tc.constraint_name
WHERE tc.constraint_type = 'FOREIGN KEY' 
  AND tc.table_schema = 'public'
ORDER BY tc.table_name, kcu.column_name;

-- Step 4: Check RLS policies
SELECT 
  'RLS POLICIES' as check_type,
  tablename,
  policyname,
  cmd,
  permissive
FROM pg_policies 
WHERE schemaname = 'public'
ORDER BY tablename, policyname;

-- Step 5: Check functions
SELECT 
  'FUNCTIONS' as check_type,
  routine_name,
  routine_type,
  data_type
FROM information_schema.routines 
WHERE routine_schema = 'public'
  AND routine_name IN ('create_api_key', 'process_sensor_data', 'handle_watering_command', 'process_pending_commands', 'update_device_health', 'audit_table_changes')
ORDER BY routine_name;

-- Step 6: Check triggers
SELECT 
  'TRIGGERS' as check_type,
  trigger_name,
  event_manipulation,
  event_object_table,
  action_statement
FROM information_schema.triggers 
WHERE trigger_schema = 'public'
ORDER BY event_object_table, trigger_name;

-- Step 7: Check default data
SELECT 
  'DEFAULT DATA' as check_type,
  'soil_types' as table_name,
  COUNT(*) as record_count
FROM soil_types;

-- Step 8: Test API key creation (commented out - uncomment to test)
/*
-- Test API key creation
SELECT create_api_key('test_key', 'sk_test_' || gen_random_uuid()) as test_result;
*/

-- Step 9: Test device health update (commented out - uncomment to test)
/*
-- Test device health update (requires a device to exist)
-- SELECT update_device_health('00000000-0000-0000-0000-000000000000', 'online') as test_result;
*/

-- Step 10: Summary
SELECT 
  'SUMMARY' as check_type,
  'Total Tables' as metric,
  COUNT(*) as value
FROM information_schema.tables 
WHERE table_schema = 'public' 
  AND table_name IN ('zones', 'devices', 'sensor_data', 'watering_controls', 'watering_schedules', 'commands', 'api_keys', 'alerts', 'soil_types', 'audit_logs')

UNION ALL

SELECT 
  'SUMMARY' as check_type,
  'Total Functions' as metric,
  COUNT(*) as value
FROM information_schema.routines 
WHERE routine_schema = 'public'
  AND routine_name IN ('create_api_key', 'process_sensor_data', 'handle_watering_command', 'process_pending_commands', 'update_device_health', 'audit_table_changes')

UNION ALL

SELECT 
  'SUMMARY' as check_type,
  'Total Triggers' as metric,
  COUNT(*) as value
FROM information_schema.triggers 
WHERE trigger_schema = 'public'

UNION ALL

SELECT 
  'SUMMARY' as check_type,
  'Total RLS Policies' as metric,
  COUNT(*) as value
FROM pg_policies 
WHERE schemaname = 'public'; 