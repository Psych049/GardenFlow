-- Fix API Keys Table Schema
-- This script ensures the api_keys table has the correct structure

-- Drop the existing api_keys table if it exists (WARNING: This will delete all API keys)
-- DROP TABLE IF EXISTS api_keys CASCADE;

-- Create the api_keys table with the correct structure
CREATE TABLE IF NOT EXISTS api_keys (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  key TEXT UNIQUE NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);

-- Enable RLS on api_keys
ALTER TABLE api_keys ENABLE ROW LEVEL SECURITY;

-- Drop existing policies if they exist
DROP POLICY IF EXISTS "Users can view their own API keys" ON api_keys;
DROP POLICY IF EXISTS "Users can insert their own API keys" ON api_keys;
DROP POLICY IF EXISTS "Users can delete their own API keys" ON api_keys;

-- Create policies for api_keys
CREATE POLICY "Users can view their own API keys" 
  ON api_keys FOR SELECT 
  USING (auth.uid() = user_id);

CREATE POLICY "Users can insert their own API keys" 
  ON api_keys FOR INSERT 
  WITH CHECK (auth.uid() = user_id);

CREATE POLICY "Users can delete their own API keys" 
  ON api_keys FOR DELETE 
  USING (auth.uid() = user_id);

-- Verify the table structure
SELECT column_name, data_type, is_nullable 
FROM information_schema.columns 
WHERE table_name = 'api_keys' 
ORDER BY ordinal_position;

-- Test insert to verify the table works
-- INSERT INTO api_keys (name, key, user_id) VALUES ('test_key', 'test_value', '00000000-0000-0000-0000-000000000000'); 