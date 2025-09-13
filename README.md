# 🌿 FarmFlow Dashboard

A modern IoT garden monitoring dashboard for real-time plant monitoring and smart watering system control.

## Project Overview

FarmFlow Dashboard is a comprehensive solution for gardeners to monitor environmental conditions, automate watering, and analyze plant health data. The dashboard provides real-time visibility into garden conditions through ESP32 IoT devices and offers automated watering system management.

## Features

- **Real-time Sensor Monitoring**: Temperature, humidity, and soil moisture tracking
- **Plant Zone Management**: Customizable zones with different soil types
- **Automated Watering Scheduling**: Configurable schedules with manual override
- **Advanced Analytics**: Interactive charts and historical data analysis
- **ESP32 Device Integration**: Direct communication with IoT sensors
- **Responsive Design**: Works on desktop, tablet, and mobile devices
- **Dark/Light Theme**: User preference support with system detection

## Technology Stack

### Frontend
- React 18.2.0
- Vite 7.1.4
- Tailwind CSS 3.4.0
- Recharts 3.1.0
- React Router DOM 7.7.0
- React Icons 5.5.0

### Backend/Services
- Supabase 2.52.0 (Database, Authentication, Edge Functions)
- WeatherAPI for weather data

## Project Structure

```
src/
├── components/          # Reusable UI components
│   ├── charts/          # Data visualization components
│   ├── layout/          # Layout components (AuthenticatedLayout, etc.)
│   └── ui/              # Standardized UI components (Button, Card, etc.)
├── contexts/            # React context providers (Auth, Theme)
├── lib/                 # Supabase client configuration
├── pages/               # Feature-specific page components
├── services/            # Business logic and API services
└── index.css            # Global styles and Tailwind configuration

supabase/
├── functions/           # Edge functions for ESP32 communication
├── migrations/          # Database schema and RLS policies
└── README.md            # Supabase configuration documentation
```

## Setup Instructions

### Prerequisites
- Node.js 18+
- pnpm package manager
- Supabase account

### Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd Project_Dashboard
   ```

2. Install dependencies:
   ```bash
   pnpm install
   ```

3. Set up environment variables:
   Create a `.env` file in the root directory with the following variables:
   ```
   VITE_SUPABASE_URL=your_supabase_url
   VITE_SUPABASE_ANON_KEY=your_supabase_anon_key
   VITE_WEATHER_API_KEY=your_weatherapi_key
   ```

4. Set up Supabase:
   - Create a new Supabase project
   - Run the SQL schema from `supabase/schema.sql`
   - Apply RLS policies from `supabase/migrations/001-rls-policies.sql`
   - Deploy the Edge Functions from `supabase/functions/`

### Development

```bash
pnpm run dev
```

### Production Build

```bash
pnpm run build
```

### Preview Build

```bash
pnpm run preview
```

## ESP32 Integration

The dashboard integrates with ESP32 devices through Supabase Edge Functions:

1. **Device Registration**: Devices are registered in the dashboard and assigned API keys
2. **Data Transmission**: ESP32 devices send sensor data to Supabase using HTTP requests
3. **Command Handling**: The dashboard sends watering commands that ESP32 devices fetch periodically
4. **Real-time Updates**: Changes are reflected immediately in the dashboard through Supabase real-time subscriptions

### Security

- Row Level Security (RLS) policies ensure users can only access their own data
- API keys authenticate ESP32 devices
- All communication happens over HTTPS
- Database functions validate data ownership

## Database Schema

The project uses a comprehensive database schema with tables for:
- Users and profiles
- Plant zones
- ESP32 devices
- Sensor data
- Watering schedules and controls
- System alerts
- API keys

## Row Level Security (RLS)

All tables have RLS enabled with policies that ensure users can only access data they own. See `supabase/migrations/001-rls-policies.sql` for detailed policies.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For support, please open an issue on the GitHub repository or contact the maintainers.
