-- Diagnostic script for API Keys table
-- Run this in your Supabase SQL editor to check the current state

-- Check if the api_keys table exists
SELECT EXISTS (
  SELECT FROM information_schema.tables 
  WHERE table_schema = 'public' 
  AND table_name = 'api_keys'
) as table_exists;

-- Check the table structure
SELECT 
  column_name, 
  data_type, 
  is_nullable,
  column_default
FROM information_schema.columns 
WHERE table_schema = 'public' 
  AND table_name = 'api_keys' 
ORDER BY ordinal_position;

-- Check if there are any existing API keys
SELECT COUNT(*) as api_key_count FROM api_keys;

-- Check RLS policies
SELECT 
  policyname,
  permissive,
  roles,
  cmd,
  qual,
  with_check
FROM pg_policies 
WHERE tablename = 'api_keys';

-- Test a simple insert (this will fail if there are issues)
-- INSERT INTO api_keys (name, key, user_id) VALUES ('test_diagnostic', 'test_key_123', '00000000-0000-0000-0000-000000000000'); 