-- Create tables and set up Row-Level Security
BEGIN;

-- Create sensor_data table
CREATE TABLE IF NOT EXISTS sensor_data (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  zone_id UUID NOT NULL,
  temperature DECIMAL(5,2) NOT NULL,
  humidity DECIMAL(5,2) NOT NULL,
  soil_moisture DECIMAL(5,2) NOT NULL,
  timestamp TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);

-- Create devices table for ESP32 management
CREATE TABLE IF NOT EXISTS devices (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  device_id TEXT UNIQUE NOT NULL,
  device_type TEXT NOT NULL DEFAULT 'esp32',
  status TEXT NOT NULL DEFAULT 'offline',
  last_seen TIMESTAMPTZ DEFAULT NOW(),
  ip_address INET,
  mac_address TEXT,
  firmware_version TEXT,
  user_id UUID REFERENCES auth.users NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

-- Create watering_controls table
CREATE TABLE IF NOT EXISTS watering_controls (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  zone_id UUID NOT NULL,
  device_id UUID REFERENCES devices(id),
  pump_pin INTEGER NOT NULL,
  valve_pin INTEGER,
  is_active BOOLEAN DEFAULT FALSE,
  auto_mode BOOLEAN DEFAULT TRUE,
  moisture_threshold DECIMAL(5,2) DEFAULT 30.0,
  watering_duration INTEGER DEFAULT 30, -- seconds
  last_watered TIMESTAMPTZ,
  user_id UUID REFERENCES auth.users NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

-- Create watering_schedules table
CREATE TABLE IF NOT EXISTS watering_schedules (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  zone_id UUID NOT NULL,
  name TEXT NOT NULL,
  cron_expression TEXT NOT NULL,
  duration INTEGER DEFAULT 30, -- seconds
  is_active BOOLEAN DEFAULT TRUE,
  user_id UUID REFERENCES auth.users NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
);

-- Create commands table for real-time ESP32 control
CREATE TABLE IF NOT EXISTS commands (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  device_id UUID REFERENCES devices(id),
  command_type TEXT NOT NULL, -- 'pump_on', 'pump_off', 'valve_on', 'valve_off', 'auto_mode', 'manual_mode'
  parameters JSONB,
  status TEXT DEFAULT 'pending', -- 'pending', 'sent', 'executed', 'failed'
  executed_at TIMESTAMPTZ,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);

-- Create api_keys table
CREATE TABLE IF NOT EXISTS api_keys (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  key TEXT UNIQUE NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);

-- Enable RLS on api_keys
ALTER TABLE api_keys ENABLE ROW LEVEL SECURITY;

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

-- Create soil_types table
CREATE TABLE IF NOT EXISTS soil_types (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  type TEXT NOT NULL,
  ideal_plants JSONB NOT NULL,
  watering_tips TEXT NOT NULL,
  amendments TEXT NOT NULL,
  characteristics TEXT NOT NULL,
  user_id UUID REFERENCES auth.users
);

-- Create alerts table
CREATE TABLE IF NOT EXISTS alerts (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  type TEXT NOT NULL,
  zone TEXT NOT NULL,
  message TEXT NOT NULL,
  severity TEXT NOT NULL,
  timestamp TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL,
  read BOOLEAN DEFAULT FALSE NOT NULL
);

-- Create zones table
CREATE TABLE IF NOT EXISTS zones (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  soil_type TEXT NOT NULL,
  sensor_id TEXT NOT NULL,
  moisture_level DECIMAL(5,2) NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);

-- Enable RLS on all tables
ALTER TABLE devices ENABLE ROW LEVEL SECURITY;
ALTER TABLE watering_controls ENABLE ROW LEVEL SECURITY;
ALTER TABLE watering_schedules ENABLE ROW LEVEL SECURITY;
ALTER TABLE commands ENABLE ROW LEVEL SECURITY;

-- Create policies for devices
CREATE POLICY "Users can view their own devices" 
  ON devices FOR SELECT 
  USING (auth.uid() = user_id);

CREATE POLICY "Users can insert their own devices" 
  ON devices FOR INSERT 
  WITH CHECK (auth.uid() = user_id);

CREATE POLICY "Users can update their own devices" 
  ON devices FOR UPDATE
  USING (auth.uid() = user_id);

CREATE POLICY "Users can delete their own devices" 
  ON devices FOR DELETE 
  USING (auth.uid() = user_id);

-- Create policies for watering_controls
CREATE POLICY "Users can view their own watering controls" 
  ON watering_controls FOR SELECT 
  USING (auth.uid() = user_id);

CREATE POLICY "Users can insert their own watering controls" 
  ON watering_controls FOR INSERT 
  WITH CHECK (auth.uid() = user_id);

CREATE POLICY "Users can update their own watering controls" 
  ON watering_controls FOR UPDATE
  USING (auth.uid() = user_id);

CREATE POLICY "Users can delete their own watering controls" 
  ON watering_controls FOR DELETE 
  USING (auth.uid() = user_id);

-- Create policies for watering_schedules
CREATE POLICY "Users can view their own watering schedules" 
  ON watering_schedules FOR SELECT 
  USING (auth.uid() = user_id);

CREATE POLICY "Users can insert their own watering schedules" 
  ON watering_schedules FOR INSERT 
  WITH CHECK (auth.uid() = user_id);

CREATE POLICY "Users can update their own watering schedules" 
  ON watering_schedules FOR UPDATE
  USING (auth.uid() = user_id);

CREATE POLICY "Users can delete their own watering schedules" 
  ON watering_schedules FOR DELETE 
  USING (auth.uid() = user_id);

-- Create policies for commands
CREATE POLICY "Users can view their own commands" 
  ON commands FOR SELECT 
  USING (auth.uid() = user_id);

CREATE POLICY "Users can insert their own commands" 
  ON commands FOR INSERT 
  WITH CHECK (auth.uid() = user_id);

CREATE POLICY "Users can update their own commands" 
  ON commands FOR UPDATE
  USING (auth.uid() = user_id);

-- Create stored procedures for initializing tables
CREATE OR REPLACE FUNCTION init_sensor_data_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'sensor_data') THEN
    CREATE TABLE sensor_data (
      id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      zone_id UUID NOT NULL,
      temperature DECIMAL(5,2) NOT NULL,
      humidity DECIMAL(5,2) NOT NULL,
      soil_moisture DECIMAL(5,2) NOT NULL,
      timestamp TIMESTAMPTZ DEFAULT NOW() NOT NULL,
      user_id UUID REFERENCES auth.users NOT NULL
    );
    
    -- Enable RLS
    ALTER TABLE sensor_data ENABLE ROW LEVEL SECURITY;

    -- Create policies
    CREATE POLICY "Users can view their own sensor data" 
      ON sensor_data FOR SELECT 
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can insert their own sensor data" 
      ON sensor_data FOR INSERT 
      WITH CHECK (auth.uid() = user_id);
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION init_devices_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'devices') THEN
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
      user_id UUID REFERENCES auth.users NOT NULL,
      created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
    );
    
    -- Enable RLS
    ALTER TABLE devices ENABLE ROW LEVEL SECURITY;

    -- Create policies
    CREATE POLICY "Users can view their own devices" 
      ON devices FOR SELECT 
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can insert their own devices" 
      ON devices FOR INSERT 
      WITH CHECK (auth.uid() = user_id);

    CREATE POLICY "Users can update their own devices" 
      ON devices FOR UPDATE
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can delete their own devices" 
      ON devices FOR DELETE 
      USING (auth.uid() = user_id);
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION init_watering_controls_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'watering_controls') THEN
    CREATE TABLE watering_controls (
      id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      zone_id UUID NOT NULL,
      device_id UUID REFERENCES devices(id),
      pump_pin INTEGER NOT NULL,
      valve_pin INTEGER,
      is_active BOOLEAN DEFAULT FALSE,
      auto_mode BOOLEAN DEFAULT TRUE,
      moisture_threshold DECIMAL(5,2) DEFAULT 30.0,
      watering_duration INTEGER DEFAULT 30,
      last_watered TIMESTAMPTZ,
      user_id UUID REFERENCES auth.users NOT NULL,
      created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
    );
    
    -- Enable RLS
    ALTER TABLE watering_controls ENABLE ROW LEVEL SECURITY;

    -- Create policies
    CREATE POLICY "Users can view their own watering controls" 
      ON watering_controls FOR SELECT 
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can insert their own watering controls" 
      ON watering_controls FOR INSERT 
      WITH CHECK (auth.uid() = user_id);

    CREATE POLICY "Users can update their own watering controls" 
      ON watering_controls FOR UPDATE
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can delete their own watering controls" 
      ON watering_controls FOR DELETE 
      USING (auth.uid() = user_id);
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION init_watering_schedules_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'watering_schedules') THEN
    CREATE TABLE watering_schedules (
      id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      zone_id UUID NOT NULL,
      name TEXT NOT NULL,
      cron_expression TEXT NOT NULL,
      duration INTEGER DEFAULT 30,
      is_active BOOLEAN DEFAULT TRUE,
      user_id UUID REFERENCES auth.users NOT NULL,
      created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL
    );
    
    -- Enable RLS
    ALTER TABLE watering_schedules ENABLE ROW LEVEL SECURITY;

    -- Create policies
    CREATE POLICY "Users can view their own watering schedules" 
      ON watering_schedules FOR SELECT 
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can insert their own watering schedules" 
      ON watering_schedules FOR INSERT 
      WITH CHECK (auth.uid() = user_id);

    CREATE POLICY "Users can update their own watering schedules" 
      ON watering_schedules FOR UPDATE
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can delete their own watering schedules" 
      ON watering_schedules FOR DELETE 
      USING (auth.uid() = user_id);
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION init_commands_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'commands') THEN
    CREATE TABLE commands (
      id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      device_id UUID REFERENCES devices(id),
      command_type TEXT NOT NULL,
      parameters JSONB,
      status TEXT DEFAULT 'pending',
      executed_at TIMESTAMPTZ,
      created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
      user_id UUID REFERENCES auth.users NOT NULL
    );
    
    -- Enable RLS
    ALTER TABLE commands ENABLE ROW LEVEL SECURITY;

    -- Create policies
    CREATE POLICY "Users can view their own commands" 
      ON commands FOR SELECT 
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can insert their own commands" 
      ON commands FOR INSERT 
      WITH CHECK (auth.uid() = user_id);

    CREATE POLICY "Users can update their own commands" 
      ON commands FOR UPDATE
      USING (auth.uid() = user_id);
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION init_soil_types_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'soil_types') THEN
    CREATE TABLE soil_types (
      id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      type TEXT NOT NULL,
      ideal_plants JSONB NOT NULL,
      watering_tips TEXT NOT NULL,
      amendments TEXT NOT NULL,
      characteristics TEXT NOT NULL,
      user_id UUID REFERENCES auth.users
    );
    
    -- Enable RLS
    ALTER TABLE soil_types ENABLE ROW LEVEL SECURITY;

    -- Create policies
    CREATE POLICY "Anyone can view public soil types" 
      ON soil_types FOR SELECT 
      USING (user_id IS NULL OR auth.uid() = user_id);

    CREATE POLICY "Users can insert their own soil types" 
      ON soil_types FOR INSERT 
      WITH CHECK (auth.uid() = user_id);
      
    -- Insert default soil types (public)
    INSERT INTO soil_types (type, ideal_plants, watering_tips, amendments, characteristics, user_id)
    VALUES 
      (
        'Loam soil', 
        '["Tomatoes", "Peppers", "Cucumbers", "Zucchini", "Roses"]'::jsonb, 
        'Water deeply but less frequently to encourage root growth.', 
        'Add compost yearly to maintain organic content.', 
        'Equal parts of sand, silt, and clay. Excellent drainage and moisture retention.',
        NULL
      ),
      (
        'Sandy soil', 
        '["Lavender", "Rosemary", "Cacti", "Sedums", "Zinnias"]'::jsonb, 
        'Water more frequently but in smaller amounts.', 
        'Add compost and mulch to improve water retention.', 
        'Gritty texture with large particles. Drains quickly but retains little moisture.',
        NULL
      ),
      (
        'Potting Mix', 
        '["Basil", "Mint", "Parsley", "Thyme", "Cilantro"]'::jsonb, 
        'Check moisture level daily, water when top inch feels dry.', 
        'Replace or refresh annually with fresh mix.', 
        'Lightweight and sterile. Good drainage with adequate water retention.',
        NULL
      ),
      (
        'Peat soil', 
        '["Ferns", "Orchids", "African Violets", "Peace Lily", "Spider Plant"]'::jsonb, 
        'Water thoroughly when top layer becomes dry, avoid overwatering.', 
        'Mix with perlite or sand to improve drainage.', 
        'Dark, rich organic material. Excellent water retention but can become compacted.',
        NULL
      );
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION init_api_keys_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'api_keys') THEN
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
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION init_alerts_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'alerts') THEN
    CREATE TABLE alerts (
      id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      type TEXT NOT NULL,
      zone TEXT NOT NULL,
      message TEXT NOT NULL,
      severity TEXT NOT NULL,
      timestamp TIMESTAMPTZ DEFAULT NOW() NOT NULL,
      user_id UUID REFERENCES auth.users NOT NULL,
      read BOOLEAN DEFAULT FALSE NOT NULL
    );
    
    -- Enable RLS
    ALTER TABLE alerts ENABLE ROW LEVEL SECURITY;

    -- Create policies
    CREATE POLICY "Users can view their own alerts" 
      ON alerts FOR SELECT 
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can insert their own alerts" 
      ON alerts FOR INSERT 
      WITH CHECK (auth.uid() = user_id);
      
    CREATE POLICY "Users can update their own alerts" 
      ON alerts FOR UPDATE
      USING (auth.uid() = user_id);
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION init_zones_table() RETURNS void AS $$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_tables WHERE tablename = 'zones') THEN
    CREATE TABLE zones (
      id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      name TEXT NOT NULL,
      soil_type TEXT NOT NULL,
      sensor_id TEXT NOT NULL,
      moisture_level DECIMAL(5,2) NOT NULL,
      user_id UUID REFERENCES auth.users NOT NULL
    );
    
    -- Enable RLS
    ALTER TABLE zones ENABLE ROW LEVEL SECURITY;

    -- Create policies
    CREATE POLICY "Users can view their own zones" 
      ON zones FOR SELECT 
      USING (auth.uid() = user_id);

    CREATE POLICY "Users can insert their own zones" 
      ON zones FOR INSERT 
      WITH CHECK (auth.uid() = user_id);
      
    CREATE POLICY "Users can update their own zones" 
      ON zones FOR UPDATE
      USING (auth.uid() = user_id);
  END IF;
END;
$$ LANGUAGE plpgsql;

-- Create function to process new sensor data and create alerts
CREATE OR REPLACE FUNCTION process_sensor_data() 
RETURNS TRIGGER AS $$
BEGIN
  -- Check moisture levels and create alerts if needed
  IF NEW.soil_moisture < 25 THEN
    INSERT INTO alerts (type, zone, message, severity, user_id)
    SELECT 'warning', z.name, 'Soil moisture critically low (' || NEW.soil_moisture || '%)', 'high', NEW.user_id
    FROM zones z WHERE z.id = NEW.zone_id;
  ELSIF NEW.soil_moisture < 30 THEN
    INSERT INTO alerts (type, zone, message, severity, user_id)
    SELECT 'warning', z.name, 'Soil moisture below optimal levels (' || NEW.soil_moisture || '%)', 'medium', NEW.user_id
    FROM zones z WHERE z.id = NEW.zone_id;
  END IF;
  
  -- Check temperature and create alerts if needed
  IF NEW.temperature > 30 THEN
    INSERT INTO alerts (type, zone, message, severity, user_id)
    SELECT 'warning', z.name, 'Temperature too high (' || NEW.temperature || '°C)', 'medium', NEW.user_id
    FROM zones z WHERE z.id = NEW.zone_id;
  END IF;
  
  -- Update zone moisture level
  UPDATE zones SET moisture_level = NEW.soil_moisture
  WHERE id = NEW.zone_id;
  
  -- Check if auto watering is needed
  INSERT INTO commands (device_id, command_type, parameters, user_id)
  SELECT wc.device_id, 'check_watering', 
         jsonb_build_object('moisture', NEW.soil_moisture, 'threshold', wc.moisture_threshold),
         NEW.user_id
  FROM watering_controls wc
  WHERE wc.zone_id = NEW.zone_id 
    AND wc.auto_mode = true 
    AND wc.is_active = true
    AND NEW.soil_moisture < wc.moisture_threshold;
  
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Create function to handle watering commands
CREATE OR REPLACE FUNCTION handle_watering_command() 
RETURNS TRIGGER AS $$
BEGIN
  -- If it's a check_watering command and moisture is below threshold
  IF NEW.command_type = 'check_watering' AND NEW.parameters->>'moisture'::text < NEW.parameters->>'threshold'::text THEN
    -- Create a pump_on command
    INSERT INTO commands (device_id, command_type, parameters, user_id)
    VALUES (NEW.device_id, 'pump_on', 
            jsonb_build_object('duration', 
              (SELECT watering_duration FROM watering_controls WHERE device_id = NEW.device_id LIMIT 1)), 
            NEW.user_id);
  END IF;
  
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Create trigger for sensor data processing
CREATE TRIGGER sensor_data_trigger
  AFTER INSERT ON sensor_data
  FOR EACH ROW
  EXECUTE FUNCTION process_sensor_data();

-- Create trigger for watering command processing
CREATE TRIGGER watering_command_trigger
  AFTER INSERT ON commands
  FOR EACH ROW
  EXECUTE FUNCTION handle_watering_command();

-- Execute initialization functions
SELECT init_sensor_data_table();
SELECT init_devices_table();
SELECT init_watering_controls_table();
SELECT init_watering_schedules_table();
SELECT init_commands_table();
SELECT init_soil_types_table();
SELECT init_alerts_table();
SELECT init_zones_table();
SELECT init_api_keys_table();

COMMIT;