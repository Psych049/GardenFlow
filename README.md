# 🌱 Smart Garden Dashboard

A modern, responsive web application for monitoring and managing your smart garden ecosystem. Built with React, Supabase, and Tailwind CSS, this dashboard provides real-time sensor data visualization, plant management, automated watering schedules, and intelligent recommendations.

## ✨ Features

### 📊 **Real-time Monitoring**
- Live sensor data from temperature, humidity, and soil moisture sensors
- Interactive charts and graphs using Recharts
- Weather integration for local conditions
- Real-time alerts and notifications

### 🌿 **Plant Management**
- Zone-based plant organization
- Soil type analysis and recommendations
- Plant health monitoring
- Customizable plant zones with sensor mapping

### 💧 **Smart Watering System**
- Automated watering schedules
- Zone-specific watering plans
- Moisture-based trigger systems
- Manual override capabilities

### 📈 **Analytics & Insights**
- Historical data analysis
- Performance trends
- Predictive maintenance alerts
- Garden health scoring

### 🔐 **User Management**
- Secure authentication with Supabase Auth
- User-specific data isolation
- API key management for sensor integration
- Role-based access control

### 🎨 **Modern UI/UX**
- Dark/Light theme toggle
- Responsive design for all devices
- Intuitive navigation
- Beautiful data visualizations

## 🛠️ Tech Stack

- **Frontend**: React 18, Vite, React Router DOM
- **Styling**: Tailwind CSS, PostCSS
- **Charts**: Recharts
- **Backend**: Supabase (PostgreSQL, Auth, Real-time)
- **Icons**: React Icons
- **Date Handling**: date-fns
- **HTTP Client**: Axios

## 🚀 Getting Started

### Prerequisites

- Node.js (v16 or higher)
- pnpm (recommended) or npm
- Supabase account and project

### Installation

1. **Clone the repository**
   ```bash
   git clone <your-repo-url>
   cd M_49
   ```

2. **Install dependencies**
   ```bash
   pnpm install
   ```

3. **Set up Supabase**
   - Create a new Supabase project
   - Run the schema from `supabase/schema.sql` in your Supabase SQL editor
   - Copy your Supabase URL and anon key

4. **Configure environment variables**
   Create a `.env` file in the root directory:
   ```env
   VITE_SUPABASE_URL=your_supabase_url
   VITE_SUPABASE_ANON_KEY=your_supabase_anon_key
   ```

5. **Start the development server**
   ```bash
   pnpm run dev
   ```

6. **Open your browser**
   Navigate to `http://localhost:5173`

## 📁 Project Structure

```
M_49/
├── src/
│   ├── components/          # Reusable UI components
│   │   ├── charts/         # Data visualization components
│   │   ├── layout/         # Layout components
│   │   └── ...
│   ├── contexts/           # React contexts (Auth, Theme)
│   ├── data/              # Mock data and utilities
│   ├── lib/               # External library configurations
│   ├── pages/             # Page components
│   └── ...
├── supabase/
│   ├── functions/         # Edge functions
│   └── schema.sql         # Database schema
└── ...
```

## 🔧 Available Scripts

- `pnpm run dev` - Start development server
- `pnpm run build` - Build for production
- `pnpm run preview` - Preview production build

## 🌐 Pages & Features

### Dashboard (`/dashboard`)
- Overview of garden health
- Real-time sensor data
- Weather information
- Quick alerts and notifications

### Plants (`/plants`)
- Plant zone management
- Soil type analysis
- Plant recommendations
- Health monitoring

### Watering Schedule (`/schedule`)
- Automated watering plans
- Zone-specific schedules
- Manual override controls
- Schedule history

### Sensors (`/sensors`)
- Sensor configuration
- Data visualization
- Calibration tools
- Sensor health status

### Analytics (`/analytics`)
- Historical data analysis
- Performance metrics
- Trend analysis
- Export capabilities

### System (`/system`)
- System status
- Configuration settings
- Maintenance tools
- System logs

### Settings (`/settings`)
- User preferences
- Theme customization
- API key management
- Account settings

## 🔌 API Integration

The dashboard integrates with ESP32 sensors through Supabase Edge Functions:

- **ESP32 Data Function**: Receives sensor data from ESP32 devices
- **Simulate Sensor Data**: Generates mock data for testing

### Sensor Data Format
```json
{
  "zone_id": "uuid",
  "temperature": 25.5,
  "humidity": 65.2,
  "soil_moisture": 45.8,
  "timestamp": "2024-01-01T12:00:00Z"
}
```

## 🎨 Customization

### Themes
The application supports both light and dark themes. Users can toggle between themes using the theme toggle button in the header.

### Styling
The project uses Tailwind CSS for styling. Custom styles can be added in `src/index.css` or by extending the Tailwind configuration in `tailwind.config.js`.

## 🔒 Security

- Row Level Security (RLS) enabled on all tables
- User-specific data isolation
- Secure API key management
- Authentication required for all protected routes

## 📊 Database Schema

The application uses the following main tables:
- `sensor_data` - Stores sensor readings
- `zones` - Garden zones configuration
- `alerts` - System alerts and notifications
- `soil_types` - Soil type information
- `api_keys` - API key management

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

**Built with ❤️ for smart gardening enthusiasts** 
