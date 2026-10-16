import { useEffect, useState } from "react";
import "./App.css";

const LOGO_PATH = "/sentinelnet-logo.png";

const initialNodes = [
  {
    id: "NODE 01",
    battery: 92,
    signal: -65,
    status: "ONLINE",
    icon: "⌘",
  },
  {
    id: "NODE 02",
    battery: 78,
    signal: -72,
    status: "ONLINE",
    icon: "◉",
  },
  {
    id: "NODE 03",
    battery: 64,
    signal: -81,
    status: "ONLINE",
    icon: "⌖",
  },
  {
    id: "NODE 04",
    battery: 35,
    signal: -96,
    status: "WEAK SIGNAL",
    icon: "◇",
  },
];

const initialNotifications = [
  {
    id: "text",
    title: "Text Messages",
    shortTitle: "TEXT",
    icon: "✉",
    count: 2,
    description: "Text communication between connected nodes.",
  },
  {
    id: "images",
    title: "Images",
    shortTitle: "IMAGES",
    icon: "▧",
    count: 3,
    description: "Images received through the communication network.",
  },
  {
    id: "audio",
    title: "Audio",
    shortTitle: "AUDIO",
    icon: "♫",
    count: 1,
    description: "Audio communication received from field units.",
  },
  {
    id: "gps",
    title: "GPS Locations",
    shortTitle: "GPS",
    icon: "◎",
    count: 4,
    description: "Location records associated with connected nodes.",
  },
];

const activityItems = [
  {
    type: "TEXT",
    title: "Text message received",
    detail: "Incoming communication · NODE-02",
    time: "Recent",
  },
  {
    type: "GPS",
    title: "Location update",
    detail: "Position record · NODE-03",
    time: "Recent",
  },
  {
    type: "IMAGE",
    title: "Image record available",
    detail: "Media notification · NODE-01",
    time: "Recent",
  },
];

/* ==========================================
   REUSABLE SENTINELNET BRAND
========================================== */

function Brand() {
  return (
    <div className="brand">
      <div className="brand-symbol">
        <img
          src={LOGO_PATH}
          alt="SentinelNet logo"
          className="brand-logo"
        />
      </div>

      <div className="brand-text">
        <h1>
          SENTINEL<span>NET</span>
        </h1>

        <p>TACTICAL COMMUNICATION SYSTEM</p>
      </div>
    </div>
  );
}

/* ==========================================
   LOGIN PAGE
========================================== */

function LoginPage({ onLogin }) {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [captcha, setCaptcha] = useState("");
  const [captchaCode, setCaptchaCode] = useState("SNT482");
  const [showPassword, setShowPassword] = useState(false);
  const [message, setMessage] = useState("");

  function refreshCaptcha() {
    const characters = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    let code = "";

    for (let i = 0; i < 6; i++) {
      code += characters.charAt(
        Math.floor(Math.random() * characters.length)
      );
    }

    setCaptchaCode(code);
    setCaptcha("");
    setMessage("");
  }

  function handleLogin(event) {
    event.preventDefault();

    if (!username.trim() || !password || !captcha.trim()) {
      setMessage("Please complete all fields.");
      return;
    }

    if (captcha.trim().toUpperCase() !== captchaCode) {
      setMessage("Incorrect CAPTCHA. Please try again.");
      refreshCaptcha();
      return;
    }

    /*
      Demonstration login only.
      Real authentication must be implemented by the backend.
    */
    onLogin(username.trim());
  }

  return (
    <main className="login-page">
      {/* Moving video background */}
      <video
        className="background-video"
        autoPlay
        muted
        loop
        playsInline
        preload="auto"
        aria-hidden="true"
      >
        <source src="/soldiers.mp4" type="video/mp4" />
      </video>

      <div className="video-overlay" aria-hidden="true" />

      {/* Header */}
      <header className="system-header">
        <Brand />

        <div className="system-status">
          <span className="status-dot" />
          SYSTEM INTERFACE
        </div>
      </header>

      {/* Login form */}
      <section className="login-container">
        <div className="login-card">
          <div className="card-top-line" />

          <div className="login-heading">
            <div className="access-icon">⌘</div>

            <p className="eyebrow">SECURE ACCESS TERMINAL</p>

            <h2>
              Welcome<span>.</span>
            </h2>

            <p className="subtitle">
              Authenticate to access the command interface.
            </p>
          </div>

          <form onSubmit={handleLogin}>
            <label htmlFor="username">USERNAME</label>

            <div className="input-wrapper">
              <span className="input-icon">⌁</span>

              <input
                id="username"
                type="text"
                placeholder="Enter your username"
                value={username}
                onChange={(event) => setUsername(event.target.value)}
                autoComplete="username"
              />
            </div>

            <label htmlFor="password">PASSWORD</label>

            <div className="input-wrapper">
              <span className="input-icon">⌑</span>

              <input
                id="password"
                type={showPassword ? "text" : "password"}
                placeholder="Enter your password"
                value={password}
                onChange={(event) => setPassword(event.target.value)}
                autoComplete="current-password"
              />

              <button
                className="password-toggle"
                type="button"
                onClick={() => setShowPassword(!showPassword)}
              >
                {showPassword ? "HIDE" : "SHOW"}
              </button>
            </div>

            <label htmlFor="captcha">SECURITY VERIFICATION</label>

            <div className="captcha-row">
              <div className="captcha-code">{captchaCode}</div>

              <button
                className="refresh-captcha"
                type="button"
                onClick={refreshCaptcha}
                aria-label="Refresh CAPTCHA"
              >
                ↻
              </button>

              <input
                id="captcha"
                type="text"
                placeholder="Enter code"
                value={captcha}
                onChange={(event) => setCaptcha(event.target.value)}
                autoComplete="off"
              />
            </div>

            {message && (
              <p className="login-message error" role="alert">
                {message}
              </p>
            )}

            <button className="login-button" type="submit">
              <span>AUTHENTICATE</span>
              <span className="button-arrow">→</span>
            </button>
          </form>

          <div className="card-footer">
            <span className="footer-indicator" />
            AUTHORIZED PERSONNEL ONLY
          </div>
        </div>

        <p className="security-note">
          SENTINELNET <span>•</span> SECURE COMMUNICATION INTERFACE
        </p>
      </section>

      {/* Login footer */}
      <footer className="system-footer">
        <span>TACTICAL NETWORK</span>
        <span>SECURE ACCESS PORTAL</span>
        <span>INTERFACE v1.0</span>
      </footer>
    </main>
  );
}

/* ==========================================
   DASHBOARD
========================================== */

function Dashboard({ username, onLogout }) {
  const [selectedCategory, setSelectedCategory] = useState(null);
  const [selectedNode, setSelectedNode] = useState(null);

  const selectedNotification = initialNotifications.find(
    (item) => item.id === selectedCategory
  );

  return (
    <main className="dashboard-page">
      {/* Dashboard header */}
      <header className="dashboard-header">
        <Brand />

        <div className="dashboard-header-right">
          <div className="connection-status">
            <span className="status-dot" />
            NETWORK MONITORING
          </div>

          <div className="user-profile">
            <div className="user-avatar">
              {username.charAt(0).toUpperCase()}
            </div>

            <span>{username}</span>
          </div>

          <button className="logout-button" onClick={onLogout}>
            LOGOUT ↗
          </button>
        </div>
      </header>

      <div className="dashboard-content">
        {/* Dashboard heading */}
        <section className="dashboard-welcome">
          <div>
            <p className="eyebrow">
              COMMAND &amp; CONTROL / OVERVIEW
            </p>

            <h2>
              Network <span>Dashboard</span>
            </h2>

            <p className="dashboard-subtitle">
              Monitor your nodes and communication activity from one place.
            </p>
          </div>

          <div className="overview-badge">
            <span className="status-dot" />
            LOCAL DASHBOARD
          </div>
        </section>

        {/* Notification cards */}
        <section className="notification-grid">
          {initialNotifications.map((item) => (
            <button
              className={`notification-card ${
                selectedCategory === item.id ? "selected" : ""
              }`}
              key={item.id}
              onClick={() =>
                setSelectedCategory(
                  selectedCategory === item.id ? null : item.id
                )
              }
              aria-pressed={selectedCategory === item.id}
            >
              <div className="notification-card-top">
                <span className="notification-icon">{item.icon}</span>
                <span className="notification-arrow">↗</span>
              </div>

              <div className="notification-count">{item.count}</div>

              <div className="notification-title">{item.title}</div>

              <div className="notification-card-bottom">
                <span>{item.shortTitle} NOTIFICATIONS</span>
                <span className="mini-indicator" />
              </div>
            </button>
          ))}
        </section>

        {/* Dashboard main layout */}
        <div className="dashboard-layout">
          {/* LEFT SIDEBAR: NODES */}
          <aside className="panel nodes-panel">
            <div className="panel-heading">
              <div>
                <p className="panel-kicker">NETWORK</p>
                <h3>Connected Nodes</h3>
              </div>

              <span className="panel-count">04</span>
            </div>

            <div className="node-list">
              {initialNodes.map((node) => (
                <button
                  key={node.id}
                  className={`node-card ${
                    selectedNode === node.id ? "node-selected" : ""
                  }`}
                  onClick={() =>
                    setSelectedNode(
                      selectedNode === node.id ? null : node.id
                    )
                  }
                  aria-pressed={selectedNode === node.id}
                >
                  <div className="node-card-heading">
                    <div
                      className={`node-icon-circle ${
                        node.status === "WEAK SIGNAL" ? "node-weak" : ""
                      }`}
                    >
                      <span>{node.icon}</span>
                      <i className="node-icon-status" />
                    </div>

                    <div className="node-heading-info">
                      <span className="node-id">{node.id}</span>

                      <span
                        className={`node-status ${
                          node.status === "ONLINE" ? "online" : "warning"
                        }`}
                      >
                        {node.status}
                      </span>
                    </div>

                    <span className="node-chevron">
                      {selectedNode === node.id ? "−" : "+"}
                    </span>
                  </div>

                  {/* Battery */}
                  <div className="node-metric">
                    <div className="metric-label">
                      <span>
                        <span className="metric-icon">▰</span>
                        BATTERY
                      </span>

                      <span>{node.battery}%</span>
                    </div>

                    <div className="progress-track">
                      <div
                        className={`progress-fill ${
                          node.battery < 40 ? "low-battery" : ""
                        }`}
                        style={{ width: `${node.battery}%` }}
                      />
                    </div>
                  </div>

                  {/* Signal strength */}
                  <div className="node-signal">
                    <span>
                      <span className="metric-icon">⌁</span>
                      SIGNAL STRENGTH
                    </span>

                    <span>{node.signal} dBm</span>
                  </div>
                </button>
              ))}
            </div>

            <div className="nodes-footer">
              <span className="status-dot" />
              DEMONSTRATION DATA
            </div>
          </aside>

          {/* RIGHT-SIDE CONTENT */}
          <div className="dashboard-main-column">
            {/* Selected notification details */}
            {selectedNotification && (
              <section className="panel category-detail">
                <div className="panel-heading">
                  <div>
                    <p className="panel-kicker">COMMUNICATION MODULE</p>
                    <h3>{selectedNotification.title}</h3>
                  </div>

                  <button
                    className="close-detail"
                    onClick={() => setSelectedCategory(null)}
                    aria-label="Close details"
                  >
                    ×
                  </button>
                </div>

                <p>{selectedNotification.description}</p>

                <div className="detail-placeholder">
                  <span className="detail-icon">
                    {selectedNotification.icon}
                  </span>

                  <div>
                    <strong>
                      {selectedNotification.count} notification
                      {selectedNotification.count === 1 ? "" : "s"}
                    </strong>

                    <p>
                      Detailed records will appear here after backend
                      integration.
                    </p>
                  </div>
                </div>
              </section>
            )}

            {/* Network overview */}
            <section className="panel network-panel">
              <div className="panel-heading">
                <div>
                  <p className="panel-kicker">SYSTEM STATUS</p>
                  <h3>Network Overview</h3>
                </div>

                <span className="live-label">
                  <span className="status-dot" />
                  DEMO
                </span>
              </div>

              <div className="network-visual">
                <div className="network-orbit orbit-one" />
                <div className="network-orbit orbit-two" />
                <div className="network-orbit orbit-three" />

                {/* New SentinelNet logo in the network hub */}
                <div className="network-center">
                  <div className="network-center-symbol">
                    <img
                      src={LOGO_PATH}
                      alt="SentinelNet communication hub"
                      className="network-center-logo"
                    />
                  </div>

                  <strong>SENTINELNET</strong>
                  <span>COMMUNICATION HUB</span>
                </div>

                <div className="network-node network-node-one">
                  <span />
                  NODE 01
                </div>

                <div className="network-node network-node-two">
                  <span />
                  NODE 02
                </div>

                <div className="network-node network-node-three">
                  <span />
                  NODE 03
                </div>

                <div className="network-node network-node-four">
                  <span />
                  NODE 04
                </div>
              </div>

              <div className="network-legend">
                <span>
                  <i className="legend-dot green-dot" />
                  Connected
                </span>

                <span>
                  <i className="legend-dot amber-dot" />
                  Weak signal
                </span>

                <span>4 REGISTERED NODES</span>
              </div>
            </section>

            {/* Recent activity */}
            <section className="panel activity-panel">
              <div className="panel-heading">
                <div>
                  <p className="panel-kicker">COMMUNICATION LOG</p>
                  <h3>Recent Activity</h3>
                </div>

                <span className="activity-count">03 RECORDS</span>
              </div>

              <div className="activity-list">
                {activityItems.map((item) => (
                  <div className="activity-item" key={item.type}>
                    <div className="activity-marker">{item.type}</div>

                    <div className="activity-description">
                      <strong>{item.title}</strong>
                      <p>{item.detail}</p>
                    </div>

                    <span className="activity-time">{item.time}</span>
                  </div>
                ))}
              </div>

              <p className="demo-notice">
                Sample interface records — actual data will come from the
                backend.
              </p>
            </section>
          </div>

          {/* FULL-WIDTH ABOUT SECTION */}
          <section className="panel project-panel full-width-section">
            <p className="panel-kicker">ABOUT THE PROJECT</p>

            <h3>
              Built for <span>Connected Communication.</span>
            </h3>

            <p>
              SentinelNet is a LoRa-based communication and monitoring
              project designed to support messaging, location awareness,
              and node-status monitoring when conventional connectivity
              is limited.
            </p>

            <div className="project-tags">
              <span>LoRa</span>
              <span>IoT</span>
              <span>GPS</span>
              <span>ESP32</span>
            </div>
          </section>

          {/* FULL-WIDTH PROJECT TEAM SECTION */}
          <section className="panel team-panel full-width-section">
            <div className="team-icon">⌘</div>

            <div className="team-description">
              <p className="panel-kicker">PROJECT TEAM</p>
              <h3>Team SentinelNet</h3>

              <p>
                Six-member engineering team developing a connected
                communication and monitoring prototype.
              </p>
            </div>

            <div className="team-member-count">
              <strong>06</strong>
              <span>MEMBERS</span>
            </div>
          </section>
        </div>

        {/* Dashboard footer */}
        <footer className="dashboard-footer">
          <span>SENTINELNET © 2026</span>
          <span>LORA-BASED COMMUNICATION SYSTEM</span>
          <span>FRONTEND PROTOTYPE v1.0</span>
        </footer>
      </div>
    </main>
  );
}

/* ==========================================
   MAIN APP
========================================== */

function NotFoundPage({ onGoHome }) {
  return (
    <main className="not-found-page">
      <div className="not-found-glow not-found-glow-one" />
      <div className="not-found-glow not-found-glow-two" />

      {/* Header */}
      <header className="not-found-header">
        <div className="brand">
          <img
            src="/sentinelnet-logo.png"
            alt="SentinelNet Logo"
            className="brand-logo-image"
          />

          <div className="brand-text">
            <h1>
              SENTINEL<span>NET</span>
            </h1>
            <p>TACTICAL COMMUNICATION SYSTEM</p>
          </div>
        </div>

        <div className="system-status">
          <span className="status-dot" />
          SYSTEM INTERFACE
        </div>
      </header>

      {/* 404 Content */}
      <section className="not-found-content">
        <div className="not-found-card">
          <div className="not-found-code">
            <span>4</span>
            <span className="not-found-zero">0</span>
            <span>4</span>
          </div>

          <div className="not-found-divider">
            <span />
          </div>

          <p className="eyebrow">
            SIGNAL LOST / ROUTE NOT FOUND
          </p>

          <h2>
            Oops! <span>You're off the grid.</span>
          </h2>

          <p className="not-found-description">
            The page you're looking for could not be located.
            It may have been moved, removed, or the address
            may be incorrect.
          </p>

          <div className="not-found-status">
            <span className="status-dot warning-dot" />
            DESTINATION UNAVAILABLE
          </div>

          <button
            type="button"
            className="not-found-home-button"
            onClick={onGoHome}
          >
            <span>RETURN TO COMMAND CENTER</span>
            <span className="button-arrow">→</span>
          </button>

          <div className="not-found-footer">
            <span>ERROR CODE: 404</span>
            <span>SENTINELNET NETWORK</span>
          </div>
        </div>
      </section>

      {/* Footer */}
      <footer className="not-found-bottom">
        <span>SENTINELNET © 2026</span>
        <span>SECURE COMMUNICATION INTERFACE</span>
        <span>FRONTEND PROTOTYPE v1.0</span>
      </footer>
    </main>
  );
}

function App() {
  const [loggedInUser, setLoggedInUser] = useState("");

  const [currentPath, setCurrentPath] = useState(
    window.location.pathname
  );

  useEffect(() => {
    function handleLocationChange() {
      setCurrentPath(window.location.pathname);
    }

    window.addEventListener("popstate", handleLocationChange);

    return () => {
      window.removeEventListener(
        "popstate",
        handleLocationChange
      );
    };
  }, []);

  function handleLogout() {
    setLoggedInUser("");
    window.history.pushState({}, "", "/");
    setCurrentPath("/");
  }

  function handleGoHome() {
    window.history.pushState({}, "", "/");
    setCurrentPath("/");
  }

  // Display the custom 404 page for unknown routes.
  if (currentPath !== "/") {
    return <NotFoundPage onGoHome={handleGoHome} />;
  }

  // Display the dashboard after login.
  if (loggedInUser) {
    return (
      <Dashboard
        username={loggedInUser}
        onLogout={handleLogout}
      />
    );
  }

  // Otherwise, display the login page.
  return <LoginPage onLogin={setLoggedInUser} />;
}

export default App;