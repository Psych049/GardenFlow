import React, { useState, useEffect } from 'react';
import { useTheme } from '../../contexts/ThemeContext';
import { FiX, FiCheck, FiAlertTriangle, FiInfo } from 'react-icons/fi';

const Alert = ({ 
  children, 
  variant = 'info', 
  onClose, 
  className = '',
  dismissible = false,
  autoHide = false,
  duration = 5000,
  icon,
  showTimestamp = false,
  priority = 'normal',
  ...props 
}) => {
  const { theme } = useTheme();
  const isDark = theme === 'dark';
  const [isVisible, setIsVisible] = useState(true);
  const [timeAgo, setTimeAgo] = useState('now');
  const [createdAt] = useState(new Date());

  // Auto-hide functionality
  useEffect(() => {
    if (autoHide && duration > 0 && !dismissible) {
      const timer = setTimeout(() => {
        handleClose();
      }, duration);
      
      return () => clearTimeout(timer);
    }
  }, [autoHide, duration, dismissible]);

  // Update timestamp display
  useEffect(() => {
    if (showTimestamp) {
      const updateTimeAgo = () => {
        const now = new Date();
        const diffMinutes = Math.floor((now - createdAt) / (1000 * 60));
        
        if (diffMinutes < 1) setTimeAgo('now');
        else if (diffMinutes < 60) setTimeAgo(`${diffMinutes}m ago`);
        else setTimeAgo(`${Math.floor(diffMinutes / 60)}h ago`);
      };
      
      updateTimeAgo();
      const interval = setInterval(updateTimeAgo, 60000); // Update every minute
      
      return () => clearInterval(interval);
    }
  }, [showTimestamp, createdAt]);

  const handleClose = () => {
    setIsVisible(false);
    if (onClose) {
      setTimeout(() => onClose(), 150); // Delay to allow animation
    }
  };

  if (!isVisible) return null;

  // Enhanced variants with priority levels
  const variants = {
    success: {
      container: isDark ? 'bg-green-900 bg-opacity-20 border-green-500 text-green-400' : 'bg-green-50 border-green-500 text-green-700',
      icon: isDark ? 'text-green-400' : 'text-green-600',
      iconComponent: icon || <FiCheck className="w-5 h-5" />,
      borderColor: 'border-green-500'
    },
    error: {
      container: isDark ? 'bg-red-900 bg-opacity-20 border-red-500 text-red-400' : 'bg-red-50 border-red-500 text-red-700',
      icon: isDark ? 'text-red-400' : 'text-red-600',
      iconComponent: icon || <FiX className="w-5 h-5" />,
      borderColor: 'border-red-500'
    },
    warning: {
      container: isDark ? 'bg-yellow-900 bg-opacity-20 border-yellow-500 text-yellow-400' : 'bg-yellow-50 border-yellow-500 text-yellow-700',
      icon: isDark ? 'text-yellow-400' : 'text-yellow-600',
      iconComponent: icon || <FiAlertTriangle className="w-5 h-5" />,
      borderColor: 'border-yellow-500'
    },
    info: {
      container: isDark ? 'bg-blue-900 bg-opacity-20 border-blue-500 text-blue-400' : 'bg-blue-50 border-blue-500 text-blue-700',
      icon: isDark ? 'text-blue-400' : 'text-blue-600',
      iconComponent: icon || <FiInfo className="w-5 h-5" />,
      borderColor: 'border-blue-500'
    }
  };

  const selectedVariant = variants[variant];
  const priorityRing = priority === 'high' ? `ring-2 ring-${selectedVariant.borderColor.split('-')[1]}-500 ring-opacity-50` : '';
  
  const containerClasses = `
    flex items-center p-4 border-l-4 rounded-lg shadow-sm transition-all duration-300 transform
    ${selectedVariant.container}
    ${priorityRing}
    ${className}
    ${isVisible ? 'translate-y-0 opacity-100' : '-translate-y-2 opacity-0'}
  `.trim();

  return (
    <div className={containerClasses} {...props}>
      <div className={`mr-3 flex-shrink-0 ${selectedVariant.icon}`}>
        {selectedVariant.iconComponent}
      </div>
      <div className="flex-1">
        {children}
        {showTimestamp && (
          <div className={`text-xs mt-1 opacity-70`}>
            {timeAgo}
          </div>
        )}
      </div>
      {(dismissible || autoHide) && (
        <button 
          onClick={handleClose}
          className={`ml-3 p-1 rounded-full transition-colors ${
            isDark ? 'hover:bg-opacity-20 hover:bg-white' : 'hover:bg-opacity-20 hover:bg-black'
          }`}
          aria-label="Close alert"
        >
          <FiX className="w-4 h-4" />
        </button>
      )}
    </div>
  );
};

export default Alert;