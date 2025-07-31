// src/pages/Dashboard.jsx
import React, { useState, useEffect } from 'react';
import { useTheme } from '../contexts/ThemeContext';
import StatsCard from '../components/StatsCard';
import WeatherPanel from '../components/WeatherPanel';
import AlertsPanel from '../components/AlertsPanel';
import TemperatureChart from '../components/charts/TemperatureChart';
import MoistureHumidityChart from '../components/charts/MoistureHumidityChart';
import PlantZonesPanel from '../components/PlantZonesPanel';
import PlantRecommendationsPanel from '../components/PlantRecommendationsPanel';
import DataService from '../services/dataService';

const Dashboard = () => {
  const { theme } = useTheme();
  const isDark = theme === 'dark';
  
  const [stats, setStats] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    loadDashboardData();
  }, []);

  const loadDashboardData = async () => {
    try {
      setLoading(true);
      setError(null);
      
      const dashboardStats = await DataService.getDashboardStats();
      setStats(dashboardStats);
    } catch (err) {
      console.error('Error loading dashboard data:', err);
      setError('Failed to load dashboard data. Please try again.');
    } finally {
      setLoading(false);
    }
  };

  const handleRefresh = () => {
    loadDashboardData();
  };

  if (loading) {
    return (
      <div className="space-y-6 max-w-full">
        <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between">
          <h1 className={`text-2xl sm:text-3xl font-bold ${isDark ? 'text-white' : 'text-gray-800'}`}>Garden Dashboard</h1>
          <p className={`text-sm sm:text-base mt-2 sm:mt-0 ${isDark ? 'text-gray-300' : 'text-gray-600'}`}>
            Loading your garden data...
          </p>
        </div>
        
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4 sm:gap-6">
          {[1, 2, 3, 4].map((i) => (
            <div key={i} className={`${isDark ? 'bg-gray-800' : 'bg-white'} p-6 rounded-lg shadow-sm border ${isDark ? 'border-gray-700' : 'border-gray-200'} animate-pulse`}>
              <div className="flex items-center justify-between">
                <div className="h-4 bg-gray-300 rounded w-24"></div>
                <div className="h-8 w-8 bg-gray-300 rounded"></div>
              </div>
              <div className="mt-4">
                <div className="h-8 bg-gray-300 rounded w-16"></div>
                <div className="h-4 bg-gray-300 rounded w-12 mt-2"></div>
              </div>
            </div>
          ))}
        </div>
      </div>
    );
  }

  if (error) {
    return (
      <div className="space-y-6 max-w-full">
        <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between">
          <h1 className={`text-2xl sm:text-3xl font-bold ${isDark ? 'text-white' : 'text-gray-800'}`}>Garden Dashboard</h1>
          <button 
            onClick={handleRefresh}
            className={`mt-4 sm:mt-0 px-4 py-2 rounded-lg transition-colors ${
              isDark 
                ? 'bg-gray-700 hover:bg-gray-600 text-white' 
                : 'bg-gray-200 hover:bg-gray-300 text-gray-800'
            }`}
          >
            Retry
          </button>
        </div>
        
        <div className={`${isDark ? 'bg-red-900/20 border-red-800' : 'bg-red-50 border-red-200'} border rounded-lg p-6`}>
          <div className="flex items-center">
            <svg className="h-6 w-6 text-red-500 mr-3" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 8v4m0 4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
            </svg>
            <div>
              <h3 className="text-lg font-medium text-red-800">Error Loading Data</h3>
              <p className="text-red-700 mt-1">{error}</p>
            </div>
          </div>
        </div>
      </div>
    );
  }

  return (
    <div className="space-y-6 max-w-full">
      <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between">
        <h1 className={`text-2xl sm:text-3xl font-bold ${isDark ? 'text-white' : 'text-gray-800'}`}>Garden Dashboard</h1>
        <div className="flex items-center space-x-4 mt-4 sm:mt-0">
          <p className={`text-sm sm:text-base ${isDark ? 'text-gray-300' : 'text-gray-600'}`}>
            Welcome back! Here's your garden overview.
          </p>
          <button 
            onClick={handleRefresh}
            className={`p-2 rounded-lg transition-colors ${
              isDark 
                ? 'bg-gray-700 hover:bg-gray-600 text-white' 
                : 'bg-gray-200 hover:bg-gray-300 text-gray-800'
            }`}
            title="Refresh data"
          >
            <svg className="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15" />
            </svg>
          </button>
        </div>
      </div>

      {/* Stats Cards */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4 sm:gap-6">
        {stats.length > 0 ? (
          stats.map((stat, index) => (
            <StatsCard
              key={index}
              title={stat.title}
              value={stat.value}
              change={stat.change}
              trend={stat.trend}
              icon={stat.icon}
            />
          ))
        ) : (
          <div className={`col-span-full ${isDark ? 'bg-gray-800' : 'bg-white'} p-6 rounded-lg shadow-sm border ${isDark ? 'border-gray-700' : 'border-gray-200'} text-center`}>
            <svg className={`h-12 w-12 mx-auto ${isDark ? 'text-gray-600' : 'text-gray-300'}`} fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z" />
            </svg>
            <p className={`mt-2 ${isDark ? 'text-gray-400' : 'text-gray-500'}`}>No sensor data available</p>
            <p className={`text-sm ${isDark ? 'text-gray-500' : 'text-gray-400'}`}>Connect your ESP32 devices to see real-time data</p>
          </div>
        )}
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
