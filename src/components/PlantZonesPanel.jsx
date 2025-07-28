import React from 'react';
import { useTheme } from '../contexts/ThemeContext';
import { plantZones } from '../data/mockData';

const PlantZonesPanel = () => {
  const { theme } = useTheme();
  const isDark = theme === 'dark';

  const getMoistureColor = (level) => {
    if (level < 30) return 'text-red-500';
    if (level < 40) return 'text-yellow-500';
    return 'text-green-500';
  };

  const getMoistureLabel = (level) => {
    if (level < 30) return 'Low';
    if (level < 40) return 'Medium';
    return 'Good';
  };

  return (
    <div className={`${isDark ? 'bg-gray-800' : 'bg-white'} p-6 rounded-lg shadow-sm border ${isDark ? 'border-gray-700' : 'border-gray-200'}`}>
      <h2 className={`text-lg font-medium mb-4 ${isDark ? 'text-white' : 'text-gray-800'}`}>Plant Zones</h2>
      
      <div className="overflow-x-auto">
        <table className="min-w-full">
          <thead>
            <tr>
              <th className={`px-3 py-2 text-left text-xs font-medium uppercase tracking-wider ${isDark ? 'text-gray-400' : 'text-gray-500'}`}>Zone</th>
              <th className={`px-3 py-2 text-left text-xs font-medium uppercase tracking-wider ${isDark ? 'text-gray-400' : 'text-gray-500'}`}>Soil Type</th>
              <th className={`px-3 py-2 text-left text-xs font-medium uppercase tracking-wider ${isDark ? 'text-gray-400' : 'text-gray-500'}`}>Moisture Level</th>
            </tr>
          </thead>
          <tbody className={`divide-y ${isDark ? 'divide-gray-700' : 'divide-gray-200'}`}>
            {plantZones.map((zone, index) => (
              <tr key={index}>
                <td className={`px-3 py-3 text-sm font-medium ${isDark ? 'text-white' : 'text-gray-800'}`}>{zone.name}</td>
                <td className={`px-3 py-3 text-sm ${isDark ? 'text-gray-300' : 'text-gray-600'}`}>{zone.soilType}</td>
                <td className="px-3 py-3">
                  <div className="flex items-center">
                    <div className={`w-24 h-2 rounded mr-2 ${isDark ? 'bg-gray-600' : 'bg-gray-200'}`}>
                      <div 
                        className={`h-full rounded ${zone.moistureLevel < 30 ? 'bg-red-500' : zone.moistureLevel < 40 ? 'bg-yellow-500' : 'bg-green-500'}`}
                        style={{ width: `${zone.moistureLevel}%` }}
                      ></div>
                    </div>
                    <span className={`text-sm font-medium ${getMoistureColor(zone.moistureLevel)}`}>
                      {zone.moistureLevel}% ({getMoistureLabel(zone.moistureLevel)})
                    </span>
                  </div>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
};

export default PlantZonesPanel;