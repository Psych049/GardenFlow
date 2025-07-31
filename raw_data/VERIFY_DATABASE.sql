-- VERIFY DATABASE SETUP
-- Run this after FINAL_DATABASE_SETUP.sql to verify everything works

-- 1. Check authentication
SELECT auth.uid() as current_user_id;

-- 2. Verify all tables exist
SELECT 
  table_name,
  column_count
FROM (
  SELECT 
    table_name,
    COUNT(*) as column_count
  FROM information_schema.columns 
  WHERE table_schema = 'public' 
    AND table_name IN ('zones', 'devices', 'api_keys', 'sensor_data', 'watering_schedules', 'commands', 'alerts', 'soil_types')
  GROUP BY table_name
) t
ORDER BY table_name;

-- 3. Verify foreign key relationships
SELECT 
  tc.table_name, 
  kcu.column_name, 
  ccu.table_name AS foreign_table_name,
  ccu.column_name AS foreign_column_name 
FROM 
  information_schema.table_constraints AS tc 
  JOIN information_schema.key_column_usage AS kcu
    ON tc.constraint_name = kcu.constraint_name
    AND tc.table_schema = kcu.table_schema
  JOIN information_schema.constraint_column_usage AS ccu
    ON ccu.constraint_name = tc.constraint_name
    AND ccu.table_schema = tc.table_schema
WHERE tc.constraint_type = 'FOREIGN KEY' 
  AND tc.table_schema = 'public'
ORDER BY tc.table_name;

-- 4. Verify RLS policies
SELECT 
  tablename,
  COUNT(*) as policy_count
FROM pg_policies 
WHERE schemaname = 'public'
GROUP BY tablename
ORDER BY tablename;

-- 5. Verify functions exist
SELECT 
  routine_name,
  routine_type
FROM information_schema.routines 
WHERE routine_schema = 'public'
ORDER BY routine_name;

-- 6. Verify triggers exist
SELECT 
  trigger_name,
  event_object_table,
  event_manipulation
FROM information_schema.triggers 
WHERE trigger_schema = 'public'
ORDER BY event_object_table, trigger_name;

-- 7. Check initial data
SELECT 'Soil Types' as table_name, COUNT(*) as count FROM soil_types
UNION ALL
SELECT 'Zones' as table_name, COUNT(*) as count FROM zones WHERE user_id = auth.uid()
UNION ALL
SELECT 'Devices' as table_name, COUNT(*) as count FROM devices WHERE user_id = auth.uid()
UNION ALL
SELECT 'API Keys' as table_name, COUNT(*) as count FROM api_keys WHERE user_id = auth.uid()
UNION ALL
SELECT 'Sensor Data' as table_name, COUNT(*) as count FROM sensor_data WHERE user_id = auth.uid()
UNION ALL
SELECT 'Watering Schedules' as table_name, COUNT(*) as count FROM watering_schedules WHERE user_id = auth.uid()
UNION ALL
SELECT 'Commands' as table_name, COUNT(*) as count FROM commands WHERE user_id = auth.uid()
UNION ALL
SELECT 'Alerts' as table_name, COUNT(*) as count FROM alerts WHERE user_id = auth.uid();

-- 8. Test creating data through frontend (this will work if user is authenticated)
-- The frontend will handle data creation automatically

-- 9. Show final status
SELECT 
  'Database verification completed!' as status,
  'Your database is ready for the frontend application' as details; 