# API Key Schema Fix Guide

## Problem
You're encountering the error: "Could not find the 'key' column of 'api_keys' in the schema cache"

This happens when the Supabase database schema cache is out of sync with the actual database structure.

## Solution Steps

### Option 1: Quick Fix (Recommended)
1. Go to your Supabase project dashboard
2. Navigate to the SQL Editor
3. Run this command:

```sql
-- Drop and recreate the api_keys table
DROP TABLE IF EXISTS api_keys CASCADE;

CREATE TABLE api_keys (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  key TEXT UNIQUE NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);

-- Enable RLS
ALTER TABLE api_keys ENABLE ROW LEVEL SECURITY;

-- Create policies
CREATE POLICY "Users can view their own API keys" 
  ON api_keys FOR SELECT 
  USING (auth.uid() = user_id);

CREATE POLICY "Users can insert their own API keys" 
  ON api_keys FOR INSERT 
  WITH CHECK (auth.uid() = user_id);

CREATE POLICY "Users can delete their own API keys" 
  ON api_keys FOR DELETE 
  USING (auth.uid() = user_id);
```

### Option 2: Use the Provided Scripts
1. Run `raw_data/fix_api_keys_schema.sql` in your Supabase SQL editor
2. Run `raw_data/create_api_key_function.sql` to create a fallback RPC function

### Option 3: Diagnostic Check
Run `raw_data/diagnose_api_keys.sql` to check the current state of your database.

## What This Fixes
- ✅ Resolves the schema cache issue
- ✅ Ensures the `api_keys` table has the correct structure
- ✅ Sets up proper Row Level Security (RLS) policies
- ✅ Enables API key generation functionality

## After Running the Fix
1. Refresh your application
2. Try creating a device again
3. The API key generation should work properly

## Prevention
To avoid this issue in the future:
- Always run schema migrations in the correct order
- Use the provided initialization scripts
- Check the database schema before deploying changes

## Files Created
- `raw_data/fix_api_keys_schema.sql` - Main fix script
- `raw_data/create_api_key_function.sql` - Fallback RPC function
- `raw_data/diagnose_api_keys.sql` - Diagnostic script
- `API_KEY_SCHEMA_FIX.md` - This guide 