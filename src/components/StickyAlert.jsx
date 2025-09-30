// src/components/StickyAlert.jsx
import React, { useState, useEffect } from "react";
import { FiAlertTriangle, FiX, FiWifi, FiWifiOff } from "react-icons/fi";
import { useTheme } from '../contexts/ThemeContext';
import DataService from '../services/dataService';

const StickyAlert = ({ 
  message = "Sensor disconnected!", 
  type = "error", 
  dismissible = false, 
  onDismiss,
  autoHide = false,
  duration = 5000,
  showConnectivityStatus = false,
  deviceStatus = null // { online: number, offline: number, total: number }
}) => {
  const { theme } = useTheme();
  const isDark = theme === 'dark';
  const [isVisible, setIsVisible] = useState(true);
  const [isOnline, setIsOnline] = useState(navigator.onLine);
  const [lastDataUpdate, setLastDataUpdate] = useState(null);

  // Monitor connectivity status (optimized)
  useEffect(() => {
    if (showConnectivityStatus) {
      const handleOnline = () => setIsOnline(true);
      const handleOffline = () => setIsOnline(false);
      
      window.addEventListener('online', handleOnline);
      window.addEventListener('offline', handleOffline);
      
      // Check data freshness periodically with longer interval
      const checkDataFreshness = async () => {
        try {
          const latestData = await DataService.getLatestSensorData();
          if (latestData && latestData.timestamp) {
            setLastDataUpdate(new Date(latestData.timestamp));
          }
        } catch (error) {
          console.error('Error checking data freshness:', error);
        }
      };
      
      checkDataFreshness();
      const interval = setInterval(checkDataFreshness, 60000); // Check every 60 seconds instead of 30
      
      return () => {
        window.removeEventListener('online', handleOnline);
        window.removeEventListener('offline', handleOffline);
        clearInterval(interval);
      };
    }
  }, [showConnectivityStatus]);

  // Auto-hide functionality
  useEffect(() => {
    if (autoHide && duration > 0) {
      const timer = setTimeout(() => {
        handleDismiss();
      }, duration);
      
      return () => clearTimeout(timer);
    }
  }, [autoHide, duration]);

  const handleDismiss = () => {
    setIsVisible(false);
    if (onDismiss) {
      onDismiss();
    }
  };

  const getDataFreshnessStatus = () => {
    if (!lastDataUpdate) return null;
    
    const now = new Date();
    const diffMinutes = Math.floor((now - lastDataUpdate) / (1000 * 60));
    
    if (diffMinutes < 5) return { status: 'fresh', text: 'Live data' };
    if (diffMinutes < 15) return { status: 'recent', text: `${diffMinutes}m ago` };
    return { status: 'stale', text: `${diffMinutes}m ago` };
  };

  const getIcon = () => {
    if (showConnectivityStatus) {
      return isOnline ? 
        <FiWifi className="text-lg flex-shrink-0" /> : 
        <FiWifiOff className="text-lg flex-shrink-0" />;
    }
    return <FiAlertTriangle className="text-lg flex-shrink-0" />;
  };

  const getDisplayMessage = () => {
    if (showConnectivityStatus && !isOnline) {
      return "Internet connection lost. Real-time updates unavailable.";
    }
    
    // Show device connectivity status if provided
    if (deviceStatus && deviceStatus.total > 0) {
      const { online, offline, total } = deviceStatus;
      if (offline > 0) {
        return `${offline} of ${total} device${total > 1 ? 's' : ''} offline. Check your ESP32 connections.`;
      } else if (online === total) {
        return `All ${total} device${total > 1 ? 's' : ''} online and connected.`;
      }
    }
    
    return message;
  };

  const dataStatus = getDataFreshnessStatus();

  // Auto-determine alert type based on device status
  const getAlertType = () => {
    if (deviceStatus && deviceStatus.total > 0) {
      const { online, offline } = deviceStatus;
      if (offline > 0) return 'warning';
      if (online === deviceStatus.total) return 'info';
    }
    return type;
  };

  const alertType = getAlertType();

  if (!isVisible) return null;

  const colorMap = {
    error: isDark 
      ? "bg-red-900 bg-opacity-30 text-red-300 border-red-500" 
      : "bg-red-100 text-red-800 border-red-400",
    warning: isDark 
      ? "bg-yellow-900 bg-opacity-30 text-yellow-300 border-yellow-500" 
      : "bg-yellow-100 text-yellow-800 border-yellow-400",
    info: isDark 
      ? "bg-blue-900 bg-opacity-30 text-blue-300 border-blue-500" 
      : "bg-blue-100 text-blue-800 border-blue-400",
  };

  return (
    <div className={`mx-2 mb-4 p-3 border-l-4 ${colorMap[alertType]} rounded flex items-center gap-2 text-sm transition-all duration-300 transform ${
      isVisible ? 'translate-y-0 opacity-100' : '-translate-y-2 opacity-0'
    }`}>
      {getIcon()}
      <span className="flex-1">{getDisplayMessage()}</span>
      
      {showConnectivityStatus && dataStatus && (
        <div className={`px-2 py-1 rounded-full text-xs ${
          dataStatus.status === 'fresh' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-300' :
          dataStatus.status === 'recent' ? 'bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-300' :
          'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-300'
        }`}>
          {dataStatus.text}
        </div>
      )}
      
      {dismissible && (
        <button
          onClick={handleDismiss}
          className={`p-1 rounded-full transition-colors ${
            isDark ? 'hover:bg-white hover:bg-opacity-10' : 'hover:bg-black hover:bg-opacity-10'
          }`}
          aria-label="Dismiss alert"
        >
          <FiX className="text-sm" />
        </button>
      )}
    </div>
  );
};

export default StickyAlert;
