# Plant Monitoring System - API Documentation

## Overview

This document describes all the APIs available in the Plant Monitoring System, including Supabase Edge Functions and database endpoints.

## Base URLs

- **Supabase Project URL**: `https://your-project.supabase.co`
- **Edge Functions Base**: `https://your-project.supabase.co/functions/v1`

## Authentication

### API Key Authentication
Most endpoints require an API key for ESP32 devices:
```
Authorization: Bearer YOUR_SUPABASE_ANON_KEY
Content-Type: application/json
```

### User Authentication
Dashboard endpoints require user authentication:
```
Authorization: Bearer USER_SESSION_TOKEN
```

## Edge Functions

### 1. ESP32 Data Collection

**Endpoint**: `POST /functions/v1/esp32-data`

**Description**: Receives sensor data from ESP32 devices and stores it in the database.

**Request Body**:
```json
{
  "sensor_id": "ZONE_001",
  "temperature": 24.5,
  "humidity": 45.2,
  "soil_moisture": 35.8,
  "apiKey": "sk_plant_monitor_abc123"
}
```

**Response**:
```json
{
  "message": "Data received successfully",
  "irrigation_needed": false,
  "data": {
    "id": "uuid",
    "zone_id": "uuid",
    "temperature": 24.5,
    "humidity": 45.2,
    "soil_moisture": 35.8,
    "timestamp": "2024-01-15T10:30:00Z"
  }
}
```

**Error Responses**:
- `400`: Missing required fields
- `401`: Invalid API key
- `404`: Zone not found

### 2. ESP32 Commands

**Endpoint**: `POST /functions/v1/esp32-commands`

**Description**: Sends commands to ESP32 devices from the dashboard.

**Request Body**:
```json
{
  "device_id": "uuid",
  "command_type": "pump_on",
  "parameters": {
    "duration": 30
  }
}
```

**Command Types**:
- `pump_on`: Turn on water pump
- `pump_off`: Turn off water pump
- `valve_on`: Open valve
- `valve_off`: Close valve
- `auto_mode`: Enable automatic watering
- `manual_mode`: Disable automatic watering

**Response**:
```json
{
  "message": "Command sent successfully",
  "command": {
    "id": "uuid",
    "device_id": "uuid",
    "command_type": "pump_on",
    "parameters": {"duration": 30},
    "status": "pending",
    "created_at": "2024-01-15T10:30:00Z"
  }
}
```

**Get Pending Commands**: `GET /functions/v1/esp32-commands?device_id=uuid&apiKey=key`

**Update Command Status**: `PUT /functions/v1/esp32-commands`
```json
{
  "command_id": "uuid",
  "status": "executed",
  "apiKey": "sk_plant_monitor_abc123"
}
```

### 3. Device Management

**Endpoint**: `POST /functions/v1/device-management`

**Description**: Registers and manages ESP32 devices.

**Request Body**:
```json
{
  "device_id": "ESP32_PLANT_MONITOR_001",
  "name": "Garden Monitor 1",
  "device_type": "esp32",
  "ip_address": "192.168.1.100",
  "mac_address": "AA:BB:CC:DD:EE:FF",
  "firmware_version": "1.0.0",
  "apiKey": "sk_plant_monitor_abc123"
}
```

**Response**:
```json
{
  "message": "Device registered successfully",
  "device": {
    "id": "uuid",
    "device_id": "ESP32_PLANT_MONITOR_001",
    "name": "Garden Monitor 1",
    "status": "online",
    "last_seen": "2024-01-15T10:30:00Z"
  }
}
```

**Get User Devices**: `GET /functions/v1/device-management`

**Update Device Status**: `PUT /functions/v1/device-management`
```json
{
  "device_id": "ESP32_PLANT_MONITOR_001",
  "status": "online",
  "apiKey": "sk_plant_monitor_abc123"
}
```

**Delete Device**: `DELETE /functions/v1/device-management`
```json
{
  "device_id": "uuid"
}
```

### 4. Simulate Sensor Data

**Endpoint**: `POST /functions/v1/simulate-sensor-data`

**Description**: Generates simulated sensor data for testing (requires user authentication).

**Response**:
```json
{
  "message": "Sensor data simulated successfully",
  "data": [
    {
      "id": "uuid",
      "zone_id": "uuid",
      "temperature": 24.5,
      "humidity": 45.2,
      "soil_moisture": 35.8,
      "timestamp": "2024-01-15T10:30:00Z"
    }
  ]
}
```

## Database Tables

### 1. sensor_data

Stores sensor readings from ESP32 devices.

**Schema**:
```sql
CREATE TABLE sensor_data (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  zone_id UUID NOT NULL,
  temperature DECIMAL(5,2) NOT NULL,
  humidity DECIMAL(5,2) NOT NULL,
  soil_moisture DECIMAL(5,2) NOT NULL,
  timestamp TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);
```

**Queries**:
```sql
-- Get latest sensor data for a zone
SELECT * FROM sensor_data 
WHERE zone_id = 'uuid' 
ORDER BY timestamp DESC 
LIMIT 1;

-- Get sensor data for the last 24 hours
SELECT * FROM sensor_data 
WHERE zone_id = 'uuid' 
AND timestamp > NOW() - INTERVAL '24 hours'
ORDER BY timestamp;
```

### 2. devices

Manages ESP32 devices.

**Schema**:
```sql
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
```

### 3. watering_controls

Controls watering systems for each zone.

**Schema**:
```sql
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
```

### 4. commands

Stores commands sent to ESP32 devices.

**Schema**:
```sql
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
```

### 5. zones

Defines plant zones.

**Schema**:
```sql
CREATE TABLE zones (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  soil_type TEXT NOT NULL,
  sensor_id TEXT NOT NULL,
  moisture_level DECIMAL(5,2) NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);
```

### 6. alerts

Stores system alerts and notifications.

**Schema**:
```sql
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
```

### 7. api_keys

Manages API keys for ESP32 devices.

**Schema**:
```sql
CREATE TABLE api_keys (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  key TEXT UNIQUE NOT NULL,
  created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
  user_id UUID REFERENCES auth.users NOT NULL
);
```

## Real-time Subscriptions

### Sensor Data Updates
```javascript
const subscription = supabase
  .channel('sensor_data')
  .on('postgres_changes', 
    { event: 'INSERT', schema: 'public', table: 'sensor_data' },
    (payload) => {
      console.log('New sensor data:', payload.new);
    }
  )
  .subscribe();
```

### Command Updates
```javascript
const subscription = supabase
  .channel('commands')
  .on('postgres_changes', 
    { event: 'INSERT', schema: 'public', table: 'commands' },
    (payload) => {
      console.log('New command:', payload.new);
    }
  )
  .subscribe();
```

### Alert Updates
```javascript
const subscription = supabase
  .channel('alerts')
  .on('postgres_changes', 
    { event: 'INSERT', schema: 'public', table: 'alerts' },
    (payload) => {
      console.log('New alert:', payload.new);
    }
  )
  .subscribe();
```

## Error Codes

| Code | Description |
|------|-------------|
| 200 | Success |
| 201 | Created |
| 400 | Bad Request - Missing or invalid parameters |
| 401 | Unauthorized - Invalid API key or authentication |
| 404 | Not Found - Resource not found |
| 405 | Method Not Allowed |
| 500 | Internal Server Error |

## Rate Limits

- **ESP32 Data**: 1 request per 30 seconds per device
- **Commands**: 10 requests per minute per user
- **Device Management**: 5 requests per minute per user

## Security

### API Key Security
- API keys are stored securely in the database
- Keys are validated on every request
- Keys are associated with specific users
- Keys can be revoked at any time

### Row Level Security (RLS)
All tables have RLS policies that ensure users can only access their own data:
- Users can only view/modify their own sensor data
- Users can only control their own devices
- Users can only manage their own zones and alerts

### HTTPS
All communications use HTTPS for encryption.

## Testing

### Using curl

**Send sensor data**:
```bash
curl -X POST https://your-project.supabase.co/functions/v1/esp32-data \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_ANON_KEY" \
  -d '{
    "sensor_id": "ZONE_001",
    "temperature": 24.5,
    "humidity": 45.2,
    "soil_moisture": 35.8,
    "apiKey": "sk_plant_monitor_abc123"
  }'
```

**Send command**:
```bash
curl -X POST https://your-project.supabase.co/functions/v1/esp32-commands \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_ANON_KEY" \
  -d '{
    "device_id": "uuid",
    "command_type": "pump_on",
    "parameters": {"duration": 30}
  }'
```

### Using Postman

1. Import the collection
2. Set environment variables
3. Update request bodies with your data
4. Send requests to test functionality

## Support

For API support:
1. Check error messages in responses
2. Verify authentication credentials
3. Ensure proper request format
4. Check Supabase logs for detailed errors 