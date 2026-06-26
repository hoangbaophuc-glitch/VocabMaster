import React, { useState } from 'react';
import './AuthScreen.css';
import wmLogo from '../asset/icons/wm.svg';

interface AuthScreenProps {
  onLogin: (name: string) => void;
  isLightMode: boolean;
}

export default function AuthScreen({ onLogin, isLightMode }: AuthScreenProps) {
  const [username, setUsername] = useState<string>('');

  const handleStart = () => {
    if (username.trim()) {
      onLogin(username);
    }
  };

  return (
    <div className={`auth-wrapper ${isLightMode ? 'light-mode' : 'dark-mode'}`}>
      <div className={"auth-container glass-panel"}>
        <div className="logo-container">
          <img src={wmLogo}/>
        </div>
        <h1 className="auth-title">
          <span className="brand-vocab">Vocab</span>
          <span className="brand-master">Master</span>
        </h1>
        <p className="auth-subtitle">Điền tên để bắt đầu</p>

        <div className="input-group">
          <input
            type="text"
            placeholder="Username..."
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            onKeyDown={(e) => e.key === 'Enter' && handleStart()}
          />
          <button className="primary-btn" onClick={handleStart}>Bắt đầu</button>
        </div>
      </div>
    </div>
  );
}
