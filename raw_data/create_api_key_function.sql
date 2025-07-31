-- Create RPC function for API key generation (fallback method)
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