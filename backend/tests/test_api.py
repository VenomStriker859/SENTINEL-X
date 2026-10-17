import pytest
from fastapi.testclient import TestClient

from app.main import app
from app.services.message_service import message_service
from app.services.device_manager import device_manager
from app.services.alert_manager import alert_manager

client = TestClient(app)


@pytest.fixture(autouse=True)
def reset_state():
    message_service.messages.clear()
    message_service.processed_ids.clear()
    device_manager.devices.clear()
    alert_manager.alerts.clear()


def test_health_endpoint():
    response = client.get("/api/health")

    assert response.status_code == 200
    assert response.json()["project"] == "SentinelX"
    assert response.json()["status"] == "online"


def test_receive_text_message():
    payload = {
        "device_id": "device-test-1",
        "message_id": "msg-test-1",
        "message_type": "text",
        "content": "Test message"
    }

    response = client.post("/api/messages/", json=payload)

    assert response.status_code == 200
    assert response.json()["success"] is True
    assert response.json()["message"]["message_id"] == "msg-test-1"


def test_duplicate_message_rejected():
    payload = {
        "device_id": "device-test-1",
        "message_id": "duplicate-test",
        "message_type": "text",
        "content": "Hello"
    }

    first = client.post("/api/messages/", json=payload)
    second = client.post("/api/messages/", json=payload)

    assert first.status_code == 200
    assert second.status_code == 409
    assert second.json()["detail"] == "Duplicate message"


def test_empty_text_message_rejected():
    payload = {
        "device_id": "device-test-1",
        "message_id": "empty-test",
        "message_type": "text",
        "content": "   "
    }

    response = client.post("/api/messages/", json=payload)

    assert response.status_code == 422


def test_message_not_found():
    response = client.get("/api/messages/does-not-exist")

    assert response.status_code == 404


def test_device_registered_after_message():
    payload = {
        "device_id": "device-test-2",
        "message_id": "msg-device-test",
        "message_type": "text",
        "content": "Register device"
    }

    client.post("/api/messages/", json=payload)

    response = client.get("/api/devices/device-test-2")

    assert response.status_code == 200

    device = response.json()
    assert device["device_id"] == "device-test-2"
    assert device["status"] == "ONLINE"
    assert device["messages_received"] == 1


def test_device_not_found():
    response = client.get("/api/devices/unknown-device")

    assert response.status_code == 404


def test_create_alert():
    payload = {
        "device_id": "device-alert-1",
        "message": "Test intrusion alert",
        "severity": "HIGH"
    }

    response = client.post("/api/alerts/", json=payload)

    assert response.status_code == 200

    alert = response.json()
    assert alert["device_id"] == "device-alert-1"
    assert alert["message"] == "Test intrusion alert"
    assert alert["severity"] == "HIGH"
    assert alert["status"] == "ACTIVE"
    assert "alert_id" in alert


def test_invalid_alert_severity():
    payload = {
        "device_id": "device-alert-1",
        "message": "Invalid severity test",
        "severity": "EXTREME"
    }

    response = client.post("/api/alerts/", json=payload)

    assert response.status_code == 400


def test_alert_not_found():
    response = client.get("/api/alerts/unknown-alert")

    assert response.status_code == 404


def test_resolve_alert():
    payload = {
        "device_id": "device-alert-2",
        "message": "Resolve this alert",
        "severity": "CRITICAL"
    }

    created = client.post("/api/alerts/", json=payload)
    alert_id = created.json()["alert_id"]

    response = client.patch(f"/api/alerts/{alert_id}/resolve")

    assert response.status_code == 200
    assert response.json()["status"] == "RESOLVED"


def test_resolve_missing_alert():
    response = client.patch("/api/alerts/unknown-alert/resolve")

    assert response.status_code == 404