-- NUCLEAR OPTION: Complete API Keys Table Reset
-- WARNING: This will delete ALL API keys and recreate the table from scratch
-- Only use this if other fixes don't work

-- Step 1: Drop everything related to api_keys
DROP TABLE IF EXISTS public.api_keys CASCADE;
DROP POLICY IF EXISTS "Users can view their own API keys" ON public.api_keys;
DROP POLICY IF EXISTS "Users can insert their own API keys" ON public.api_keys;
DROP POLICY IF EXISTS "Users can delete their own API keys" ON public.api_keys;
DROP POLICY IF EXISTS "Users can update their own API keys" ON public.api_keys;

-- Step 2: Drop the RPC function if it exists
DROP FUNCTION IF EXISTS create_api_key(TEXT, TEXT);

-- Step 3: Create the table completely fresh
CREATE TABLE public.api_keys (
  id uuid NOT NULL DEFAULT gen_random_uuid(),
  name text NOT NULL,
  key text NOT NULL,
  created_at timestamp with time zone NOT NULL DEFAULT now(),
  user_id uuid NOT NULL,
  CONSTRAINT api_keys_pkey PRIMARY KEY (id),
  CONSTRAINT api_keys_key_key UNIQUE (key),
  CONSTRAINT api_keys_user_id_fkey FOREIGN KEY (user_id) REFERENCES auth.users (id) ON DELETE CASCADE
) TABLESPACE pg_default;

-- Step 4: Enable RLS
ALTER TABLE public.api_keys ENABLE ROW LEVEL SECURITY;

-- Step 5: Create policies
CREATE POLICY "Users can view their own API keys" 
  ON public.api_keys FOR SELECT 
  USING (auth.uid() = user_id);

CREATE POLICY "Users can insert their own API keys" 
  ON public.api_keys FOR INSERT 
  WITH CHECK (auth.uid() = user_id);

CREATE POLICY "Users can delete their own API keys" 
  ON public.api_keys FOR DELETE 
  USING (auth.uid() = user_id);

-- Step 6: Create the RPC function
CREATE OR REPLACE FUNCTION create_api_key(
  key_name TEXT,
  key_value TEXT
) RETURNS JSONB AS $$
DECLARE
  current_user_id UUID;
  new_api_key_id UUID;
  result JSONB;
BEGIN
  -- Get current user ID
  current_user_id := auth.uid();
  
  IF current_user_id IS NULL THEN
    RAISE EXCEPTION 'User not authenticated';
  END IF;
  
  -- Insert the API key
  INSERT INTO api_keys (name, key, user_id)
  VALUES (key_name, key_value, current_user_id)
  RETURNING id INTO new_api_key_id;
  
  -- Return the created API key data
  SELECT jsonb_build_object(
    'id', id,
    'name', name,
    'key', key,
    'created_at', created_at,
    'user_id', user_id
  ) INTO result
  FROM api_keys
  WHERE id = new_api_key_id;
  
  RETURN result;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

-- Grant execute permission to authenticated users
GRANT EXECUTE ON FUNCTION create_api_key(TEXT, TEXT) TO authenticated;

-- Step 7: Verify everything is working
SELECT 
  'Table created successfully' as status,
  COUNT(*) as column_count
FROM information_schema.columns 
WHERE table_schema = 'public' 
  AND table_name = 'api_keys';

-- Step 8: Test insert (commented out - uncomment to test)
-- INSERT INTO api_keys (name, key, user_id) 
-- VALUES ('test_key', 'sk_test_' || gen_random_uuid(), '00000000-0000-0000-0000-000000000000')
-- RETURNING *; 