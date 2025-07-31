-- Manual Database Setup - No Authentication Required
-- Run this in your Supabase SQL Editor

-- Step 1: Drop everything first
DROP TRIGGER IF EXISTS set_user_id_trigger ON watering_schedules;
DROP TRIGGER IF EXISTS sensor_data_trigger ON sensor_data;
DROP TRIGGER IF EXISTS watering_command_trigger ON commands;
DROP TRIGGER IF EXISTS devices_audit_trigger ON devices;

DROP FUNCTION IF EXISTS set_user_id_on_insert();
DROP FUNCTION IF EXISTS process_sensor_data();
DROP FUNCTION IF EXISTS handle_watering_command();
DROP FUNCTION IF EXISTS audit_table_changes();
DROP FUNCTION IF EXISTS validate_api_key();
DROP FUNCTION IF EXISTS get_device_from_api_key();
DROP FUNCTION IF EXISTS create_device_api_key();

DROP TABLE IF EXISTS audit_logs;
DROP TABLE IF EXISTS commands;
DROP TABLE IF EXISTS watering_schedules;
DROP TABLE IF EXISTS watering_controls;
DROP TABLE IF EXISTS sensor_data;
DROP TABLE IF EXISTS api_keys;
DROP TABLE IF EXISTS devices;
DROP TABLE IF EXISTS alerts;
DROP TABLE IF EXISTS zones;
DROP TABLE IF EXISTS soil_types;

-- Step 2: Create tables in order
CREATE TABLE soil_types (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  type TEXT NOT NULL UNIQUE,
  ideal_plants JSONB NOT NULL,
  watering_tips TEXT NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

CREATE TABLE zones (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  description TEXT,
  soil_type TEXT NOT NULL,
  moisture_threshold DECIMAL(5,2) DEFAULT 30.0,
  pump_on BOOLEAN DEFAULT FALSE,
  user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

CREATE TABLE devices (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  device_id TEXT UNIQUE NOT NULL,
  device_type TEXT NOT NULL DEFAULT 'esp32',
  status TEXT NOT NULL DEFAULT 'offline',
  last_seen TIMESTAMPTZ DEFAULT NOW(),
  ip_address INET,
  mac_address TEXT,
  firmware_version TEXT,
  user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

CREATE TABLE api_keys (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  key_value TEXT UNIQUE NOT NULL,
  device_id UUID REFERENCES devices(id) ON DELETE CASCADE,
  name TEXT NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL
);

CREATE TABLE sensor_data (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  zone_id UUID REFERENCES zones(id) ON DELETE CASCADE NOT NULL,
  temperature DECIMAL(5,2) NOT NULL,
  humidity DECIMAL(5,2) NOT NULL,
  soil_moisture DECIMAL(5,2) NOT NULL,
  timestamp TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL
);

CREATE TABLE watering_controls (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  zone_id UUID REFERENCES zones(id) ON DELETE CASCADE NOT NULL,
  device_id UUID REFERENCES devices(id) ON DELETE CASCADE,
  pump_pin INTEGER NOT NULL,
  valve_pin INTEGER,
  is_active BOOLEAN DEFAULT FALSE,
  auto_mode BOOLEAN DEFAULT TRUE,
  moisture_threshold DECIMAL(5,2) DEFAULT 30.0,
  watering_duration INTEGER DEFAULT 30,
  last_watered TIMESTAMPTZ,
  user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

CREATE TABLE watering_schedules (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  zone_id UUID REFERENCES zones(id) ON DELETE CASCADE NOT NULL,
  name TEXT NOT NULL,
  cron_expression TEXT NOT NULL,
  duration INTEGER DEFAULT 30,
  is_active BOOLEAN DEFAULT TRUE,
  user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

CREATE TABLE commands (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  device_id UUID REFERENCES devices(id) ON DELETE CASCADE,
  command_type TEXT NOT NULL,
  parameters JSONB,
  status TEXT DEFAULT 'pending',
  executed_at TIMESTAMPTZ,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL
);

CREATE TABLE alerts (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  type TEXT NOT NULL,
  message TEXT NOT NULL,
  severity TEXT NOT NULL DEFAULT 'info',
  read BOOLEAN DEFAULT FALSE,
  timestamp TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users(id) ON DELETE CASCADE NOT NULL
);

CREATE TABLE audit_logs (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  table_name TEXT NOT NULL,
  operation TEXT NOT NULL,
  old_data JSONB,
  new_data JSONB,
  timestamp TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

-- Step 3: Enable RLS
ALTER TABLE soil_types ENABLE ROW LEVEL SECURITY;
ALTER TABLE zones ENABLE ROW LEVEL SECURITY;
ALTER TABLE devices ENABLE ROW LEVEL SECURITY;
ALTER TABLE api_keys ENABLE ROW LEVEL SECURITY;
ALTER TABLE sensor_data ENABLE ROW LEVEL SECURITY;
ALTER TABLE watering_controls ENABLE ROW LEVEL SECURITY;
ALTER TABLE watering_schedules ENABLE ROW LEVEL SECURITY;
ALTER TABLE commands ENABLE ROW LEVEL SECURITY;
ALTER TABLE alerts ENABLE ROW LEVEL SECURITY;

-- Step 4: Create RLS policies
CREATE POLICY "Anyone can view soil types" ON soil_types FOR SELECT USING (true);

CREATE POLICY "Users can view their own zones" ON zones FOR SELECT USING (auth.uid() = user_id);
CREATE POLICY "Users can insert their own zones" ON zones FOR INSERT WITH CHECK (auth.uid() = user_id);
CREATE POLICY "Users can update their own zones" ON zones FOR UPDATE USING (auth.uid() = user_id);
CREATE POLICY "Users can delete their own zones" ON zones FOR DELETE USING (auth.uid() = user_id);

CREATE POLICY "Users can view their own devices" ON devices FOR SELECT USING (auth.uid() = user_id);
CREATE POLICY "Users can insert their own devices" ON devices FOR INSERT WITH CHECK (auth.uid() = user_id);
CREATE POLICY "Users can update their own devices" ON devices FOR UPDATE USING (auth.uid() = user_id);
CREATE POLICY "Users can delete their own devices" ON devices FOR DELETE USING (auth.uid() = user_id);

CREATE POLICY "Users can view their own API keys" ON api_keys FOR SELECT USING (auth.uid() = user_id);
CREATE POLICY "Users can insert their own API keys" ON api_keys FOR INSERT WITH CHECK (auth.uid() = user_id);
CREATE POLICY "Users can update their own API keys" ON api_keys FOR UPDATE USING (auth.uid() = user_id);
CREATE POLICY "Users can delete their own API keys" ON api_keys FOR DELETE USING (auth.uid() = user_id);

CREATE POLICY "Users can view their own sensor data" ON sensor_data FOR SELECT USING (auth.uid() = user_id);
CREATE POLICY "Users can insert their own sensor data" ON sensor_data FOR INSERT WITH CHECK (auth.uid() = user_id);

CREATE POLICY "Users can view their own watering controls" ON watering_controls FOR SELECT USING (auth.uid() = user_id);
CREATE POLICY "Users can insert their own watering controls" ON watering_controls FOR INSERT WITH CHECK (auth.uid() = user_id);
CREATE POLICY "Users can update their own watering controls" ON watering_controls FOR UPDATE USING (auth.uid() = user_id);
CREATE POLICY "Users can delete their own watering controls" ON watering_controls FOR DELETE USING (auth.uid() = user_id);

CREATE POLICY "Users can view their own watering schedules" ON watering_schedules FOR SELECT USING (auth.uid() = user_id);
CREATE POLICY "Users can insert their own watering schedules" ON watering_schedules FOR INSERT WITH CHECK (auth.uid() = user_id);
CREATE POLICY "Users can update their own watering schedules" ON watering_schedules FOR UPDATE USING (auth.uid() = user_id);
CREATE POLICY "Users can delete their own watering schedules" ON watering_schedules FOR DELETE USING (auth.uid() = user_id);

CREATE POLICY "Users can view their own commands" ON commands FOR SELECT USING (auth.uid() = user_id);
CREATE POLICY "Users can insert their own commands" ON commands FOR INSERT WITH CHECK (auth.uid() = user_id);
CREATE POLICY "Users can update their own commands" ON commands FOR UPDATE USING (auth.uid() = user_id);
CREATE POLICY "Users can delete their own commands" ON commands FOR DELETE USING (auth.uid() = user_id);

CREATE POLICY "Users can view their own alerts" ON alerts FOR SELECT USING (auth.uid() = user_id);
CREATE POLICY "Users can insert their own alerts" ON alerts FOR INSERT WITH CHECK (auth.uid() = user_id);
CREATE POLICY "Users can update their own alerts" ON alerts FOR UPDATE USING (auth.uid() = user_id);
CREATE POLICY "Users can delete their own alerts" ON alerts FOR DELETE USING (auth.uid() = user_id);

-- Step 5: Create functions
CREATE OR REPLACE FUNCTION set_user_id_on_insert()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.user_id IS NULL THEN
        NEW.user_id = auth.uid();
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

CREATE OR REPLACE FUNCTION process_sensor_data()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.soil_moisture < 20 THEN
        INSERT INTO alerts (type, message, severity, user_id)
        VALUES (
            'moisture_low',
            'Low soil moisture detected in zone: ' || (SELECT name FROM zones WHERE id = NEW.zone_id),
            'warning',
            NEW.user_id
        );
    END IF;
    
    IF NEW.temperature > 35 THEN
        INSERT INTO alerts (type, message, severity, user_id)
        VALUES (
            'temperature_high',
            'High temperature detected in zone: ' || (SELECT name FROM zones WHERE id = NEW.zone_id),
            'warning',
            NEW.user_id
        );
    END IF;
    
    UPDATE zones 
    SET moisture_threshold = NEW.soil_moisture
    WHERE id = NEW.zone_id;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

CREATE OR REPLACE FUNCTION validate_api_key(input_key TEXT)
RETURNS BOOLEAN AS $$
BEGIN
    RETURN EXISTS (
        SELECT 1 FROM api_keys 
        WHERE key_value = input_key
    );
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

CREATE OR REPLACE FUNCTION get_device_from_api_key(input_key TEXT)
RETURNS UUID AS $$
DECLARE
    device_uuid UUID;
BEGIN
    SELECT device_id INTO device_uuid
    FROM api_keys 
    WHERE key_value = input_key;
    
    RETURN device_uuid;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

CREATE OR REPLACE FUNCTION create_device_api_key(device_uuid UUID, key_name TEXT)
RETURNS TEXT AS $$
DECLARE
    new_key TEXT;
BEGIN
    new_key := encode(gen_random_bytes(32), 'base64');
    
    INSERT INTO api_keys (key_value, device_id, name, user_id)
    VALUES (new_key, device_uuid, key_name, (SELECT user_id FROM devices WHERE id = device_uuid));
    
    RETURN new_key;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

-- Step 6: Create triggers
CREATE TRIGGER set_user_id_trigger
    BEFORE INSERT ON watering_schedules
    FOR EACH ROW
    EXECUTE FUNCTION set_user_id_on_insert();

CREATE TRIGGER sensor_data_trigger
    AFTER INSERT ON sensor_data
    FOR EACH ROW
    EXECUTE FUNCTION process_sensor_data();

-- Step 7: Insert initial data
INSERT INTO soil_types (type, ideal_plants, watering_tips) VALUES
('Sandy', '["Cacti", "Succulents", "Herbs"]', 'Water less frequently, allow soil to dry between watering'),
('Loamy', '["Vegetables", "Flowers", "Most Plants"]', 'Water when top inch of soil feels dry'),
('Clay', '["Rice", "Willows", "Some Trees"]', 'Water slowly and less frequently, clay retains moisture'),
('Silty', '["Grasses", "Cereals", "Many Crops"]', 'Water moderately, silty soil holds moisture well');

-- Step 8: Show status
SELECT 'Database setup completed successfully!' as status;
SELECT 'Now you can create data through the frontend or manually with your user ID' as next_step; 