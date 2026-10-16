# SentinelX Backend

FastAPI REST API for SentinelX messages, observed devices, and alerts.

## Run locally

From the repository root:

```bash
cd backend
python -m pip install -r requirements.txt
uvicorn app.main:app --reload
```

The API listens at `http://127.0.0.1:8000` by default.

### Interactive API documentation

- Swagger UI: `http://127.0.0.1:8000/docs`
- ReDoc: `http://127.0.0.1:8000/redoc`
- OpenAPI JSON: `http://127.0.0.1:8000/openapi.json`

## Endpoints

| Method | Path | Success | Purpose |
| --- | --- | --- | --- |
| GET | `/api/health` | 200 | Service status |
| POST | `/api/messages/` | 201 | Receive a text, image, or audio message |
| GET | `/api/messages/` | 200 | List messages held in memory |
| GET | `/api/messages/{message_id}` | 200 | Get one message; 404 if absent |
| GET | `/api/devices/` | 200 | List devices observed in incoming messages |
| GET | `/api/devices/{device_id}` | 200 | Get one device; 404 if absent |
| POST | `/api/alerts/` | 201 | Create an alert |
| GET | `/api/alerts/` | 200 | List alerts held in memory |
| GET | `/api/alerts/{alert_id}` | 200 | Get one alert; 404 if absent |
| PATCH | `/api/alerts/{alert_id}/resolve` | 200 | Resolve an alert; 404 if absent |

Invalid request bodies return HTTP 422. Duplicate message IDs return HTTP 409.

## Request examples

### Create a message

```json
{
  "device_id": "ESP32-001",
  "message_id": "msg-001",
  "message_type": "text",
  "content": "Emergency: need assistance"
}
```

Supported `message_type` values are `text`, `image`, and `audio`. IDs must be non-blank; `device_id` is limited to 50 characters and `message_id` to 100 characters. Text messages require non-blank content. Image/audio messages require a non-blank content reference. Each accepted message ID must be unique. A successful request registers/updates the device's last-seen time and received-message count.

### Create an alert

```json
{
  "device_id": "ESP32-001",
  "message": "Perimeter breach",
  "severity": "HIGH"
}
```

Allowed severity values are uppercase `LOW`, `MEDIUM`, `HIGH`, and `CRITICAL`; omitted severity defaults to `HIGH`. Device IDs are limited to 50 characters and alert messages to 500 characters. New alerts start with status `ACTIVE`; the resolve endpoint changes the status to `RESOLVED`.

## Automated tests

Run from the `backend/` directory:

```bash
python -m pip install -r requirements.txt
python -m pytest -q
```

Tests use FastAPI's in-process test client and clear message, device, and alert state between tests. They do not require a running server or database. The suite covers successful requests, empty lists, validation failures, duplicate messages, device registration/counts, and missing resources.

## Current limitations

Messages, devices, and alerts are held in process memory and are lost when the server restarts. A device appears in the device list after its first accepted message. The health endpoint indicates that the API process responds; it is not a readiness check for external services. These endpoints do not persist data.
