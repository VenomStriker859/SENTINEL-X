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


def test_duplicate_message_returns_conflict(client):
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


def test_invalid_message_is_rejected(client):
    response = client.post("/api/messages/", json={
        "device_id": "ESP32-001",
        "message_id": "msg-invalid",
        "message_type": "text",
        "content": "   ",
    })
    assert response.status_code == 422


def test_missing_message_returns_404(client):
    response = client.get("/api/messages/does-not-exist")
    assert response.status_code == 404
