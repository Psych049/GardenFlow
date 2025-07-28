// src/pages/Dashboard.jsx
import React from 'react';
import { useTheme } from '../contexts/ThemeContext';
import StatsCard from '../components/StatsCard';
import WeatherPanel from '../components/WeatherPanel';
import AlertsPanel from '../components/AlertsPanel';
import TemperatureChart from '../components/charts/TemperatureChart';
import MoistureHumidityChart from '../components/charts/MoistureHumidityChart';
import PlantZonesPanel from '../components/PlantZonesPanel';
import PlantRecommendationsPanel from '../components/PlantRecommendationsPanel';
import { mockStats } from '../data/mockData';

const Dashboard = () => {
  const { theme } = useTheme();
  const isDark = theme === 'dark';

  return (
    <div className="space-y-6 max-w-full">
      <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between">
        <h1 className={`text-2xl sm:text-3xl font-bold ${isDark ? 'text-white' : 'text-gray-800'}`}>Garden Dashboard</h1>
        <p className={`text-sm sm:text-base mt-2 sm:mt-0 ${isDark ? 'text-gray-300' : 'text-gray-600'}`}>
          Welcome back! Here's your garden overview.
        </p>
      </div>

      {/* Stats Cards */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4 sm:gap-6">
        {mockStats.map((stat, index) => (
          <StatsCard
            key={index}
            title={stat.title}
            value={stat.value}
            change={stat.change}
            trend={stat.trend}
            icon={stat.icon}
          />
        ))}
      </div>

      {/* Weather Panel */}
      <WeatherPanel />

      {/* Alerts */}
      <AlertsPanel />

      {/* Charts Section */}
      <div className="grid grid-cols-1 xl:grid-cols-2 gap-4 sm:gap-6">
        <div className={`${isDark ? 'bg-gray-800' : 'bg-white'} p-4 sm:p-6 rounded-lg shadow-sm border ${isDark ? 'border-gray-700' : 'border-gray-200'}`}>
          <h2 className={`text-lg font-medium mb-4 ${isDark ? 'text-white' : 'text-gray-800'}`}>🌡 Temperature Trends (24h)</h2>
          <div className="h-64 sm:h-80">
            <TemperatureChart />
          </div>
        </div>
        <div className={`${isDark ? 'bg-gray-800' : 'bg-white'} p-4 sm:p-6 rounded-lg shadow-sm border ${isDark ? 'border-gray-700' : 'border-gray-200'}`}>
          <h2 className={`text-lg font-medium mb-4 ${isDark ? 'text-white' : 'text-gray-800'}`}>💧 Moisture & Humidity Trends (24h)</h2>
          <div className="h-64 sm:h-80">
            <MoistureHumidityChart />
          </div>
        </div>
      </div>

      {/* Plant Zones & Recommendations */}
      <div className="grid grid-cols-1 xl:grid-cols-2 gap-4 sm:gap-6">
        <PlantZonesPanel />
        <PlantRecommendationsPanel />
      </div>
    </div>
  );
};

export default Dashboard;
