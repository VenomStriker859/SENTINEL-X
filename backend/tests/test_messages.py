import pytest


def test_create_list_and_get_message(client):
    payload = {
        "device_id": "ESP32-001",
        "message_id": "msg-001",
        "message_type": "text",
        "content": "Test message",
    }

    created = client.post("/api/messages/", json=payload)
    assert created.status_code == 201
    assert created.json()["success"] is True
    assert created.json()["message"]["message_id"] == "msg-001"

    listed = client.get("/api/messages/")
    assert listed.status_code == 200
    assert len(listed.json()["messages"]) == 1

    fetched = client.get("/api/messages/msg-001")
    assert fetched.status_code == 200
    assert fetched.json()["content"] == "Test message"


def test_empty_message_list(client):
    response = client.get("/api/messages/")
    assert response.status_code == 200
    assert response.json() == {"messages": []}


def test_duplicate_message_returns_conflict_without_double_counting(client):
    payload = {
        "device_id": "ESP32-001",
        "message_id": "msg-duplicate",
        "message_type": "text",
        "content": "Hello",
    }
    assert client.post("/api/messages/", json=payload).status_code == 201
    response = client.post("/api/messages/", json=payload)
    assert response.status_code == 409
    assert response.json()["detail"] == "Duplicate message"
    assert client.get("/api/devices/ESP32-001").json()["messages_received"] == 1


def test_invalid_message_is_rejected(client):
    response = client.post("/api/messages/", json={
        "device_id": "ESP32-001",
        "message_id": "msg-invalid",
        "message_type": "text",
        "content": "   ",
    })
    assert response.status_code == 422


@pytest.mark.parametrize("field", ["device_id", "message_id"])
def test_blank_identifiers_are_rejected(client, field):
    payload = {
        "device_id": "ESP32-001",
        "message_id": "msg-blank",
        "message_type": "text",
        "content": "Hello",
    }
    payload[field] = "   "
    assert client.post("/api/messages/", json=payload).status_code == 422


@pytest.mark.parametrize("message_type", ["image", "audio"])
def test_media_message_requires_content_reference(client, message_type):
    response = client.post("/api/messages/", json={
        "device_id": "ESP32-001",
        "message_id": f"msg-{message_type}",
        "message_type": message_type,
        "content": "   ",
    })
    assert response.status_code == 422


def test_unsupported_message_type_is_rejected(client):
    response = client.post("/api/messages/", json={
        "device_id": "ESP32-001",
        "message_id": "msg-video",
        "message_type": "video",
        "content": "clip.mp4",
    })
    assert response.status_code == 422


def test_missing_message_returns_404(client):
    response = client.get("/api/messages/does-not-exist")
    assert response.status_code == 404
