#ifndef INDEX_HTML_H
#define INDEX_HTML_H
#include <pgmspace.h>
// ==========================================
// 📡 СТИЛЬНА сторінка налаштування WiFi (PROGMEM)
// ==========================================
const char SETUP_PAGE_HTML[] PROGMEM = R"rawliteral(
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

// ==========================================
// 🚀 SOLID DASHBOARD: Єдина панель керування (PROGMEM)
// ==========================================
const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>ESP32 • Command Center</title>
<style>
  :root { --bg: #23335d; --card: rgba(20, 25, 40, 0.7); --accent: #00f2ff; --accent-glow: rgba(0, 242, 255, 0.3); --text: #e2e8f0; --muted: #64748b; --danger: #ef4444; }
  * { box-sizing: border-box; margin: 0; padding: 0; -webkit-tap-highlight-color: transparent; }
  body { font-family: 'Segoe UI', system-ui, sans-serif; background: var(--bg); color: var(--text); padding: 16px; min-height: 100vh; display: flex; flex-direction: column; gap: 16px; }
  
  /* Анімований фон */
  body::before { content: ''; position: fixed; top: -50%; left: -50%; width: 200%; height: 200%; background: radial-gradient(circle, var(--accent-glow) 0%, transparent 60%); opacity: 0.15; animation: pulse-bg 8s ease-in-out infinite; z-index: -1; pointer-events: none; }
  @keyframes pulse-bg { 0%, 100% { transform: scale(1); opacity: 0.15; } 50% { transform: scale(1.1); opacity: 0.25; } }

  .header { display: flex; justify-content: space-between; align-items: center; padding: 8px 4px; }
  .header h1 { font-size: 22px; font-weight: 800; letter-spacing: -0.5px; background: linear-gradient(90deg, #fff, var(--accent)); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }
  .status-badge { display: flex; align-items: center; gap: 8px; background: rgba(255,255,255,0.05); padding: 6px 12px; border-radius: 20px; font-size: 12px; font-weight: 600; border: 1px solid rgba(255,255,255,0.1); }
  .dot { width: 8px; height: 8px; border-radius: 50%; background: var(--danger); box-shadow: 0 0 8px var(--danger); transition: 0.3s; }
  .dot.online { background: #10b981; box-shadow: 0 0 8px #10b981; }

  .card { background: var(--card); backdrop-filter: blur(16px); border: 1px solid rgba(255,255,255,0.08); border-radius: 20px; padding: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); }
  .card-title { font-size: 12px; text-transform: uppercase; letter-spacing: 1.5px; color: var(--muted); margin-bottom: 16px; font-weight: 700; display: flex; justify-content: space-between; }

  /* Візуалізатор (імітація матриці) */
  .visualizer { height: 120px; border-radius: 12px; background: linear-gradient(135deg, rgba(0,242,255,0.1), rgba(0,0,0,0)); border: 1px solid rgba(0,242,255,0.2); display: flex; align-items: center; justify-content: center; position: relative; overflow: hidden; margin-bottom: 20px; }
  .visualizer::after { content: 'MATRIX ACTIVE'; font-size: 14px; font-weight: 800; color: var(--accent); letter-spacing: 2px; opacity: 0.8; animation: text-flicker 3s infinite; }
  @keyframes text-flicker { 0%, 100% { opacity: 0.8; } 50% { opacity: 0.4; } }

  /* Повзунок яскравості */
  .slider-wrap { display: flex; align-items: center; gap: 16px; }
  .slider-wrap input { flex: 1; -webkit-appearance: none; height: 6px; background: rgba(255,255,255,0.1); border-radius: 3px; outline: none; }
  .slider-wrap input::-webkit-slider-thumb { -webkit-appearance: none; width: 22px; height: 22px; background: var(--accent); border-radius: 50%; cursor: pointer; box-shadow: 0 0 15px var(--accent-glow); border: 2px solid #fff; }
  .val-display { font-family: 'Courier New', monospace; font-size: 20px; font-weight: bold; color: var(--accent); min-width: 45px; text-align: right; }

  /* Сітка ефектів */
  .grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(90px, 1fr)); gap: 10px; }
  .btn { background: rgba(255,255,255,0.03); border: 1px solid rgba(255,255,255,0.08); color: var(--text); padding: 14px 8px; border-radius: 12px; font-size: 12px; font-weight: 600; cursor: pointer; transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1); text-align: center; line-height: 1.3; }
  .btn:active { transform: scale(0.95); }
  .btn.active { background: var(--accent); color: #000; border-color: var(--accent); box-shadow: 0 0 20px var(--accent-glow); font-weight: 800; }
  .btn-off { background: rgba(239, 68, 68, 0.15); color: var(--danger); border-color: rgba(239, 68, 68, 0.3); }
  .btn-off.active { background: var(--danger); color: #fff; box-shadow: 0 0 20px rgba(239, 68, 68, 0.4); }

  /* Системна інформація */
  .sys-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }
  .sys-item { background: rgba(0,0,0,0.2); padding: 12px; border-radius: 10px; border: 1px solid rgba(255,255,255,0.05); }
  .sys-label { font-size: 10px; color: var(--muted); text-transform: uppercase; margin-bottom: 4px; }
  .sys-value { font-family: monospace; font-size: 14px; color: #fff; word-break: break-all; }

  /* Toast сповіщення */
  .toast { position: fixed; bottom: 24px; left: 50%; transform: translateX(-50%) translateY(100px); background: var(--accent); color: #000; padding: 12px 24px; border-radius: 50px; font-weight: 700; font-size: 13px; opacity: 0; transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275); box-shadow: 0 10px 40px rgba(0,242,255,0.3); z-index: 999; white-space: nowrap; }
  .toast.show { transform: translateX(-50%) translateY(0); opacity: 1; }
</style>
</head>
<body>

  <div class="header">
    <h1>LED COMMAND</h1>
    <div class="status-badge">
      <div class="dot" id="connDot"></div>
      <span id="connText">Offline</span>
    </div>
  </div>

  <div class="card">
    <div class="visualizer" id="visualizer"></div>
    
    <div class="card-title">
      <span>Яскравість</span>
      <span id="brightLabel">MASTER</span>
    </div>
    <div class="slider-wrap">
      <input type="range" id="brightness" min="0" max="250" value="100" oninput="updateBrightLabel(this.value)" onchange="setBrightness(this.value)">
      <div class="val-display" id="brightVal">100</div>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Бібліотека ефектів</div>
    <div class="grid">
      <button class="btn" onclick="setEffect('/effects_fire/state1', this)">Rainbow</button>
      <button class="btn" onclick="setEffect('/effects_fire/state2', this)">Fire</button>
      <button class="btn" onclick="setEffect('/effects_fire/state3', this)">Flashing</button>
      <button class="btn" onclick="setEffect('/effects_fire/state4', this)">Starfield</button>
      <button class="btn" onclick="setEffect('/effects_fire/state5', this)">Color Waves</button>
      <button class="btn" onclick="setEffect('/effects_fire/state6', this)">Meteor</button>
      <button class="btn" onclick="setEffect('/effects_fire/state7', this)">Ripple</button>
      <button class="btn" onclick="setEffect('/effects_fire/state8', this)">Matrix</button>
      <button class="btn" onclick="setEffect('/effects_fire/state9', this)">Aurora</button>
      <button class="btn" onclick="setEffect('/effects_fire/state10', this)">Galaxy</button>
      <button class="btn" onclick="setEffect('/effects_fire/state11', this)">Tunnel</button>
      <button class="btn" onclick="setEffect('/effects_fire/state12', this)">Spiral</button>
      <button class="btn" onclick="setEffect('/effects_fire/state13', this)">Nebula</button>
      <button class="btn" onclick="setEffect('/effects_fire/state14', this)">Sparkles</button>
      <button class="btn" onclick="setEffect('/ws2812/glitch_bars', this)">Glitch Bars</button>
      <button class="btn" onclick="setEffect('/ws2812/pixel_noise', this)">Pixel Noise</button>
      <button class="btn" onclick="setEffect('/ws2812/glitch_columns', this)">Glitch Cols</button>
      <button class="btn" onclick="setEffect('/ws2812/psychedelic_flow', this)">Psychedelic</button>
      <button class="btn btn-off" onclick="setEffect('/effects_fire/off', this)" style="grid-column: span 2;">⏹ ВИМКНУТИ ВСЕ</button>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Системний моніторинг</div>
    <div class="sys-grid">
      <div class="sys-item">
        <div class="sys-label">Мережа (SSID)</div>
        <div class="sys-value" id="sysSsid">—</div>
      </div>
      <div class="sys-item">
        <div class="sys-label">IP Адреса</div>
        <div class="sys-value" id="sysIp">—</div>
      </div>
      <div class="sys-item">
        <div class="sys-label">Сила сигналу</div>
        <div class="sys-value" id="sysRssi">—</div>
      </div>
      <div class="sys-item">
        <div class="sys-label">Режим</div>
        <div class="sys-value" id="sysMode">—</div>
      </div>
    </div>
  </div>

  <div class="toast" id="toast">Команду виконано</div>

<script>
  // Оновлення підпису яскравості без запиту до сервера (для плавності)
  function updateBrightLabel(val) {
    document.getElementById('brightVal').textContent = val;
  }

  // Відправка команди яскравості
  function setBrightness(val) {
    fetch('/brightness?value=' + val).then(() => showToast('Яскравість: ' + val));
  }

  // Відправка команди ефекту
  function setEffect(url, btn) {
    document.querySelectorAll('.btn').forEach(b => b.classList.remove('active'));
    if(btn) btn.classList.add('active');
    
    // Ефект мерехтіння візуалізатора при зміні
    const viz = document.getElementById('visualizer');
    viz.style.borderColor = 'rgba(255,255,255,0.5)';
    setTimeout(() => viz.style.borderColor = 'rgba(0,242,255,0.2)', 300);

    fetch(url).then(() => showToast('Ефект активований'));
  }

  // Показ сповіщення
  function showToast(msg) {
    const t = document.getElementById('toast');
    t.textContent = msg;
    t.classList.add('show');
    setTimeout(() => t.classList.remove('show'), 1500);
  }

  // Фонове оновлення системної інформації (кожні 3 сек)
  async function updateSystemInfo() {
    try {
      const res = await fetch('/api/wifi'); // Використовуємо твій існуючий API роут
      const data = await res.json();
      
      const dot = document.getElementById('connDot');
      const connText = document.getElementById('connText');
      
      if (data.state === 'Підключено' || data.state === 'CONNECTED') {
        dot.classList.add('online');
        connText.textContent = 'Online';
      } else {
        dot.classList.remove('online');
        connText.textContent = 'AP Mode';
      }

      document.getElementById('sysSsid').textContent = data.ssid || 'Немає';
      document.getElementById('sysIp').textContent = data.ip;
      document.getElementById('sysRssi').textContent = data.rssi + ' dBm';
      document.getElementById('sysMode').textContent = data.mode;
    } catch (e) {
      // Якщо /api/wifi ще не налаштовано, ігноруємо помилку, щоб не ламати сторінку
      console.log('Очікування даних системи...');
    }
  }

  // Запуск оновлення при завантаженні і кожні 3 секунди
  updateSystemInfo();
  setInterval(updateSystemInfo, 3000);
</script>
</body>
</html>
)rawliteral";
// ==========================================
// 🎛️ Красива веб-панель керування (PROGMEM)
// ==========================================
const char CONTROL_PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>ESP32 • Панель керування</title>
<style>
  :root { --bg: #0f172a; --card: rgba(30, 41, 59, 0.8); --accent: #10b981; --text: #f1f5f9; --muted: #94a3b8; }
  * { box-sizing: border-box; margin: 0; padding: 0; -webkit-tap-highlight-color: transparent; }
  body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; background: var(--bg); color: var(--text); padding: 16px; min-height: 100vh; }
  
  .header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 24px; }
  .header h1 { font-size: 20px; font-weight: 700; }
  .wifi-link { background: rgba(255,255,255,0.1); padding: 8px 12px; border-radius: 8px; text-decoration: none; color: var(--text); font-size: 13px; display: flex; align-items: center; gap: 6px; }

  .card { background: var(--card); backdrop-filter: blur(10px); border: 1px solid rgba(255,255,255,0.1); border-radius: 16px; padding: 20px; margin-bottom: 20px; }
  .card-title { font-size: 14px; color: var(--muted); text-transform: uppercase; letter-spacing: 1px; margin-bottom: 16px; font-weight: 600; }

  /* Яскравість */
  .slider-container { display: flex; align-items: center; gap: 16px; }
  .slider-container input { flex: 1; -webkit-appearance: none; height: 8px; background: #334155; border-radius: 4px; outline: none; }
  .slider-container input::-webkit-slider-thumb { -webkit-appearance: none; width: 24px; height: 24px; background: var(--accent); border-radius: 50%; cursor: pointer; box-shadow: 0 0 10px rgba(16, 185, 129, 0.5); }
  .brightness-val { font-family: monospace; font-size: 18px; font-weight: bold; min-width: 40px; text-align: right; }

  /* Сітка ефектів */
  .effects-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(100px, 1fr)); gap: 10px; }
  .btn-effect {
    background: rgba(255,255,255,0.05); border: 1px solid rgba(255,255,255,0.1); color: var(--text);
    padding: 12px 8px; border-radius: 10px; font-size: 13px; font-weight: 500; cursor: pointer;
    transition: all 0.2s ease; text-align: center;
  }
  .btn-effect:active { transform: scale(0.95); background: rgba(16, 185, 129, 0.2); border-color: var(--accent); }
  .btn-effect.active { background: var(--accent); color: #000; font-weight: 700; border-color: var(--accent); box-shadow: 0 0 15px rgba(16, 185, 129, 0.4); }

  /* Керування */
  .control-row { display: flex; gap: 10px; margin-top: 10px; }
  .btn-control { flex: 1; background: #334155; border: none; color: white; padding: 14px; border-radius: 10px; font-size: 15px; font-weight: 600; cursor: pointer; transition: 0.2s; }
  .btn-control:active { background: #475569; }
  .btn-off { background: #ef4444; color: white; }
  .btn-off:active { background: #dc2626; }

  /* Toast сповіщення */
  .toast {
    position: fixed; bottom: 24px; left: 50%; transform: translateX(-50%) translateY(100px);
    background: rgba(16, 185, 129, 0.9); color: #000; padding: 12px 24px; border-radius: 50px;
    font-weight: 600; font-size: 14px; opacity: 0; transition: all 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275);
    box-shadow: 0 10px 30px rgba(0,0,0,0.5); pointer-events: none; z-index: 100;
  }
  .toast.show { transform: translateX(-50%) translateY(0); opacity: 1; }
</style>
</head>
<body>

  <div class="header">
    <h1>💡 LED Controller</h1>
    <a href="/wifi" class="wifi-link">📶 Wi-Fi</a>
  </div>

  <div class="card">
    <div class="card-title">Яскравість</div>
    <div class="slider-container">
      <input type="range" id="brightness" min="0" max="250" value="100" onchange="setBrightness(this.value)">
      <span class="brightness-val" id="brightVal">100</span>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Ефекти</div>
    <div class="effects-grid">
      <button class="btn-effect" onclick="setEffect('/effects_fire/state1', this)">Rainbow</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state2', this)">Fire</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state3', this)">Flashing</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state4', this)">Starfield</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state5', this)">Color Waves</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state6', this)">Meteor</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state7', this)">Ripple</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state8', this)">Matrix</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state9', this)">Aurora</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state10', this)">Galaxy</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state11', this)">Tunnel</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state12', this)">Spiral</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state13', this)">Nebula</button>
      <button class="btn-effect" onclick="setEffect('/effects_fire/state14', this)">Sparkles</button>
      <button class="btn-effect" onclick="setEffect('/ws2812/glitch_bars', this)">Glitch Bars</button>
      <button class="btn-effect" onclick="setEffect('/ws2812/pixel_noise', this)">Pixel Noise</button>
      <button class="btn-effect" onclick="setEffect('/ws2812/glitch_columns', this)">Glitch Cols</button>
      <button class="btn-effect" onclick="setEffect('/ws2812/psychedelic_flow', this)">Psychedelic</button>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Швидке керування</div>
    <div class="control-row">
      <button class="btn-control" onclick="sendCmd('/ws2812/prev')">⏮ Назад</button>
      <button class="btn-control btn-off" onclick="sendCmd('/effects_fire/off')">⏹ ВИМК</button>
      <button class="btn-control" onclick="sendCmd('/ws2812/next')">Далі ⏭</button>
    </div>
  </div>

  <div class="toast" id="toast">Команду виконано!</div>

<script>
  function sendCmd(url) {
    fetch(url).then(() => showToast("Команду відправлено"));
  }

  function setEffect(url, btn) {
    // Прибираємо активний клас у всіх кнопок
    document.querySelectorAll('.btn-effect').forEach(b => b.classList.remove('active'));
    // Додаємо активний клас натиснутій кнопці
    btn.classList.add('active');
    fetch(url).then(() => showToast("Ефект змінено!"));
  }

  function setBrightness(val) {
    document.getElementById('brightVal').textContent = val;
    fetch('/brightness?value=' + val).then(() => showToast("Яскравість: " + val));
  }

  function showToast(msg) {
    const t = document.getElementById('toast');
    t.textContent = msg;
    t.classList.add('show');
    setTimeout(() => t.classList.remove('show'), 1500);
  }
</script>
</body>
</html>
)rawliteral";
// ==========================================
// 🌐 Красива HTML-сторінка статусу Wi-Fi
// ==========================================
const char WIFI_PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 • Wi-Fi Статус</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
    background: linear-gradient(135deg, #0f172a, #1e293b);
    min-height: 100vh;
    color: #e2e8f0;
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 20px;
  }
  .card {
    background: rgba(30, 41, 59, 0.7);
    backdrop-filter: blur(12px);
    border: 1px solid rgba(255, 255, 255, 0.1);
    border-radius: 20px;
    padding: 28px;
    width: 100%;
    max-width: 420px;
    box-shadow: 0 20px 50px rgba(0, 0, 0, 0.5);
  }
  .header { display: flex; align-items: center; gap: 12px; margin-bottom: 8px; }
  .header h1 { font-size: 22px; font-weight: 600; color: #fff; }
  .pulse {
    width: 10px; height: 10px; border-radius: 50%;
    background: #10b981; box-shadow: 0 0 0 rgba(16, 185, 129, 0.4);
    animation: pulse 2s infinite;
  }
  .pulse.off { background: #ef4444; box-shadow: 0 0 0 rgba(239, 68, 68, 0.4); }
  @keyframes pulse {
    0% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.4); }
    70% { box-shadow: 0 0 0 10px rgba(16, 185, 129, 0); }
    100% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0); }
  }
  .subtitle { font-size: 13px; color: #94a3b8; margin-bottom: 24px; }
  
  .row {
    display: flex; justify-content: space-between; align-items: center;
    padding: 12px 0; border-bottom: 1px solid rgba(255, 255, 255, 0.08);
  }
  .row:last-child { border-bottom: none; }
  .label { color: #94a3b8; font-size: 14px; }
  .value { font-family: "SF Mono", Menlo, monospace; font-size: 14px; font-weight: 500; color: #f8fafc; }
  
  .signal-container { display: flex; align-items: center; gap: 10px; }
  .signal-bars { display: flex; gap: 3px; align-items: flex-end; height: 16px; }
  .signal-bars span {
    width: 4px; background: #334155; border-radius: 2px; transition: background 0.3s;
  }
  .signal-bars span:nth-child(1) { height: 25%; }
  .signal-bars span:nth-child(2) { height: 50%; }
  .signal-bars span:nth-child(3) { height: 75%; }
  .signal-bars span:nth-child(4) { height: 100%; }
  .signal-bars.active span { background: #10b981; box-shadow: 0 0 6px #10b981; }
  
  .rssi-bar-bg {
    height: 6px; background: #334155; border-radius: 3px; overflow: hidden; margin-top: 8px;
  }
  .rssi-bar-fill {
    height: 100%; border-radius: 3px; transition: width 0.5s ease, background 0.5s ease;
    background: linear-gradient(90deg, #ef4444, #f59e0b, #10b981);
  }
  .footer { margin-top: 24px; text-align: center; font-size: 12px; color: #64748b; }
</style>
</head>
<body>
  <div class="card">
    <div class="header">
      <div class="pulse" id="statusDot"></div>
      <h1>Wi-Fi Статус</h1>
    </div>
    <div class="subtitle" id="modeText">Завантаження...</div>

    <div class="row">
      <span class="label">Мережа (SSID)</span>
      <span class="value" id="ssid">—</span>
    </div>
    <div class="row">
      <span class="label">IP-адреса</span>
      <span class="value" id="ip">—</span>
    </div>
    <div class="row">
      <span class="label">MAC-адреса</span>
      <span class="value" id="mac">—</span>
    </div>
    <div class="row">
      <span class="label">Сила сигналу</span>
      <div class="signal-container">
        <span class="value" id="rssiText">—</span>
        <div class="signal-bars" id="signalBars">
          <span></span><span></span><span></span><span></span>
        </div>
      </div>
    </div>
    <div class="rssi-bar-bg">
      <div class="rssi-bar-fill" id="rssiBar" style="width: 0%"></div>
    </div>

    <div class="footer">Оновлюється автоматично кожні 2 сек</div>
  </div>

<script>
  async function updateStatus() {
    try {
      const res = await fetch('/api/wifi');
      const data = await res.json();
      
      // Статус і пульсація
      const dot = document.getElementById('statusDot');
      const modeText = document.getElementById('modeText');
      if (data.state === 'Підключено') {
        dot.classList.remove('off');
        modeText.textContent = 'Режим: ' + data.mode;
      } else {
        dot.classList.add('off');
        modeText.textContent = 'Режим: ' + data.mode + ' (Немає з\'єднання)';
      }

      // Дані
      document.getElementById('ssid').textContent = data.ssid || 'Немає';
      document.getElementById('ip').textContent = data.ip;
      document.getElementById('mac').textContent = data.mac;
      document.getElementById('rssiText').textContent = data.rssi + ' dBm';

      // Візуалізація сигналу
      const rssi = parseInt(data.rssi);
      let bars = 0;
      if (rssi > -55) bars = 4;
      else if (rssi > -65) bars = 3;
      else if (rssi > -75) bars = 2;
      else if (rssi > -85) bars = 1;
      
      const barsEl = document.getElementById('signalBars');
      barsEl.className = 'signal-bars' + (bars > 0 ? ' active' : '');
      Array.from(barsEl.children).forEach((bar, i) => {
        bar.style.background = i < bars ? '#10b981' : '#334155';
        bar.style.boxShadow = i < bars ? '0 0 6px #10b981' : 'none';
      });

      // Шкала прогресу (від -90 до -30 dBm)
      let percent = Math.min(100, Math.max(0, (rssi + 90) * (100 / 60)));
      document.getElementById('rssiBar').style.width = percent + '%';

    } catch (e) {
      console.error('Помилка отримання даних', e);
    }
  }
  updateStatus();
  setInterval(updateStatus, 2000);
</script>
</body>
</html>
)rawliteral";

#endif