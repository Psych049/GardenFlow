// src/components/Sidebar.jsx
import React from 'react';
import { Link, useLocation } from 'react-router-dom';
import { FiHome, FiSettings, FiDroplet, FiActivity, FiDatabase, FiCpu, FiX } from 'react-icons/fi';
import { FaLeaf } from 'react-icons/fa';
import StickyAlert from './StickyAlert';
import { useTheme } from '../contexts/ThemeContext';

const links = [
  { to: '/dashboard', label: 'Dashboard', icon: <FiHome /> },
  { to: '/plants', label: 'Plants', icon: <FaLeaf /> },
  { to: '/schedule', label: 'Watering', icon: <FiDroplet /> },
  { to: '/sensors', label: 'Sensors', icon: <FiDatabase /> },
  { to: '/analytics', label: 'Analytics', icon: <FiActivity /> },
  { to: '/system', label: 'System', icon: <FiCpu /> },
  { to: '/settings', label: 'Settings', icon: <FiSettings /> },
];

const Sidebar = ({ isOpen, setIsOpen }) => {
  const location = useLocation();
  const { theme } = useTheme();
  
  const darkMode = theme === 'dark';

  return (
    <>
      {/* Mobile sidebar overlay */}
      {isOpen && (
        <div 
          className="fixed inset-0 z-40 bg-black bg-opacity-50 lg:hidden"
          onClick={() => setIsOpen(false)}
        ></div>
      )}
    
      {/* Sidebar */}
      <aside 
        className={`fixed lg:static inset-y-0 left-0 z-50 lg:z-auto transform ${
          isOpen ? 'translate-x-0' : '-translate-x-full lg:translate-x-0'
        } h-full ${
          darkMode ? 'bg-gray-800 text-gray-100' : 'bg-white text-gray-900'
        } flex flex-col justify-between w-64 lg:w-64 transition-all duration-300 ease-in-out border-r ${
          darkMode ? 'border-gray-700' : 'border-gray-200'
        } shadow-lg lg:shadow-sm`}
      >
        {/* Mobile Close Button */}
        <div className="lg:hidden flex justify-end p-4 border-b border-gray-200 dark:border-gray-700">
          <button
            onClick={() => setIsOpen(false)}
            className={`p-2 rounded-md ${darkMode ? 'text-gray-400 hover:text-gray-200 hover:bg-gray-700' : 'text-gray-500 hover:text-gray-700 hover:bg-gray-100'}`}
          >
            <FiX className="h-5 w-5" />
          </button>
        </div>
        
        {/* Navigation Links */}
        <nav className="flex-1 overflow-y-auto px-4 py-6 space-y-2">
          {links.map((link) => {
            const isActive = location.pathname === link.to;
            return (
              <Link
                key={link.to}
                to={link.to}
                onClick={() => setIsOpen(false)} // Close sidebar on mobile when link is clicked
                className={`flex items-center gap-3 p-3 rounded-lg transition-all duration-200 
                  ${isActive
                    ? darkMode 
                      ? 'bg-green-900 bg-opacity-50 text-green-400 font-semibold shadow-sm'
                      : 'bg-green-50 text-green-700 font-semibold shadow-sm'
                    : darkMode
                      ? 'text-gray-300 hover:bg-gray-700 hover:text-gray-100'
                      : 'text-gray-600 hover:bg-gray-50 hover:text-gray-900'
                  }`}
              >
                <span className={`text-xl ${isActive 
                  ? darkMode ? 'text-green-400' : 'text-green-500' 
                  : darkMode ? 'text-gray-300' : 'text-gray-400'}`}
                >
                  {link.icon}
                </span>
                <span className="font-medium">{link.label}</span>
              </Link>
            );
          })}
        </nav>

        {/* Sticky Alert at Bottom */}
        <div className="p-4 border-t border-gray-200 dark:border-gray-700">
          <StickyAlert 
            message="⚠ Moisture level critically low in Zone 3" 
            type="warning" 
          />
        </div>
      </aside>
    </>
  );
};

export default Sidebar;
