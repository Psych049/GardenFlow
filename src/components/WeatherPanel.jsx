import React, { useEffect, useState } from "react";
import { useTheme } from '../contexts/ThemeContext';
import axios from "axios";

const WeatherPanel = () => {
  const { theme } = useTheme();
  const isDark = theme === 'dark';
  
  const [city, setCity] = useState("Mumbai");
  const [currentSearchCity, setCurrentSearchCity] = useState("Mumbai");
  const [weatherData, setWeatherData] = useState(null);
  const [forecast, setForecast] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  const apiKey = "031c15b46712429b8cf162631251707";
  const baseUrl = "https://api.weatherapi.com/v1";

  const fetchWeather = async (query) => {
    setLoading(true);
    setError(null);

    try {
      const response = await axios.get(
        `${baseUrl}/forecast.json?key=${apiKey}&q=${query}&days=5&aqi=no&alerts=no`
      );

      if (response.data.error) {
        if (response.data.error.code === 1006) {
          setError(`Location "${query}" not found. Please try a different city.`);
        } else {
          setError(`Weather API error: ${response.data.error.message}`);
        }
        setWeatherData(null);
        setForecast([]);
      } else if (response.data.current && response.data.forecast) {
        setWeatherData(response.data.current);
        setForecast(response.data.forecast.forecastday);
      } else {
        setError("Received incomplete weather data. Please try again.");
        setWeatherData(null);
        setForecast([]);
      }
    } catch (err) {
      console.error("Error fetching weather:", err);
      setWeatherData(null);
      setForecast([]);
      
      if (err.response?.data?.error) {
        if (err.response.data.error.code === 1006) {
          setError(`Location "${query}" not found. Please try a different city.`);
        } else {
          setError(`Weather API error: ${err.response.data.error.message}`);
        }
      } else if (err.code === 'NETWORK_ERROR' || err.message.includes('Network Error')) {
        setError("Network error. Please check your internet connection and try again.");
      } else {
        setError("Unable to fetch weather data. Please try again later.");
      }
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchWeather(currentSearchCity);
  }, [currentSearchCity]);

  const handleCityInputChange = (e) => {
    setCity(e.target.value);
  };

  const handleCitySearch = () => {
    if (!city.trim() || city.length < 2) {
      setError("Please enter a valid city name (at least 2 characters).");
      setWeatherData(null);
      setForecast([]);
      return;
    }
    setCurrentSearchCity(city);
  };

  const handleLocationClick = async () => {
    setError(null);
    if (!navigator.geolocation) {
      setError("Geolocation not supported by your browser.");
      return;
    }

    setLoading(true);
    
    // Add timeout to prevent hanging
    const timeoutId = setTimeout(() => {
      setError("Location request timed out. Please try again or enter a city manually.");
      setLoading(false);
    }, 10000); // 10 second timeout

    navigator.geolocation.getCurrentPosition(
      async (position) => {
        clearTimeout(timeoutId);
        const { latitude, longitude } = position.coords;
        try {
          const reverseRes = await axios.get(`https://nominatim.openstreetmap.org/reverse`, {
            params: {
              lat: latitude,
              lon: longitude,
              format: "json"
            },
            headers: {
              'Accept-Language': 'en'
            },
            timeout: 5000 // 5 second timeout for reverse geocoding
          });

          const cityName =
            reverseRes.data.address.city ||
            reverseRes.data.address.town ||
            reverseRes.data.address.village ||
            reverseRes.data.address.state ||
            `${latitude},${longitude}`;

          setCity(cityName);
          setCurrentSearchCity(cityName);
        } catch (geoErr) {
          console.error("Reverse geocoding failed:", geoErr);
          setError("Failed to get city from location. Showing weather by coordinates.");
          setCurrentSearchCity(`${latitude},${longitude}`);
        } finally {
          setLoading(false);
        }
      },
      (err) => {
        clearTimeout(timeoutId);
        console.error("Geolocation error:", err);
        
        let errorMessage = "Failed to get your location. Please ensure location services are enabled.";
        
        switch (err.code) {
          case err.PERMISSION_DENIED:
            errorMessage = "Location access denied. Please allow location access in your browser settings.";
            break;
          case err.POSITION_UNAVAILABLE:
            errorMessage = "Location information unavailable. Please try again or enter a city manually.";
            break;
          case err.TIMEOUT:
            errorMessage = "Location request timed out. Please try again or enter a city manually.";
            break;
          default:
            errorMessage = "Failed to get your location. Please try again or enter a city manually.";
        }
        
        setError(errorMessage);
        setLoading(false);
        setWeatherData(null);
        setForecast([]);
      }
    );
  };

  return (
    <div className={`${isDark ? 'bg-gray-800' : 'bg-white'} p-4 sm:p-6 rounded-xl shadow-md border ${isDark ? 'border-gray-700' : 'border-gray-200'}`}>
      <h2 className={`text-lg sm:text-xl font-semibold mb-4 ${isDark ? 'text-white' : 'text-gray-800'}`}>🌤️ Weather</h2>

      <div className="space-y-3 mb-4">
        <div className="flex flex-col sm:flex-row gap-2 sm:gap-3">
          <input
            type="text"
            value={city}
            onChange={handleCityInputChange}
            placeholder="Enter City"
            className={`flex-1 px-3 sm:px-4 py-2 sm:py-2.5 border rounded-md shadow-sm focus:outline-none focus:ring-2 focus:ring-green-500 transition text-sm sm:text-base ${
              isDark ? 'border-gray-600 bg-gray-700 text-white placeholder-gray-400' : 'border-gray-300 bg-white text-gray-900 placeholder-gray-500'
            }`}
          />
          <button
            onClick={handleCitySearch}
            className="px-4 py-2 sm:py-2.5 bg-green-600 text-white rounded-md hover:bg-green-700 transition text-sm sm:text-base font-medium"
          >
            Search
          </button>
        </div>

        <button
          onClick={handleLocationClick}
          className="w-full sm:w-auto px-4 py-2 sm:py-2.5 bg-blue-600 text-white rounded-md hover:bg-blue-700 transition text-sm sm:text-base font-medium"
        >
          📍 Use My Location
        </button>
      </div>

      {loading ? (
        <div className={`text-center py-8 ${isDark ? 'text-gray-300' : 'text-gray-600'}`}>
          <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-green-500 mx-auto mb-2"></div>
          Loading weather data...
        </div>
      ) : error ? (
        <div className="text-red-600 text-sm mb-4 font-medium p-3 bg-red-50 dark:bg-red-900 dark:bg-opacity-20 rounded-lg border border-red-200 dark:border-red-800">
          {error}
        </div>
      ) : weatherData && (
        <div className="space-y-4 sm:space-y-0 sm:flex sm:flex-row sm:gap-4">
          {/* Current Weather */}
          <div className={`${isDark ? 'bg-blue-900 bg-opacity-20' : 'bg-blue-50'} rounded-lg p-4 sm:p-5 flex-1 border ${isDark ? 'border-blue-800' : 'border-blue-200'}`}>
            <div className={`flex items-center gap-2 text-xl sm:text-2xl font-semibold ${isDark ? 'text-white' : 'text-gray-800'}`}>
              <img
                src={`https:${weatherData.condition.icon}`}
                alt={weatherData.condition.text}
                className="w-8 h-8 sm:w-10 sm:h-10"
              />
              <div>
                <div>{weatherData.temp_c}°C</div>
                <div className="text-sm sm:text-base font-normal opacity-90">{weatherData.condition.text}</div>
              </div>
            </div>
            <div className={`mt-3 sm:mt-4 text-sm space-y-1.5 ${isDark ? 'text-gray-300' : 'text-gray-700'}`}>
              <div className="flex justify-between"><span>Feels Like:</span><span className="font-medium">{weatherData.feelslike_c}°C</span></div>
              <div className="flex justify-between"><span>Wind:</span><span className="font-medium">{weatherData.wind_kph} kph</span></div>
              <div className="flex justify-between"><span>Humidity:</span><span className="font-medium">{weatherData.humidity}%</span></div>
              <div className="flex justify-between"><span>UV Index:</span><span className="font-medium">{weatherData.uv}</span></div>
              <div className="flex justify-between"><span>Precipitation:</span><span className="font-medium">{weatherData.precip_mm} mm</span></div>
            </div>
          </div>

          {/* 5-Day Forecast */}
          <div className={`${isDark ? 'bg-gray-700' : 'bg-gray-50'} p-4 sm:p-5 rounded-lg flex-1 border ${isDark ? 'border-gray-600' : 'border-gray-200'}`}>
            <h3 className={`text-base sm:text-lg font-medium mb-3 sm:mb-4 ${isDark ? 'text-white' : 'text-gray-800'}`}>5-Day Forecast</h3>
            <div className="grid grid-cols-5 gap-1.5 sm:gap-2 text-center">
              {forecast.map((day) => (
                <div key={day.date} className={`${isDark ? 'bg-gray-800' : 'bg-white'} p-2 sm:p-3 rounded-lg shadow border ${isDark ? 'border-gray-600' : 'border-gray-200'}`}>
                  <div className={`font-semibold text-xs sm:text-sm mb-1 ${isDark ? 'text-white' : 'text-gray-800'}`}>
                    {new Date(day.date).toLocaleDateString("en-US", { weekday: "short" })}
                  </div>
                  <img
                    src={`https:${day.day.condition.icon}`}
                    alt={day.day.condition.text}
                    className="w-6 h-6 sm:w-8 sm:h-8 mx-auto mb-1"
                  />
                  <div className="text-green-600 dark:text-green-400 text-xs font-medium">↑ {day.day.maxtemp_c}°</div>
                  <div className="text-blue-600 dark:text-blue-400 text-xs font-medium">↓ {day.day.mintemp_c}°</div>
                </div>
              ))}
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default WeatherPanel;
