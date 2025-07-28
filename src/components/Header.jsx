import React, { useState, useEffect, useRef } from 'react';
import { useAuth } from '../contexts/AuthContext';
import { useTheme } from '../contexts/ThemeContext';

const Header = ({ toggleSidebar }) => {
  const { user } = useAuth();
  const { theme, setThemeDirectly } = useTheme();
  const [showProfileMenu, setShowProfileMenu] = useState(false);
  const [showNotifications, setShowNotifications] = useState(false);
  const notificationsRef = useRef(null);
  const profileRef = useRef(null);
  
  // Mock notifications data - replace with real data from your backend
  const [notifications] = useState([
    {
      id: 1,
      title: "Watering Complete",
      description: "Zone A has been watered successfully",
      time: "2 minutes ago",
      read: false,
      icon: "💧"
    },
    {
      id: 2,
      title: "Temperature Alert",
      description: "Temperature in Zone B is above optimal range",
      time: "15 minutes ago",
      read: false,
      icon: "🌡️"
    },
    {
      id: 3,
      title: "System Update",
      description: "New firmware update available",
      time: "1 hour ago",
      read: true,
      icon: "🔧"
    }
  ]);
  
  // Close dropdowns when clicking outside
  useEffect(() => {
    const handleClickOutside = (event) => {
      if (notificationsRef.current && !notificationsRef.current.contains(event.target)) {
        setShowNotifications(false);
      }
      if (profileRef.current && !profileRef.current.contains(event.target)) {
        setShowProfileMenu(false);
      }
    };

    document.addEventListener('mousedown', handleClickOutside);
    return () => {
      document.removeEventListener('mousedown', handleClickOutside);
    };
  }, []);
  
  // Handler for the theme toggle
  const handleToggleTheme = () => {
    const newTheme = theme === 'light' ? 'dark' : 'light';
    setThemeDirectly(newTheme);
  };
  
  // Handler for marking all notifications as read
  const markAllNotificationsRead = () => {
    // In a real app, you would make an API call here
    console.log('Marking all notifications as read');
    setShowNotifications(false);
  };
  
  return (
    <header className={`${theme === 'dark' ? 'bg-gray-800 text-white' : 'bg-white'} border-b shadow-sm sticky top-0 z-30`}>
      <div className="px-4 sm:px-6 py-3 flex justify-between items-center">
        <div className="flex items-center">
          <button 
            onClick={toggleSidebar}
            className={`mr-3 lg:hidden p-2 rounded-md transition-colors ${
              theme === 'dark' 
                ? 'text-gray-300 hover:text-white hover:bg-gray-700' 
                : 'text-gray-500 hover:text-gray-700 hover:bg-gray-100'
            } focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-green-500`}
            aria-label="Toggle sidebar"
          >
            <svg className="h-5 w-5" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 6h16M4 12h16M4 18h16" />
            </svg>
          </button>
          <h1 className={`text-lg sm:text-xl font-bold ${theme === 'dark' ? 'text-green-400' : 'text-green-600'}`}>🌿 FarmFlow</h1>
        </div>

        <div className="flex items-center space-x-2 sm:space-x-4">
          {/* Theme Toggle */}
          <button 
            onClick={handleToggleTheme}
            className={`p-2 rounded-md transition-colors ${
              theme === 'dark' 
                ? 'text-yellow-300 hover:text-yellow-200 hover:bg-gray-700' 
                : 'text-gray-500 hover:text-gray-700 hover:bg-gray-100'
            } focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-green-500`}
            aria-label="Toggle dark mode"
            aria-pressed={theme === 'dark'}
          >
            {theme === 'dark' ? (
              <svg xmlns="http://www.w3.org/2000/svg" className="h-5 w-5" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z" />
              </svg>
            ) : (
              <svg xmlns="http://www.w3.org/2000/svg" className="h-5 w-5" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M20.354 15.354A9 9 0 018.646 3.646 9.003 9.003 0 0012 21a9.003 9.003 0 008.354-5.646z" />
              </svg>
            )}
          </button>
          
          {/* Notifications */}
          <div className="relative" ref={notificationsRef}>
            <button
              className={`relative p-2 rounded-md transition-colors ${
                theme === 'dark' 
                  ? 'text-gray-300 hover:text-gray-200 hover:bg-gray-700' 
                  : 'text-gray-500 hover:text-gray-700 hover:bg-gray-100'
              } focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-green-500`}
              onClick={() => setShowNotifications((prev) => !prev)}
              aria-label="View notifications"
              aria-haspopup="true"
              aria-expanded={showNotifications ? "true" : "false"}
            >
              <span className="sr-only">View notifications</span>
              <svg className="h-5 w-5" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path 
                  strokeLinecap="round" 
                  strokeLinejoin="round" 
                  strokeWidth={2} 
                  d="M15 17h5l-1.405-1.405A2.032 2.032 0 0118 14.158V11a6.002 6.002 0 00-4-5.659V5a2 2 0 10-4 0v.341C7.67 6.165 6 8.388 6 11v3.159c0 .538-.214 1.055-.595 1.436L4 17h5m6 0v1a3 3 0 11-6 0v-1m6 0H9" 
                />
              </svg>
              <span className="absolute top-1.5 right-1.5 h-2 w-2 rounded-full bg-red-500"></span>
            </button>
            {showNotifications && (
              <div
                className={`origin-top-right absolute right-0 mt-2 w-80 max-w-xs rounded-md shadow-lg z-50 ${
                  theme === 'dark'
                    ? 'bg-gray-800 border border-gray-700'
                    : 'bg-white border border-gray-200'
                }`}
                tabIndex={-1}
              >
                <div className="py-2 px-4 border-b border-gray-200 dark:border-gray-700 flex items-center justify-between">
                  <span className={`font-semibold text-base ${theme === 'dark' ? 'text-white' : 'text-gray-800'}`}>Notifications</span>
                  <button
                    className={`text-xs px-2 py-1 rounded ${
                      theme === 'dark'
                        ? 'text-gray-400 hover:text-white hover:bg-gray-700'
                        : 'text-gray-500 hover:text-gray-700 hover:bg-gray-100'
                    }`}
                    onClick={() => setShowNotifications(false)}
                  >
                    Close
                  </button>
                </div>
                <ul className="max-h-64 overflow-y-auto divide-y divide-gray-100 dark:divide-gray-700">
                  {/* Example notifications, replace with real data */}
                  {notifications && notifications.length > 0 ? (
                    notifications.map((notif, idx) => (
                      <li
                        key={notif.id || idx}
                        className={`px-4 py-3 flex items-start gap-3 hover:bg-gray-50 dark:hover:bg-gray-700 transition`}
                      >
                        <span className="mt-1">
                          {notif.icon ? notif.icon : (
                            <svg className="h-5 w-5 text-green-500" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                              <circle cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="2" fill="none" />
                              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 8v4l2 2" />
                            </svg>
                          )}
                        </span>
                        <div className="flex-1 min-w-0">
                          <div className={`text-sm ${notif.read ? 'text-gray-400 dark:text-gray-500' : theme === 'dark' ? 'text-white' : 'text-gray-900'} font-medium`}>
                            {notif.title}
                          </div>
                          <div className="text-xs text-gray-500 dark:text-gray-400">{notif.description}</div>
                          <div className="text-xs text-gray-400 mt-1">{notif.time}</div>
                        </div>
                        {!notif.read && (
                          <span className="ml-2 mt-1 h-2 w-2 rounded-full bg-green-500 inline-block"></span>
                        )}
                      </li>
                    ))
                  ) : (
                    <li className="px-4 py-6 text-center text-gray-400 dark:text-gray-500 text-sm">
                      No new notifications.
                    </li>
                  )}
                </ul>
                <div className="py-2 px-4 border-t border-gray-200 dark:border-gray-700 text-right">
                  <button
                    className={`text-xs font-medium ${
                      theme === 'dark'
                        ? 'text-green-300 hover:text-green-200'
                        : 'text-green-600 hover:text-green-700'
                    }`}
                    onClick={markAllNotificationsRead}
                  >
                    Mark all as read
                  </button>
                </div>
              </div>
            )}
          </div>

          {/* Profile dropdown */}
          <div className="relative" ref={profileRef}>
            <button
              type="button"
              className={`flex items-center text-sm rounded-full focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-green-500 transition-colors ${
                theme === 'dark' ? 'hover:bg-gray-700' : 'hover:bg-gray-100'
              }`}
              onClick={() => setShowProfileMenu(!showProfileMenu)}
            >
              <span className="sr-only">Open user menu</span>
              <div className={`h-8 w-8 rounded-full ${theme === 'dark' ? 'bg-green-800 text-green-100' : 'bg-green-100 text-green-800'} flex items-center justify-center font-medium`}>
                {user ? user.email.charAt(0).toUpperCase() : 'U'}
              </div>
            </button>
            
            {showProfileMenu && (
              <div className={`origin-top-right absolute right-0 mt-2 w-48 rounded-md shadow-lg ${theme === 'dark' ? 'bg-gray-700 ring-gray-600' : 'bg-white ring-black ring-opacity-5'} z-50`}>
                <div className="py-1">
                  <a href="/settings" className={`block px-4 py-2 text-sm transition-colors ${
                    theme === 'dark' 
                      ? 'text-gray-200 hover:bg-gray-600' 
                      : 'text-gray-700 hover:bg-gray-100'
                  }`}>
                    Settings
                  </a>
                  <a href="/login" className={`block px-4 py-2 text-sm transition-colors ${
                    theme === 'dark' 
                      ? 'text-gray-200 hover:bg-gray-600' 
                      : 'text-gray-700 hover:bg-gray-100'
                  }`}>
                    Sign out
                  </a>
                </div>
              </div>
            )}
          </div>
        </div>
      </div>
    </header>
  );
};

export default Header;