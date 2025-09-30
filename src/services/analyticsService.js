import { supabase, handleSupabaseError } from '../lib/supabase';

// Analytics Service for advanced data analysis and insights with real-time capabilities
export class AnalyticsService {
  
  // Get analytics data for a specific time period
  static async getAnalyticsData(period = 'month') {
    try {
      const { data: { user } } = await supabase.auth.getUser();
      if (!user) throw new Error('User not authenticated');

      const endDate = new Date();
      const startDate = this.getStartDate(period);
      
      // Get sensor data for the period
      const { data: sensorData, error: sensorError } = await supabase
        .from('sensor_data')
        .select('*')
        .eq('user_id', user.id)
        .gte('timestamp', startDate.toISOString())
        .lte('timestamp', endDate.toISOString())
        .order('timestamp', { ascending: true });

      if (sensorError) {
        const handledError = handleSupabaseError(sensorError);
        console.error('Error fetching sensor data:', handledError);
        throw handledError;
      }

      // Get zones data
      const { data: zones, error: zonesError } = await supabase
        .from('zones')
        .select('*')
        .eq('user_id', user.id);

      if (zonesError) {
        const handledError = handleSupabaseError(zonesError);
        console.error('Error fetching zones:', handledError);
        throw handledError;
      }

      // Get watering controls data
      const { data: wateringControls, error: wateringError } = await supabase
        .from('watering_controls')
        .select('*')
        .eq('user_id', user.id);

      if (wateringError) {
        const handledError = handleSupabaseError(wateringError);
        console.error('Error fetching watering controls:', handledError);
        throw handledError;
      }

      // Get commands data for water usage analysis
      const { data: commands, error: commandsError } = await supabase
        .from('commands')
        .select('*')
        .eq('user_id', user.id)
        .gte('created_at', startDate.toISOString())
        .lte('created_at', endDate.toISOString());

      if (commandsError) {
        const handledError = handleSupabaseError(commandsError);
        console.error('Error fetching commands:', handledError);
        throw handledError;
      }

      return {
        sensorData: sensorData || [],
        zones: zones || [],
        wateringControls: wateringControls || [],
        commands: commands || [],
        period,
        startDate,
        endDate
      };
    } catch (error) {
      console.error('Error fetching analytics data:', error);
      throw error;
    }
  }

  // Calculate water usage by zone
  static calculateWaterUsageByZone(commands, zones, period) {
    const waterUsage = {};
    
    // Initialize zones with safe defaults
    (zones || []).forEach(zone => {
      waterUsage[zone.name] = {
        name: zone.name,
        usage: 0,
        sessions: 0,
        avgDuration: 0
      };
    });

    // If no zones exist, return empty array
    if (!zones || zones.length === 0) {
      return [];
    }

    // Calculate from pump commands
    const pumpCommands = (commands || []).filter(cmd => cmd.command_type === 'pump_on');
    
    pumpCommands.forEach(cmd => {
      const duration = cmd.parameters?.duration || 30; // seconds
      const waterPerSecond = 0.5; // liters per second (adjust based on your pump)
      const waterUsed = (duration * waterPerSecond);
      
      // Find zone by device (simplified - you might need to enhance this mapping)
      const zone = zones.find(z => z.id === cmd.device_id);
      if (zone && waterUsage[zone.name]) {
        waterUsage[zone.name].usage += waterUsed;
        waterUsage[zone.name].sessions += 1;
        const currentSessions = waterUsage[zone.name].sessions;
        const currentAvg = waterUsage[zone.name].avgDuration;
        waterUsage[zone.name].avgDuration = 
          ((currentAvg * (currentSessions - 1)) + duration) / currentSessions;
      }
    });

    return Object.values(waterUsage);
  }

  // Calculate water savings
  static calculateWaterSavings(commands, period) {
    const monthlyData = [];
    const months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
    
    // Calculate for last 6 months
    for (let i = 5; i >= 0; i--) {
      const date = new Date();
      date.setMonth(date.getMonth() - i);
      const monthStart = new Date(date.getFullYear(), date.getMonth(), 1);
      const monthEnd = new Date(date.getFullYear(), date.getMonth() + 1, 0);
      
      const monthCommands = commands.filter(cmd => {
        const cmdDate = new Date(cmd.created_at);
        return cmdDate >= monthStart && cmdDate <= monthEnd;
      });

      const pumpCommands = monthCommands.filter(cmd => cmd.command_type === 'pump_on');
      const totalWaterUsed = pumpCommands.reduce((sum, cmd) => {
        const duration = cmd.parameters?.duration || 30;
        return sum + (duration * 0.5); // 0.5L per second
      }, 0);

      // Estimate regular usage (without smart system)
      const estimatedRegularUsage = totalWaterUsed * 1.3; // 30% more without smart system
      const waterSaved = estimatedRegularUsage - totalWaterUsed;

      monthlyData.push({
        month: months[date.getMonth()],
        saved: Math.round(waterSaved * 10) / 10,
        regular: Math.round(estimatedRegularUsage * 10) / 10
      });
    }

    return monthlyData;
  }

  // Calculate moisture level distribution
  static calculateMoistureDistribution(sensorData) {
    // Handle empty sensor data
    if (!sensorData || sensorData.length === 0) {
      return [];
    }
    
    const moistureLevels = sensorData.map(data => data.soil_moisture || 0);
    
    let optimal = 0;
    let slightlyDry = 0;
    let tooDry = 0;
    let tooWet = 0;

    moistureLevels.forEach(level => {
      if (level >= 40 && level <= 60) optimal++;
      else if (level >= 30 && level < 40) slightlyDry++;
      else if (level < 30) tooDry++;
      else if (level > 60) tooWet++;
    });

    const total = moistureLevels.length;
    
    return [
      { name: 'Optimal', value: total > 0 ? Math.round((optimal / total) * 100) : 0, color: '#22c55e' },
      { name: 'Slightly Dry', value: total > 0 ? Math.round((slightlyDry / total) * 100) : 0, color: '#eab308' },
      { name: 'Too Dry', value: total > 0 ? Math.round((tooDry / total) * 100) : 0, color: '#ef4444' },
      { name: 'Too Wet', value: total > 0 ? Math.round((tooWet / total) * 100) : 0, color: '#3b82f6' }
    ].filter(item => item.value > 0);
  }

  // Calculate plant health scores
  static calculatePlantHealthScores(sensorData, zones) {
    const healthScores = [];
    
    // Handle empty zones
    if (!zones || zones.length === 0) {
      return [];
    }
    
    zones.forEach(zone => {
      const zoneData = (sensorData || []).filter(data => data.zone_id === zone.id);
      
      if (zoneData.length === 0) {
        healthScores.push({
          zone: zone.name,
          score: 0,
          dataPoints: 0
        });
        return;
      }

      // Calculate health score based on multiple factors
      const avgMoisture = zoneData.reduce((sum, data) => sum + (data.soil_moisture || 0), 0) / zoneData.length;
      const avgTemperature = zoneData.reduce((sum, data) => sum + (data.temperature || 0), 0) / zoneData.length;
      const avgHumidity = zoneData.reduce((sum, data) => sum + (data.humidity || 0), 0) / zoneData.length;
      
      // Moisture score (0-40 points)
      let moistureScore = 0;
      if (avgMoisture >= 40 && avgMoisture <= 60) moistureScore = 40;
      else if (avgMoisture >= 30 && avgMoisture < 40) moistureScore = 30;
      else if (avgMoisture >= 25 && avgMoisture < 30) moistureScore = 20;
      else if (avgMoisture < 25) moistureScore = 10;
      else moistureScore = 25; // Too wet

      // Temperature score (0-30 points)
      let tempScore = 0;
      if (avgTemperature >= 18 && avgTemperature <= 26) tempScore = 30;
      else if (avgTemperature >= 15 && avgTemperature < 18) tempScore = 25;
      else if (avgTemperature >= 26 && avgTemperature <= 30) tempScore = 20;
      else tempScore = 15;

      // Humidity score (0-30 points)
      let humidityScore = 0;
      if (avgHumidity >= 40 && avgHumidity <= 70) humidityScore = 30;
      else if (avgHumidity >= 30 && avgHumidity < 40) humidityScore = 25;
      else if (avgHumidity >= 70 && avgHumidity <= 80) humidityScore = 20;
      else humidityScore = 15;

      const totalScore = moistureScore + tempScore + humidityScore;
      
      healthScores.push({
        zone: zone.name,
        score: Math.round(totalScore),
        dataPoints: zoneData.length,
        avgMoisture: Math.round(avgMoisture * 10) / 10,
        avgTemperature: Math.round(avgTemperature * 10) / 10,
        avgHumidity: Math.round(avgHumidity * 10) / 10
      });
    });

    return healthScores;
  }

  // Generate ML insights
  static generateInsights(sensorData, zones, commands) {
    const insights = [];
    
    // Handle empty data gracefully
    if (!sensorData || sensorData.length === 0) {
      insights.push({
        id: 1,
        title: 'Start Monitoring Your Garden',
        description: 'Connect your ESP32 sensors to begin receiving real-time data and smart insights about your garden.',
        icon: '🌱',
        type: 'recommendation'
      });
      
      if (!zones || zones.length === 0) {
        insights.push({
          id: 2,
          title: 'Create Your First Zone',
          description: 'Set up garden zones to organize your plants and optimize watering schedules.',
          icon: '🏡',
          type: 'recommendation'
        });
      }
      
      return insights;
    }

    // Analyze watering patterns
    const pumpCommands = (commands || []).filter(cmd => cmd.command_type === 'pump_on');
    if (pumpCommands.length > 0) {
      const wateringTimes = pumpCommands.map(cmd => new Date(cmd.created_at).getHours());
      const morningWatering = wateringTimes.filter(hour => hour >= 5 && hour <= 9).length;
      const afternoonWatering = wateringTimes.filter(hour => hour >= 12 && hour <= 16).length;
      const eveningWatering = wateringTimes.filter(hour => hour >= 18 && hour <= 22).length;

      if (morningWatering > afternoonWatering && morningWatering > eveningWatering) {
        insights.push({
          id: 1,
          title: 'Optimal Watering Time',
          description: 'Your morning watering schedule (5-9 AM) is optimal for plant health and water efficiency',
          icon: '⏰',
          type: 'positive'
        });
      } else {
        insights.push({
          id: 1,
          title: 'Watering Time Optimization',
          description: 'Consider watering in early morning (5-7 AM) to reduce evaporation and maximize efficiency',
          icon: '⏰',
          type: 'recommendation'
        });
      }
    } else {
      insights.push({
        id: 1,
        title: 'Set Up Automatic Watering',
        description: 'Configure automatic watering schedules to ensure your plants get optimal care.',
        icon: '💧',
        type: 'recommendation'
      });
    }

    // Analyze moisture patterns
    const avgMoisture = sensorData.reduce((sum, data) => sum + (data.soil_moisture || 0), 0) / sensorData.length;
    if (avgMoisture < 30) {
      insights.push({
        id: 2,
        title: 'Low Moisture Alert',
        description: 'Average soil moisture is below optimal levels. Consider adjusting watering frequency or duration',
        icon: '💧',
        type: 'warning'
      });
    } else if (avgMoisture > 60) {
      insights.push({
        id: 2,
        title: 'Overwatering Detected',
        description: 'Soil moisture levels are high. Consider reducing watering frequency to prevent root rot',
        icon: '💧',
        type: 'warning'
      });
    } else {
      insights.push({
        id: 2,
        title: 'Moisture Levels Good',
        description: 'Your soil moisture levels are within the optimal range for healthy plant growth',
        icon: '💧',
        type: 'positive'
      });
    }

    // Analyze temperature patterns
    const avgTemperature = sensorData.reduce((sum, data) => sum + (data.temperature || 0), 0) / sensorData.length;
    if (avgTemperature > 30) {
      insights.push({
        id: 3,
        title: 'High Temperature Alert',
        description: 'Average temperature is above optimal range. Consider providing shade or increasing watering',
        icon: '🌡️',
        type: 'warning'
      });
    } else if (avgTemperature < 15) {
      insights.push({
        id: 3,
        title: 'Low Temperature Alert',
        description: 'Temperature is below optimal range. Consider protection from cold or moving plants indoors',
        icon: '🌡️',
        type: 'warning'
      });
    }

    // Zone-specific insights
    (zones || []).forEach(zone => {
      const zoneData = sensorData.filter(data => data.zone_id === zone.id);
      if (zoneData.length > 0) {
        const zoneAvgMoisture = zoneData.reduce((sum, data) => sum + (data.soil_moisture || 0), 0) / zoneData.length;
        
        if (zoneAvgMoisture < 25) {
          insights.push({
            id: insights.length + 1,
            title: `${zone.name} Needs Attention`,
            description: `${zone.name} has consistently low moisture levels. Check sensor placement and watering system`,
            icon: '🌱',
            type: 'warning'
          });
        }
      }
    });

    // Water efficiency insight
    const totalWaterUsed = pumpCommands.reduce((sum, cmd) => {
      const duration = cmd.parameters?.duration || 30;
      return sum + (duration * 0.5);
    }, 0);

    if (totalWaterUsed > 0) {
      const efficiency = this.calculateWaterEfficiency(sensorData, totalWaterUsed);
      if (efficiency > 0.8) {
        insights.push({
          id: insights.length + 1,
          title: 'Excellent Water Efficiency',
          description: `Your system is ${Math.round(efficiency * 100)}% efficient. Great job optimizing water usage!`,
          icon: '🏆',
          type: 'positive'
        });
      }
    }

    return insights.slice(0, 6); // Limit to 6 insights
  }

  // Calculate water efficiency
  static calculateWaterEfficiency(sensorData, totalWaterUsed) {
    if (sensorData.length === 0 || totalWaterUsed === 0) return 0;
    
    const optimalReadings = sensorData.filter(data => 
      data.soil_moisture >= 40 && data.soil_moisture <= 60
    ).length;
    
    return optimalReadings / sensorData.length;
  }

  // Get performance metrics
  static async getPerformanceMetrics(period = 'month') {
    try {
      console.log('Getting performance metrics for period:', period);
      const data = await this.getAnalyticsData(period);
      
      console.log('Raw analytics data:', {
        sensorDataCount: data.sensorData?.length || 0,
        zonesCount: data.zones?.length || 0,
        commandsCount: data.commands?.length || 0,
        wateringControlsCount: data.wateringControls?.length || 0
      });
      
      const waterUsageByZone = this.calculateWaterUsageByZone(data.commands || [], data.zones || [], period);
      const waterSavings = this.calculateWaterSavings(data.commands || [], period);
      const moistureDistribution = this.calculateMoistureDistribution(data.sensorData || []);
      const healthScores = this.calculatePlantHealthScores(data.sensorData || [], data.zones || []);
      const insights = this.generateInsights(data.sensorData || [], data.zones || [], data.commands || []);

      // Calculate summary metrics with proper null checks
      const totalWaterUsage = waterUsageByZone.reduce((sum, zone) => sum + (zone.usage || 0), 0);
      const totalWaterSaved = waterSavings.reduce((sum, month) => sum + (month.saved || 0), 0);
      const avgMoisture = data.sensorData && data.sensorData.length > 0 
        ? data.sensorData.reduce((sum, reading) => sum + (reading.soil_moisture || 0), 0) / data.sensorData.length 
        : 0;
      const avgHealthScore = healthScores.length > 0 
        ? healthScores.reduce((sum, zone) => sum + (zone.score || 0), 0) / healthScores.length 
        : 0;

      const result = {
        summary: {
          totalWaterUsage: Math.round(totalWaterUsage * 10) / 10,
          totalWaterSaved: Math.round(totalWaterSaved * 10) / 10,
          avgMoisture: Math.round(avgMoisture * 10) / 10,
          avgHealthScore: Math.round(avgHealthScore),
          dataPoints: data.sensorData?.length || 0,
          zones: data.zones?.length || 0
        },
        waterUsageByZone,
        waterSavings,
        moistureDistribution,
        healthScores,
        insights,
        period
      };
      
      console.log('Performance metrics result:', {
        summary: result.summary,
        waterUsageByZoneCount: result.waterUsageByZone.length,
        waterSavingsCount: result.waterSavings.length,
        moistureDistributionCount: result.moistureDistribution.length,
        healthScoresCount: result.healthScores.length,
        insightsCount: result.insights.length
      });
      
      return result;
    } catch (error) {
      console.error('Error calculating performance metrics:', error);
      // Return empty data structure instead of throwing
      return {
        summary: {
          totalWaterUsage: 0,
          totalWaterSaved: 0,
          avgMoisture: 0,
          avgHealthScore: 0,
          dataPoints: 0,
          zones: 0
        },
        waterUsageByZone: [],
        waterSavings: [],
        moistureDistribution: [],
        healthScores: [],
        insights: [{
          id: 1,
          title: 'No Data Available',
          description: 'Connect your sensors and add some devices to start seeing analytics data.',
          icon: '📊',
          type: 'recommendation'
        }],
        period
      };
    }
  }

  // Helper function to get start date based on period
  static getStartDate(period) {
    const now = new Date();
    switch (period) {
      case 'week':
        return new Date(now.getTime() - 7 * 24 * 60 * 60 * 1000);
      case 'month':
        return new Date(now.getTime() - 30 * 24 * 60 * 60 * 1000);
      case 'quarter':
        return new Date(now.getTime() - 90 * 24 * 60 * 60 * 1000);
      case 'year':
        return new Date(now.getTime() - 365 * 24 * 60 * 60 * 1000);
      default:
        return new Date(now.getTime() - 30 * 24 * 60 * 60 * 1000);
    }
  }

  // Get trend data for charts
  static async getTrendData(period = 'month') {
    try {
      const { data: { user } } = await supabase.auth.getUser();
      if (!user) throw new Error('User not authenticated');

      const endDate = new Date();
      const startDate = this.getStartDate(period);
      
      const { data: sensorData, error } = await supabase
        .from('sensor_data')
        .select('*')
        .eq('user_id', user.id)
        .gte('timestamp', startDate.toISOString())
        .lte('timestamp', endDate.toISOString())
        .order('timestamp', { ascending: true });

      if (error) {
        const handledError = handleSupabaseError(error);
        console.error('Error fetching trend data:', handledError);
        throw handledError;
      }

      // Group data by day/hour based on period
      const groupedData = this.groupDataByTime(sensorData, period);
      
      return groupedData;
    } catch (error) {
      console.error('Error fetching trend data:', error);
      throw error;
    }
  }

  // Group sensor data by time intervals
  static groupDataByTime(sensorData, period) {
    const grouped = {};
    
    sensorData.forEach(data => {
      const date = new Date(data.timestamp);
      let key;
      
      if (period === 'week') {
        key = date.toLocaleDateString('en-US', { weekday: 'short' });
      } else if (period === 'month') {
        key = date.toLocaleDateString('en-US', { month: 'short', day: 'numeric' });
      } else {
        key = date.toLocaleDateString('en-US', { month: 'short' });
      }
      
      if (!grouped[key]) {
        grouped[key] = {
          temperature: [],
          humidity: [],
          soil_moisture: [],
          count: 0
        };
      }
      
      grouped[key].temperature.push(data.temperature);
      grouped[key].humidity.push(data.humidity);
      grouped[key].soil_moisture.push(data.soil_moisture);
      grouped[key].count++;
    });
    
    // Calculate averages
    const result = [];
    Object.keys(grouped).forEach(key => {
      const item = grouped[key];
      result.push({
        date: key,
        temperature: item.temperature.reduce((a, b) => a + b, 0) / item.temperature.length,
        humidity: item.humidity.reduce((a, b) => a + b, 0) / item.humidity.length,
        soil_moisture: item.soil_moisture.reduce((a, b) => a + b, 0) / item.soil_moisture.length,
        count: item.count
      });
    });
    
    return result;
  }

  // Real-time subscription for analytics data
  static subscribeToAnalyticsData(callback) {
    if (!supabase) {
      console.error('Cannot create subscription: Supabase client not initialized');
      return null;
    }

    const subscription = supabase
      .channel('analytics_data_changes')
      .on(
        'postgres_changes',
        {
          event: 'INSERT',
          schema: 'public',
          table: 'sensor_data'
        },
        (payload) => {
          console.log('Analytics data change:', payload);
          callback(payload);
        }
      )
      .on(
        'postgres_changes',
        {
          event: 'INSERT',
          schema: 'public',
          table: 'commands'
        },
        (payload) => {
          console.log('Command data change:', payload);
          callback(payload);
        }
      )
      .subscribe();

    return subscription;
  }

  // Unsubscribe from real-time updates
  static unsubscribe(subscription) {
    if (subscription && supabase) {
      supabase.removeChannel(subscription);
    }
  }
}

export default AnalyticsService; 