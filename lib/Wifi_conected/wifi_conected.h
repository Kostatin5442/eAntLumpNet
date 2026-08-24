#ifndef WIFI_CONECTED_H
#define WIFI_CONECTED_H
#include <WebServer.h>
#include <WiFi.h>
#include <Preferences.h>
#include <pgmspace.h>

const char system_wifi[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">DASHBOARD_HTML
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>ESP32 • WiFi Setup</title>
<style>
  :root { 
    --bg: #0a0e27; 
    --card: rgba(15, 23, 42, 0.8); 
    --accent: #00d4ff; 
    --accent-glow: rgba(0, 212, 255, 0.4);
    --success: #10b981;
    --error: #ef4444;
    --text: #f1f5f9; 
    --muted: #64748b;
    --border: rgba(255, 255, 255, 0.08);
  }
  
  * { box-sizing: border-box; margin: 0; padding: 0; -webkit-tap-highlight-color: transparent; }
  
  body { 
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; 
    background: var(--bg); 
    color: var(--text); 
    min-height: 100vh; 
    padding: 20px; 
    display: flex; 
    justify-content: center; 
    align-items: flex-start;
    position: relative;
    overflow-x: hidden;
  }
  
  /* Анімований фон */
  body::before {
    content: '';
    position: fixed;
    top: -50%;
    left: -50%;
    width: 200%;
    height: 200%;
    background: 
      radial-gradient(circle at 20% 50%, rgba(0, 212, 255, 0.15) 0%, transparent 50%),
      radial-gradient(circle at 80% 80%, rgba(139, 92, 246, 0.15) 0%, transparent 50%),
      radial-gradient(circle at 40% 20%, rgba(236, 72, 153, 0.1) 0%, transparent 50%);
    animation: bg-shift 20s ease-in-out infinite;
    z-index: 0;
    pointer-events: none;
  }
  
  @keyframes bg-shift {
    0%, 100% { transform: translate(0, 0) rotate(0deg); }
    33% { transform: translate(30px, -30px) rotate(120deg); }
    66% { transform: translate(-20px, 20px) rotate(240deg); }
  }
  
  .container {
    width: 100%;
    max-width: 480px;
    position: relative;
    z-index: 1;
  }
  
  /* Заголовок */
  .header {
    text-align: center;
    margin-bottom: 32px;
    animation: fadeInDown 0.6s ease-out;
  }
  
  @keyframes fadeInDown {
    from { opacity: 0; transform: translateY(-20px); }
    to { opacity: 1; transform: translateY(0); }
  }
  
  .logo {
    width: 80px;
    height: 80px;
    margin: 0 auto 20px;
    background: linear-gradient(135deg, var(--accent), #8b5cf6);
    border-radius: 24px;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 40px;
    box-shadow: 0 10px 40px var(--accent-glow);
    animation: pulse 3s ease-in-out infinite;
  }
  
  @keyframes pulse {
    0%, 100% { transform: scale(1); box-shadow: 0 10px 40px var(--accent-glow); }
    50% { transform: scale(1.05); box-shadow: 0 15px 50px var(--accent-glow); }
  }
  
  h1 {
    font-size: 28px;
    font-weight: 800;
    margin-bottom: 8px;
    background: linear-gradient(135deg, #fff, var(--accent));
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
  }
  
  .subtitle {
    color: var(--muted);
    font-size: 14px;
    line-height: 1.5;
  }
  
  /* Картка */
  .card {
    background: var(--card);
    backdrop-filter: blur(20px);
    border: 1px solid var(--border);
    border-radius: 24px;
    padding: 32px;
    box-shadow: 0 20px 60px rgba(0, 0, 0, 0.5);
    animation: fadeInUp 0.6s ease-out 0.2s both;
  }
  
  @keyframes fadeInUp {
    from { opacity: 0; transform: translateY(20px); }
    to { opacity: 1; transform: translateY(0); }
  }
  
  /* Поля */
  .field {
    margin-bottom: 24px;
  }
  
  label {
    display: block;
    font-size: 11px;
    color: var(--muted);
    text-transform: uppercase;
    letter-spacing: 1.5px;
    margin-bottom: 10px;
    font-weight: 700;
  }
  
  /* Кнопка сканування */
  .scan-btn {
    width: 100%;
    padding: 16px;
    background: linear-gradient(135deg, rgba(0, 212, 255, 0.1), rgba(139, 92, 246, 0.1));
    border: 2px solid var(--accent);
    border-radius: 16px;
    color: var(--accent);
    font-size: 15px;
    font-weight: 700;
    cursor: pointer;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 10px;
    position: relative;
    overflow: hidden;
  }
  
  .scan-btn::before {
    content: '';
    position: absolute;
    top: 50%;
    left: 50%;
    width: 0;
    height: 0;
    border-radius: 50%;
    background: var(--accent);
    transform: translate(-50%, -50%);
    transition: width 0.6s, height 0.6s;
  }
  
  .scan-btn:hover::before {
    width: 300px;
    height: 300px;
  }
  
  .scan-btn:hover {
    color: #000;
    border-color: var(--accent);
    transform: translateY(-2px);
    box-shadow: 0 10px 30px var(--accent-glow);
  }
  
  .scan-btn:active {
    transform: translateY(0);
  }
  
  .scan-btn:disabled {
    opacity: 0.5;
    cursor: not-allowed;
  }
  
  .scan-btn span {
    position: relative;
    z-index: 1;
  }
  
  /* Список мереж */
  .network-list {
    max-height: 280px;
    overflow-y: auto;
    margin-top: 16px;
    padding-right: 8px;
  }
  
  .network-list::-webkit-scrollbar {
    width: 6px;
  }
  
  .network-list::-webkit-scrollbar-track {
    background: rgba(255, 255, 255, 0.05);
    border-radius: 3px;
  }
  
  .network-list::-webkit-scrollbar-thumb {
    background: var(--accent);
    border-radius: 3px;
  }
  
  .network-item {
    padding: 16px;
    background: rgba(255, 255, 255, 0.03);
    border: 1px solid var(--border);
    border-radius: 12px;
    margin-bottom: 10px;
    cursor: pointer;
    display: flex;
    justify-content: space-between;
    align-items: center;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    position: relative;
    overflow: hidden;
  }
  
  .network-item::before {
    content: '';
    position: absolute;
    left: 0;
    top: 0;
    width: 4px;
    height: 100%;
    background: var(--accent);
    transform: scaleY(0);
    transition: transform 0.3s;
  }
  
  .network-item:hover {
    background: rgba(0, 212, 255, 0.05);
    border-color: rgba(0, 212, 255, 0.3);
    transform: translateX(4px);
  }
  
  .network-item:hover::before {
    transform: scaleY(1);
  }
  
  .network-item.selected {
    background: rgba(0, 212, 255, 0.1);
    border-color: var(--accent);
    box-shadow: 0 0 20px var(--accent-glow);
  }
  
  .network-item.selected::before {
    transform: scaleY(1);
  }
  
  .network-info {
    display: flex;
    align-items: center;
    gap: 12px;
    flex: 1;
  }
  
  .network-icon {
    width: 36px;
    height: 36px;
    background: rgba(0, 212, 255, 0.1);
    border-radius: 10px;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 18px;
  }
  
  .network-details {
    flex: 1;
  }
  
  .network-name {
    font-weight: 700;
    font-size: 15px;
    margin-bottom: 2px;
  }
  
  .network-meta {
    font-size: 12px;
    color: var(--muted);
  }
  
  /* Візуалізація сигналу */
  .signal-bars {
    display: flex;
    gap: 3px;
    align-items: flex-end;
    height: 20px;
  }
  
  .signal-bars span {
    width: 4px;
    background: rgba(255, 255, 255, 0.2);
    border-radius: 2px;
    transition: background 0.3s;
  }
  
  .signal-bars span:nth-child(1) { height: 25%; }
  .signal-bars span:nth-child(2) { height: 50%; }
  .signal-bars span:nth-child(3) { height: 75%; }
  .signal-bars span:nth-child(4) { height: 100%; }
  
  .signal-bars.strong span { background: var(--success); }
  .signal-bars.medium span:nth-child(-n+3) { background: #f59e0b; }
  .signal-bars.weak span:nth-child(-n+2) { background: var(--error); }
  
  /* Поля вводу */
  input {
    width: 100%;
    padding: 16px 18px;
    background: rgba(0, 0, 0, 0.3);
    border: 2px solid var(--border);
    border-radius: 14px;
    color: var(--text);
    font-size: 15px;
    outline: none;
    transition: all 0.3s;
  }
  
  input:focus {
    border-color: var(--accent);
    background: rgba(0, 212, 255, 0.05);
    box-shadow: 0 0 0 4px var(--accent-glow);
  }
  
  input::placeholder {
    color: var(--muted);
  }
  
  /* Кнопка збереження */
  .save-btn {
    width: 100%;
    padding: 18px;
    background: linear-gradient(135deg, var(--accent), #8b5cf6);
    border: none;
    border-radius: 14px;
    color: #000;
    font-size: 16px;
    font-weight: 800;
    cursor: pointer;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    margin-top: 8px;
    text-transform: uppercase;
    letter-spacing: 1px;
    position: relative;
    overflow: hidden;
  }
  
  .save-btn::before {
    content: '';
    position: absolute;
    top: 0;
    left: -100%;
    width: 100%;
    height: 100%;
    background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.3), transparent);
    transition: left 0.5s;
  }
  
  .save-btn:hover::before {
    left: 100%;
  }
  
  .save-btn:hover {
    transform: translateY(-2px);
    box-shadow: 0 15px 40px var(--accent-glow);
  }
  
  .save-btn:active {
    transform: translateY(0);
  }
  
  .save-btn:disabled {
    opacity: 0.5;
    cursor: not-allowed;
    transform: none;
  }
  
  /* Статус */
  .status {
    padding: 16px;
    border-radius: 14px;
    margin-top: 20px;
    font-size: 14px;
    font-weight: 600;
    text-align: center;
    display: none;
    animation: slideIn 0.3s ease-out;
  }
  
  @keyframes slideIn {
    from { opacity: 0; transform: translateY(-10px); }
    to { opacity: 1; transform: translateY(0); }
  }
  
  .status.success {
    background: rgba(16, 185, 129, 0.15);
    color: var(--success);
    border: 2px solid var(--success);
    display: block;
  }
  
  .status.error {
    background: rgba(239, 68, 68, 0.15);
    color: var(--error);
    border: 2px solid var(--error);
    display: block;
  }
  
  .status.loading {
    background: rgba(0, 212, 255, 0.15);
    color: var(--accent);
    border: 2px solid var(--accent);
    display: block;
  }
  
  /* Спінер */
  .spinner {
    display: inline-block;
    width: 16px;
    height: 16px;
    border: 2px solid var(--accent);
    border-top-color: transparent;
    border-radius: 50%;
    animation: spin 0.8s linear infinite;
    vertical-align: middle;
    margin-right: 10px;
  }
  
  @keyframes spin {
    to { transform: rotate(360deg); }
  }
  
  /* Прогрес-бар */
  .progress-bar {
    width: 100%;
    height: 4px;
    background: rgba(255, 255, 255, 0.1);
    border-radius: 2px;
    margin-top: 12px;
    overflow: hidden;
    display: none;
  }
  
  .progress-bar.active {
    display: block;
  }
  
  .progress-fill {
    height: 100%;
    background: linear-gradient(90deg, var(--accent), #8b5cf6);
    border-radius: 2px;
    animation: progress 2s ease-in-out infinite;
  }
  
  @keyframes progress {
    0% { width: 0%; }
    50% { width: 100%; }
    100% { width: 0%; }
  }
  
  /* Адаптив */
  @media (max-width: 480px) {
    .card { padding: 24px; }
    h1 { font-size: 24px; }
    .logo { width: 64px; height: 64px; font-size: 32px; }
  }
</style>
</head>
<body>
  <div class="container">
    <div class="header">
      <div class="logo">📡</div>
      <h1>WiFi Setup</h1>
      <div class="subtitle">Підключіть ESP32 до вашої домашньої мережі</div>
    </div>

    <div class="card">
      <div class="field">
        <label>Доступні мережі</label>
        <button class="scan-btn" onclick="scanNetworks()" id="scanBtn">
          <span>🔍 Сканувати мережі</span>
        </button>
        <div class="network-list" id="networkList"></div>
      </div>

      <div class="field">
        <label>Назва мережі (SSID)</label>
        <input type="text" id="ssid" placeholder="Оберіть зі списку або введіть вручну">
      </div>

      <div class="field">
        <label>Пароль</label>
        <input type="password" id="password" placeholder="••••••••">
      </div>

      <button class="save-btn" onclick="saveWifi()" id="saveBtn">
        💾 Зберегти та підключитись
      </button>

      <div class="progress-bar" id="progressBar">
        <div class="progress-fill"></div>
      </div>

      <div class="status" id="status"></div>
    </div>
  </div>

<script>
function getSignalStrength(rssi) {
  if (rssi > -55) return 'strong';
  if (rssi > -70) return 'medium';
  return 'weak';
}

function getSignalText(rssi) {
  if (rssi > -55) return 'Відмінний';
  if (rssi > -70) return 'Добрий';
  if (rssi > -80) return 'Слабкий';
  return 'Дуже слабкий';
}

function showStatus(msg, type) {
  const s = document.getElementById('status');
  s.className = 'status ' + type;
  s.innerHTML = (type === 'loading' ? '<span class="spinner"></span>' : '') + msg;
}

async function scanNetworks() {
  const btn = document.getElementById('scanBtn');
  const list = document.getElementById('networkList');
  btn.disabled = true;
  btn.innerHTML = '<span><span class="spinner"></span>Сканування...</span>';
  showStatus('Пошук WiFi мереж...', 'loading');

  try {
    const res = await fetch('/api/scan');
    const networks = await res.json();
    list.innerHTML = '';
    
    if (networks.length === 0) {
      list.innerHTML = '<div style="padding:20px;color:var(--muted);text-align:center;">Мереж не знайдено</div>';
    } else {
      networks.sort((a, b) => b.rssi - a.rssi); // Сортування за силою сигналу
      
      networks.forEach(n => {
        const signalClass = getSignalStrength(n.rssi);
        const signalText = getSignalText(n.rssi);
        
        const item = document.createElement('div');
        item.className = 'network-item';
        item.innerHTML = `
          <div class="network-info">
            <div class="network-icon">${n.lock ? '🔒' : '📶'}</div>
            <div class="network-details">
              <div class="network-name">${n.ssid}</div>
              <div class="network-meta">${signalText} • ${n.lock ? 'Захищена' : 'Відкрита'}</div>
            </div>
          </div>
          <div class="signal-bars ${signalClass}">
            <span></span><span></span><span></span><span></span>
          </div>
        `;
        
        item.onclick = () => {
          document.querySelectorAll('.network-item').forEach(el => el.classList.remove('selected'));
          item.classList.add('selected');
          document.getElementById('ssid').value = n.ssid;
          document.getElementById('password').focus();
        };
        
        list.appendChild(item);
      });
    }
    
    showStatus(`✅ Знайдено мереж: ${networks.length}`, 'success');
  } catch (e) {
    showStatus('❌ Помилка сканування', 'error');
  }
  
  btn.disabled = false;
  btn.innerHTML = '<span>🔄 Сканувати мережі</span>';
}

async function saveWifi() {
  const ssid = document.getElementById('ssid').value.trim();
  const password = document.getElementById('password').value;
  
  if (!ssid) {
    showStatus('❌ Введіть назву мережі', 'error');
    return;
  }

  const btn = document.getElementById('saveBtn');
  const progressBar = document.getElementById('progressBar');
  
  btn.disabled = true;
  btn.innerHTML = '<span class="spinner"></span>Збереження...';
  progressBar.classList.add('active');
  showStatus('Збереження налаштувань...', 'loading');

  try {
    const res = await fetch('/api/save', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: 'ssid=' + encodeURIComponent(ssid) + '&password=' + encodeURIComponent(password)
    });
    const data = await res.json();
    
    if (data.success) {
      showStatus('✅ Збережено! ESP32 перезавантажується...', 'success');
      btn.innerHTML = '✅ Збережено';
      
      setTimeout(() => {
        showStatus('🔄 Підключення до ' + ssid + '... Зачекайте 15 сек', 'loading');
      }, 2000);
    } else {
      showStatus('❌ Помилка: ' + data.error, 'error');
      btn.disabled = false;
      btn.innerHTML = '💾 Зберегти та підключитись';
      progressBar.classList.remove('active');
    }
  } catch (e) {
    showStatus('❌ Помилка з\'єднання', 'error');
    btn.disabled = false;
    btn.innerHTML = '💾 Зберегти та підключитись';
    progressBar.classList.remove('active');
  }
}

// Автоматичне сканування при завантаженні
window.onload = () => setTimeout(scanNetworks, 500);
</script>
</body>
</html>
)rawliteral";



#endif