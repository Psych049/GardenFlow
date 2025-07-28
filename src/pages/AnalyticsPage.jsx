import React, { useState } from 'react';
import { BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, PieChart, Pie, Cell } from 'recharts';

const AnalyticsPage = () => {
  const [period, setPeriod] = useState('month');

  // Sample data for water usage by zone
  const waterUsageData = [
    { name: 'Vegetable Garden', usage: 125 },
    { name: 'Flower Bed', usage: 85 },
    { name: 'Herb Garden', usage: 40 },
    { name: 'Indoor Plants', usage: 30 }
  ];

  // Sample data for water savings
  const waterSavingsData = [
    { month: 'Jan', saved: 15, regular: 120 },
    { month: 'Feb', saved: 18, regular: 115 },
    { month: 'Mar', saved: 25, regular: 110 },
    { month: 'Apr', saved: 30, regular: 105 },
    { month: 'May', saved: 35, regular: 100 },
    { month: 'Jun', saved: 40, regular: 90 }
  ];

  // Sample data for moisture levels
  const moisturePieData = [
    { name: 'Optimal', value: 65, color: '#22c55e' },
    { name: 'Slightly Dry', value: 20, color: '#eab308' },
    { name: 'Too Dry', value: 15, color: '#ef4444' }
  ];

  // Sample data for plant health scores
  const healthScoreData = [
    { zone: 'Vegetable Garden', score: 87 },
    { zone: 'Flower Bed', score: 74 },
    { zone: 'Herb Garden', score: 92 },
    { zone: 'Indoor Plants', score: 80 }
  ];

  // Mock ML recommendation insights
  const mlInsights = [
    {
      id: 1,
      title: 'Optimal Watering Time',
      description: 'Based on temperature and humidity patterns, watering in early morning (5-7am) will maximize efficiency',
      icon: '⏰'
    },
    {
      id: 2,
      title: 'Reduce Watering Frequency',
      description: 'Your Herb Garden can maintain optimal moisture with 30% less frequent watering',
      icon: '💧'
    },
    {
      id: 3,
      title: 'Plant Recommendation',
      description: 'The soil composition in your Flower Bed is ideal for growing Lavender and Rosemary',
      icon: '🌿'
    }
  ];

  return (
    <div className="space-y-6">
      <h1 className="text-2xl font-semibold text-gray-800 dark:text-white">Analytics & Insights</h1>

      <div className="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-4 mb-6">
        <div className="flex justify-between items-center mb-4">
          <h2 className="text-lg font-medium text-gray-800 dark:text-white">Garden Performance</h2>
          <div className="inline-flex rounded-md shadow-sm" role="group">
            <button
              type="button"
              onClick={() => setPeriod('week')}
              className={`px-4 py-2 text-sm font-medium rounded-l-md ${
                period === 'week' 
                  ? 'bg-green-600 text-white' 
                  : 'bg-white dark:bg-gray-700 text-gray-700 dark:text-gray-200 border border-gray-300 dark:border-gray-600 hover:bg-gray-50 dark:hover:bg-gray-600'
              }`}
            >
              Week
            </button>
            <button
              type="button"
              onClick={() => setPeriod('month')}
              className={`px-4 py-2 text-sm font-medium rounded-r-md ${
                period === 'month' 
                  ? 'bg-green-600 text-white' 
                  : 'bg-white dark:bg-gray-700 text-gray-700 dark:text-gray-200 border border-gray-300 dark:border-gray-600 hover:bg-gray-50 dark:hover:bg-gray-600'
              }`}
            >
              Month
            </button>
          </div>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          <div className="p-4 border border-gray-200 dark:border-gray-700 rounded-lg bg-white dark:bg-gray-700">
            <div className="text-sm font-medium text-gray-500 dark:text-gray-400">Total Water Usage</div>
            <div className="mt-1 flex items-baseline">
              <div className="text-2xl font-semibold text-gray-900 dark:text-white">280</div>
              <div className="ml-2 text-sm text-green-600 dark:text-green-400">-15% vs avg</div>
            </div>
          </div>
          
          <div className="p-4 border border-gray-200 dark:border-gray-700 rounded-lg bg-white dark:bg-gray-700">
            <div className="text-sm font-medium text-gray-500 dark:text-gray-400">Water Savings</div>
            <div className="mt-1 flex items-baseline">
              <div className="text-2xl font-semibold text-gray-900 dark:text-white">45L</div>
              <div className="ml-2 text-sm text-green-600 dark:text-green-400">+8% vs last month</div>
            </div>
          </div>
          
          <div className="p-4 border border-gray-200 dark:border-gray-700 rounded-lg bg-white dark:bg-gray-700">
            <div className="text-sm font-medium text-gray-500 dark:text-gray-400">Avg. Soil Moisture</div>
            <div className="mt-1 flex items-baseline">
              <div className="text-2xl font-semibold text-gray-900 dark:text-white">42%</div>
              <div className="ml-2 text-sm text-green-600 dark:text-green-400">Optimal</div>
            </div>
          </div>
          
          <div className="p-4 border border-gray-200 dark:border-gray-700 rounded-lg bg-white dark:bg-gray-700">
            <div className="text-sm font-medium text-gray-500 dark:text-gray-400">Plant Health Score</div>
            <div className="mt-1 flex items-baseline">
              <div className="text-2xl font-semibold text-gray-900 dark:text-white">83/100</div>
              <div className="ml-2 text-sm text-green-600 dark:text-green-400">+5 pts</div>
            </div>
          </div>
        </div>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Water Usage By Zone Chart */}
        <div className="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
          <h2 className="text-lg font-medium text-gray-800 dark:text-white mb-4">Water Usage By Zone</h2>
          <div className="h-64">
            <ResponsiveContainer width="100%" height="100%">
              <BarChart data={waterUsageData}>
                <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                <XAxis dataKey="name" stroke="#9CA3AF" />
                <YAxis label={{ value: 'Liters', angle: -90, position: 'insideLeft' }} stroke="#9CA3AF" />
                <Tooltip 
                  contentStyle={{ 
                    backgroundColor: '#1F2937', 
                    border: '1px solid #374151',
                    borderRadius: '8px',
                    color: '#F9FAFB'
                  }}
                />
                <Bar dataKey="usage" fill="#22c55e" />
              </BarChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Water Savings Chart */}
        <div className="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
          <h2 className="text-lg font-medium text-gray-800 dark:text-white mb-4">Water Savings Trend</h2>
          <div className="h-64">
            <ResponsiveContainer width="100%" height="100%">
              <BarChart data={waterSavingsData}>
                <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                <XAxis dataKey="month" stroke="#9CA3AF" />
                <YAxis label={{ value: 'Liters', angle: -90, position: 'insideLeft' }} stroke="#9CA3AF" />
                <Tooltip 
                  contentStyle={{ 
                    backgroundColor: '#1F2937', 
                    border: '1px solid #374151',
                    borderRadius: '8px',
                    color: '#F9FAFB'
                  }}
                />
                <Legend />
                <Bar dataKey="saved" fill="#22c55e" name="Water Saved" />
                <Bar dataKey="regular" fill="#94a3b8" name="Regular Usage" />
              </BarChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Moisture Level Distribution */}
        <div className="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
          <h2 className="text-lg font-medium text-gray-800 dark:text-white mb-4">Moisture Level Distribution</h2>
          <div className="h-64">
            <ResponsiveContainer width="100%" height="100%">
              <PieChart>
                <Pie
                  data={moisturePieData}
                  cx="50%"
                  cy="50%"
                  innerRadius={60}
                  outerRadius={90}
                  dataKey="value"
                  label={({ name, percent }) => `${name}: ${(percent * 100).toFixed(0)}%`}
                >
                  {moisturePieData.map((entry, index) => (
                    <Cell key={`cell-${index}`} fill={entry.color} />
                  ))}
                </Pie>
                <Tooltip 
                  contentStyle={{ 
                    backgroundColor: '#1F2937', 
                    border: '1px solid #374151',
                    borderRadius: '8px',
                    color: '#F9FAFB'
                  }}
                />
              </PieChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Plant Health Scores */}
        <div className="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
          <h2 className="text-lg font-medium text-gray-800 dark:text-white mb-4">Plant Health Scores</h2>
          <div className="h-64">
            <ResponsiveContainer width="100%" height="100%">
              <BarChart data={healthScoreData}>
                <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                <XAxis dataKey="zone" stroke="#9CA3AF" />
                <YAxis label={{ value: 'Score', angle: -90, position: 'insideLeft' }} domain={[0, 100]} stroke="#9CA3AF" />
                <Tooltip 
                  contentStyle={{ 
                    backgroundColor: '#1F2937', 
                    border: '1px solid #374151',
                    borderRadius: '8px',
                    color: '#F9FAFB'
                  }}
                />
                <Bar dataKey="score" fill="#3b82f6">
                  {healthScoreData.map((entry, index) => (
                    <Cell
                      key={`cell-${index}`}
                      fill={
                        entry.score >= 90 ? '#22c55e' : 
                        entry.score >= 75 ? '#3b82f6' : 
                        entry.score >= 60 ? '#eab308' : '#ef4444'
                      }
                    />
                  ))}
                </Bar>
              </BarChart>
            </ResponsiveContainer>
          </div>
        </div>
      </div>

      {/* ML Insights */}
      <div className="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h2 className="text-lg font-medium text-gray-800 dark:text-white mb-4">Smart Gardening Insights</h2>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
          {mlInsights.map(insight => (
            <div key={insight.id} className="border border-gray-200 dark:border-gray-700 rounded-lg p-4 hover:shadow-md transition-shadow bg-white dark:bg-gray-700">
              <div className="text-3xl mb-3">{insight.icon}</div>
              <h3 className="font-medium text-lg text-gray-800 dark:text-white mb-2">{insight.title}</h3>
              <p className="text-gray-600 dark:text-gray-300 text-sm">{insight.description}</p>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default AnalyticsPage;