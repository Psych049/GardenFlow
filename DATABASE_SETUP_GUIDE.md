# Database Setup Guide for Smart Garden Dashboard

## 🎯 **Overview**
This guide will help you recreate all database tables required by the Smart Garden Dashboard to ensure everything is properly connected without errors.

## 📋 **Prerequisites**
- Supabase project with admin access
- SQL Editor access in Supabase dashboard

## 🚀 **Step-by-Step Setup**

### **Step 1: Run the Complete Database Setup**
1. Go to your Supabase project dashboard
2. Navigate to **SQL Editor**
3. Copy and paste the entire contents of `raw_data/COMPLETE_DATABASE_SETUP.sql`
4. Click **Run** to execute the script

### **Step 2: Verify the Setup**
1. In the same SQL Editor, run the contents of `raw_data/VERIFY_DATABASE_SETUP.sql`
2. Check that all tables, functions, and policies are created correctly

### **Step 3: Test the Application**
1. Refresh your application (hard refresh: Ctrl+F5)
2. Go to the System page
3. Use the API Key Debugger to test functionality
4. Try creating a device and generating API keys

## 📊 **Database Schema Overview**

### **Core Tables:**
- **`zones`** - Garden zones with soil types and moisture thresholds
- **`devices`** - ESP32 devices with status and firmware info
- **`sensor_data`** - Temperature, humidity, and soil moisture readings
- **`watering_controls`** - Automatic watering system controls
- **`watering_schedules`** - Scheduled watering times
- **`commands`** - Device commands for real-time control
- **`api_keys`** - API keys for device authentication
- **`alerts`** - System alerts and notifications
- **`soil_types`** - Soil type information and recommendations
- **`audit_logs`** - Audit trail for system changes

### **Key Features:**
- ✅ **Row Level Security (RLS)** - Users can only access their own data
- ✅ **Foreign Key Constraints** - Ensures data integrity
- ✅ **Triggers** - Automatic alerts and command processing
- ✅ **Functions** - API key generation and device health updates
- ✅ **Realtime** - Live updates for sensor data and device status

## 🔧 **Functions Created:**
- `create_api_key()` - Generate API keys for devices
- `process_sensor_data()` - Process sensor readings and create alerts
- `handle_watering_command()` - Handle automatic watering commands
- `process_pending_commands()` - Retry failed commands
- `update_device_health()` - Update device status and health
- `audit_table_changes()` - Log all changes for audit trail

## 🛡️ **Security Features:**
- **RLS Policies** - Users can only access their own data
- **API Key Authentication** - Secure device communication
- **Audit Logging** - Track all system changes
- **Input Validation** - Prevent invalid data entry

## 🔍 **Troubleshooting**

### **If Tables Don't Create:**
1. Check that you have admin privileges
2. Ensure no existing tables with the same names
3. Run the setup script again

### **If RLS Policies Fail:**
1. Check that RLS is enabled on all tables
2. Verify user authentication is working
3. Check policy syntax

### **If Functions Don't Work:**
1. Verify function permissions are granted
2. Check function syntax and parameters
3. Test with simple queries first

### **If API Keys Don't Generate:**
1. Use the API Key Debugger component
2. Check the `api_keys` table structure
3. Verify the `create_api_key` function exists

## 📈 **Expected Results**

After running the setup:
- ✅ 10 tables created with proper relationships
- ✅ 6 functions created for system operations
- ✅ 4 triggers for automatic processing
- ✅ 25+ RLS policies for security
- ✅ Default soil types data inserted
- ✅ Realtime enabled for live updates

## 🔄 **Testing Checklist**

### **Database Level:**
- [ ] All tables exist and have correct structure
- [ ] Foreign key constraints are working
- [ ] RLS policies are active
- [ ] Functions can be executed
- [ ] Triggers are firing correctly

### **Application Level:**
- [ ] User authentication works
- [ ] Device creation works
- [ ] API key generation works
- [ ] Sensor data can be inserted
- [ ] Alerts are generated automatically
- [ ] Realtime updates work

## 📁 **Files Created:**
- `raw_data/COMPLETE_DATABASE_SETUP.sql` - Main setup script
- `raw_data/VERIFY_DATABASE_SETUP.sql` - Verification script
- `DATABASE_SETUP_GUIDE.md` - This guide

## 🆘 **Support**

If you encounter issues:
1. Check the verification script results
2. Use the API Key Debugger in the application
3. Review the error messages in the browser console
4. Check the Supabase logs for detailed errors

The database should now be fully functional and ready for the Smart Garden Dashboard application. 