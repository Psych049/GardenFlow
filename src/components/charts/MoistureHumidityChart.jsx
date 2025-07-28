import React from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { moistureHumidityData } from '../../data/mockData';

const MoistureHumidityChart = () => {
  return (
    <div className="h-72">
      <ResponsiveContainer width="100%" height="100%">
        <LineChart
          data={moistureHumidityData}
          margin={{
            top: 5,
            right: 30,
            left: 20,
            bottom: 5,
          }}
        >
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="time" />
          <YAxis />
          <Tooltip />
          <Legend />
          <Line 
            type="monotone" 
            dataKey="moisture" 
            stroke="#3b82f6" 
            strokeWidth={2}
            activeDot={{ r: 8 }} 
            name="Soil Moisture (%)"
          />
          <Line 
            type="monotone" 
            dataKey="humidity" 
            stroke="#a855f7" 
            strokeWidth={2} 
            name="Humidity (%)"
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};

export default MoistureHumidityChart;