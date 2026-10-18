::: {align="center"}
# 🛡️ SENTINEL-X

### Real-Time Tactical Communication & Situational Awareness

**Connecting field devices, backend services, and live dashboards in one
communication platform.**

```{=html}
<p>
```
`<img src="https://img.shields.io/badge/Status-Active%20Development-orange?style=for-the-badge" alt="Project status">`{=html}
`<img src="https://img.shields.io/badge/Python-3.12%2B-blue?style=for-the-badge&logo=python&logoColor=white" alt="Python">`{=html}
`<img src="https://img.shields.io/badge/FastAPI-Backend-009688?style=for-the-badge&logo=fastapi&logoColor=white" alt="FastAPI">`{=html}
`<img src="https://img.shields.io/badge/WebSockets-Realtime-6A5ACD?style=for-the-badge" alt="WebSockets">`{=html}
```{=html}
</p>
```
**A team-built project under active development.** Features marked as
planned are not yet complete.
:::

------------------------------------------------------------------------

## 🚀 What is Sentinel-X?

**Sentinel-X** is a real-time communication and situational-awareness
project designed to connect ESP32-based field devices, LoRa
communication, a FastAPI backend, and a web dashboard.

The goal is to move field information from device to dashboard with a
clear, modular architecture---supporting messages, device information,
alerts, and eventually media such as images and audio.

> **Project vision:** Field device → communication link → backend API →
> real-time events → dashboard.

## ✨ Core Capabilities

-   **⚡ REST API:** HTTP endpoints for messages, devices, and alerts.
-   **🔌 WebSocket connection:** A real-time channel at `/ws`, including
    connection events and ping/pong handling.
-   **🚨 Alert handling:** Create, retrieve, and resolve alerts through
    the backend API.
-   **📡 Hardware integration path:** Designed to integrate ESP32 and
    LoRa field communication through a separate serial-communication
    component.
-   **🧪 Automated tests:** Backend and WebSocket tests help catch
    regressions as the project evolves.
-   **🧩 Modular design:** API routes, schemas, services, database
    components, and WebSocket logic are separated into dedicated
    modules.

### 🛠️ In progress

-   ESP32/LoRa serial communication bridge.
-   Frontend integration with REST endpoints and WebSocket events.
-   Database integration and persistence verification.
-   Image and audio upload, storage, retrieval, and event notifications.
-   Full end-to-end testing across hardware, backend, and frontend.

These are development goals, not claims that the entire pipeline is
already operational.

## 🏗️ Architecture

``` mermaid
flowchart LR
    A["ESP32 / Field Device"] <-->|"LoRa"| B["LoRa Receiver"]
    B <-->|"Serial bridge · planned"| C["FastAPI Backend"]
    C --> D["REST API"]
    C <--> E["WebSocket · /ws"]
    D <--> F["Web Dashboard · integration in progress"]
    E <--> F
    C <--> G["Database · integration in progress"]
    C -. "Planned media support" .-> H["Image & Audio Storage"]
```

### Intended data flow

1.  A field device produces a message or event.
2.  The receiver obtains the data over the device communication link.
3.  A serial bridge passes the data to the backend (integration in
    progress).
4.  FastAPI exposes API operations and WebSocket events.
5.  The dashboard consumes API responses and real-time events.
6.  Planned media support will add controlled image/audio upload and
    retrieval.

## 🧰 Tech Stack

  -----------------------------------------------------------------------
  Layer                   Technology              Role
  ----------------------- ----------------------- -----------------------
  Backend API             Python, FastAPI         HTTP endpoints and
                                                  application logic

  Real-time channel       WebSockets              Live client connections
                                                  and event messages

  Data validation         Pydantic                Request/response data
                                                  models

  ASGI server             Uvicorn                 Run the FastAPI
                                                  application

  Hardware communication  ESP32 + LoRa            Field-device
                                                  communication;
                                                  integration in progress

  Frontend                Team web client         Dashboard integration
                                                  in progress

  Testing                 pytest, FastAPI         Automated backend/API
                          TestClient              checks
  -----------------------------------------------------------------------

## 📂 Project Structure

The repository may evolve as the team integrates the remaining
components. The main areas include:

``` text
SENTINEL-X/
├── backend/
│   ├── app/
│   │   ├── api/
│   │   │   └── routes/
│   │   │       ├── alerts.py
│   │   │       ├── devices.py
│   │   │       └── messages.py
│   │   ├── schemas/
│   │   ├── services/
│   │   └── main.py
│   ├── tests/
│   └── requirements.txt
├── ws/
│   ├── manager.py
│   ├── events.py
│   └── tests/
├── .github/
│   └── workflows/
│       └── backend-tests.yml
└── README.md
```

*This is a guide to the main code areas, not an exhaustive file
listing.*

## ⚙️ Getting Started

### Prerequisites

-   Python 3.12 recommended for the current CI workflow.
-   Git.
-   A terminal such as PowerShell, Command Prompt, or a Unix shell.

### 1. Clone the repository

``` bash
git clone https://github.com/VenomStriker859/SENTINEL-X.git
cd SENTINEL-X
```

### 2. Create and activate a virtual environment

**Windows PowerShell**

``` powershell
py -3.12 -m venv .venv
.\.venv\Scripts\Activate.ps1
```

**Linux / macOS**

``` bash
python3.12 -m venv .venv
source .venv/bin/activate
```

### 3. Install backend dependencies

``` bash
python -m pip install --upgrade pip
python -m pip install -r backend/requirements.txt
python -m pip install pytest httpx
```

### 4. Run the backend

From the repository root:

``` bash
python -m uvicorn app.main:app --app-dir backend --reload --host 127.0.0.1 --port 8000
```

When the server starts, open:

-   **API base:** `http://127.0.0.1:8000`
-   **Interactive API documentation:** `http://127.0.0.1:8000/docs`
-   **Alternative API documentation:** `http://127.0.0.1:8000/redoc`
-   **WebSocket endpoint:** `ws://127.0.0.1:8000/ws`

The documentation pages list the routes exposed by the running version
of the application.

## 🔌 API Overview

The backend currently organizes its HTTP routes around these areas:

  -----------------------------------------------------------------------
  Area                                Purpose
  ----------------------------------- -----------------------------------
  Health                              Check whether the API is responding

  Messages                            Create and retrieve messages;
                                      handle duplicate-message responses

  Devices                             Access device-related API
                                      operations

  Alerts                              Create, retrieve, and resolve
                                      alerts

  WebSocket                           Maintain a real-time connection and
                                      exchange supported events
  -----------------------------------------------------------------------

For the exact paths, request schemas, and response examples, use the
running **`/docs`** page. Do not assume that proposed image/audio
endpoints exist until they have been implemented and documented.

### WebSocket

Connect to:

``` text
ws://127.0.0.1:8000/ws
```

The event format used by the WebSocket package is structured around:

``` json
{
  "event": "event_name",
  "data": {},
  "timestamp": "UTC timestamp"
}
```

Supported event names in the current WebSocket implementation include
`connected`, `pong`, `error`, `new_message`, and `alert`. The precise
payload depends on the event. A client can send a ping message to
exercise the ping/pong flow.

**Note:** API-triggered broadcasts are covered by backend tests using a
mocked broadcast manager. Verify the complete live REST-to-WebSocket
path during end-to-end testing.

## 🧪 Run Tests

From the repository root, activate your virtual environment and run:

**Windows PowerShell**

``` powershell
$env:PYTHONPATH = "$PWD\backend;$PWD"
python -m pytest -vv backend\tests ws\tests
```

**Linux / macOS**

``` bash
PYTHONPATH="backend:." python -m pytest -vv backend/tests ws/tests
```

The current backend and WebSocket test suite has passed **27 tests** in
the development environment. Results may change as new commits are
added; run the tests locally or check the latest GitHub Actions run for
current status.

The repository includes a GitHub Actions workflow for running backend
tests on relevant pushes and pull requests to `tester/rough`.

## 🗺️ Roadmap

-   [x] FastAPI application and core REST route groups
-   [x] WebSocket connection and event manager
-   [x] Automated backend and WebSocket tests
-   [x] CI workflow for backend tests
-   [ ] Complete database integration and verify persistence
-   [ ] Complete ESP32/LoRa serial bridge
-   [ ] Connect frontend to REST APIs and WebSocket events
-   [ ] Implement image upload, storage, and retrieval
-   [ ] Implement audio upload, storage, and retrieval
-   [ ] Add media metadata and notifications
-   [ ] Run complete hardware-to-dashboard end-to-end tests
-   [ ] Review authentication, authorization, and production deployment
    requirements

## 🤝 Contributing

Sentinel-X is being developed collaboratively. To keep changes easy to
review:

1.  Coordinate with the team before modifying shared components.
2.  Create a separate branch for your work.
3.  Keep changes focused and avoid duplicating existing API or WebSocket
    logic.
4.  Add or update tests for behavior changes.
5.  Run the test suite before opening a pull request.
6.  Document new endpoints, event payloads, configuration, and setup
    steps.
7.  Do not commit credentials, `.env` files, private keys, virtual
    environments, or generated artifacts.

## 🔐 Security & Deployment

Sentinel-X is under active development and should **not be considered
production-ready** without further review. Before deployment, the team
should evaluate authentication, authorization, input validation, rate
limiting, secure media storage, logging, secrets management, and network
transport security.

Do not expose a development server directly to the public internet.

## 👥 Team & Project

Built collaboratively as a hardware-and-software integration project.

-   **Repository:**
    [VenomStriker859/SENTINEL-X](https://github.com/VenomStriker859/SENTINEL-X)
-   **Issues and contributions:** Use the repository's Issues and Pull
    Requests to coordinate work.

------------------------------------------------------------------------

::: {align="center"}
### 🛡️ SENTINEL-X

**Connecting devices. Delivering signals. Building situational
awareness.**

*Built with Python, FastAPI, WebSockets, ESP32, and LoRa.*
:::
