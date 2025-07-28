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
  
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Create trigger for sensor data processing
CREATE TRIGGER sensor_data_trigger
  AFTER INSERT ON sensor_data
  FOR EACH ROW
  EXECUTE FUNCTION process_sensor_data();

-- Execute initialization functions
SELECT init_sensor_data_table();
SELECT init_soil_types_table();
SELECT init_alerts_table();
SELECT init_zones_table();
SELECT init_api_keys_table();

COMMIT;