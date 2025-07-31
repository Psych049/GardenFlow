-- Test API Keys Table Functionality
-- Run this in your Supabase SQL Editor to verify everything is working

-- Step 1: Check if the table exists and has correct structure
SELECT 
  'Table Structure Check' as test_name,
  COUNT(*) as column_count,
  STRING_AGG(column_name, ', ' ORDER BY ordinal_position) as columns
FROM information_schema.columns 
WHERE table_schema = 'public' 
  AND table_name = 'api_keys';

-- Step 2: Check if the key column exists specifically
SELECT 
  'Key Column Check' as test_name,
  column_name,
  data_type,
  is_nullable
FROM information_schema.columns 
WHERE table_schema = 'public' 
  AND table_name = 'api_keys' 
  AND column_name = 'key';

-- Step 3: Check RLS policies
SELECT 
  'RLS Policies Check' as test_name,
  policyname,
  cmd,
  permissive
FROM pg_policies 
WHERE schemaname = 'public' 
  AND tablename = 'api_keys'
ORDER BY policyname;

-- Step 4: Test insert (this will fail if there are issues)
-- Uncomment the lines below to test actual insertion
/*
INSERT INTO api_keys (name, key, user_id) 
VALUES (
  'test_key_' || EXTRACT(EPOCH FROM NOW())::TEXT,
  'sk_test_' || gen_random_uuid(),
  (SELECT id FROM auth.users LIMIT 1)
) RETURNING id, name, key, created_at;
*/

-- Step 5: Check if RPC function exists
SELECT 
  'RPC Function Check' as test_name,
  routine_name,
  routine_type
FROM information_schema.routines 
WHERE routine_schema = 'public' 
  AND routine_name = 'create_api_key';

-- Step 6: Test RPC function (uncomment to test)
/*
SELECT create_api_key(
  'test_rpc_key_' || EXTRACT(EPOCH FROM NOW())::TEXT,
  'sk_rpc_' || gen_random_uuid()
);
*/ 