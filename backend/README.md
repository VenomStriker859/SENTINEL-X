# SentinelX Backend

FastAPI REST API for SentinelX messages, observed devices, and alerts.

## Run locally

From the repository root:

```bash
cd backend
python -m pip install -r requirements.txt
uvicorn app.main:app --reload
```

Interactive API documentation:
- Swagger UI: `http://127.0.0.1:8000/docs`
- ReDoc: `http://127.0.0.1:8000/redoc`
- OpenAPI JSON: `http://127.0.0.1:8000/openapi.json`

## Endpoints

| Method | Path | Purpose |
| --- | --- | --- |
| GET | `/api/health` | Service status |
| POST | `/api/messages/` | Receive a text, image, or audio message |
| GET | `/api/messages/` | List messages |
| GET | `/api/messages/{message_id}` | Get one message |
| GET | `/api/devices/` | List devices observed in incoming messages |
| GET | `/api/devices/{device_id}` | Get one device |
| POST | `/api/alerts/` | Create an alert |
| GET | `/api/alerts/` | List alerts |
| GET | `/api/alerts/{alert_id}` | Get one alert |
| PATCH | `/api/alerts/{alert_id}/resolve` | Resolve an alert |

### Message example

```json
{
  "device_id": "ESP32-001",
  "message_id": "msg-001",
  "message_type": "text",
  "content": "Emergency: need assistance"
}
```

Supported `message_type` values are `text`, `image`, and `audio`. Text messages require non-blank content; image/audio messages require a content reference. Reusing a `message_id` returns HTTP 409.

### Alert example

```json
{
  "device_id": "ESP32-001",
  "message": "Perimeter breach",
  "severity": "HIGH"
}
```

Allowed severities: `LOW`, `MEDIUM`, `HIGH`, `CRITICAL`. Severity defaults to `HIGH`. Creating a valid alert returns HTTP 201. Invalid payloads return HTTP 422; missing message/device/alert IDs return HTTP 404 where applicable.

## Tests

Run from the `backend/` directory:

```bash
python -m pytest -q
```

Tests use FastAPI's in-process test client and clear the in-memory message, device, and alert state between tests. They do not require a running server or database.

## Current limitations

Messages, devices, and alerts are currently held in process memory and are lost when the server restarts. A device appears in the device list after its first accepted message. The health endpoint indicates that the API process responds; it is not a readiness check for external services.
