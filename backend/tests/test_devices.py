def test_device_is_registered_by_incoming_message(client):
    response = client.post("/api/messages/", json={
        "device_id": "ESP32-001",
        "message_id": "device-test-msg",
        "message_type": "text",
        "content": "Ping",
    })
    assert response.status_code == 201

    listed = client.get("/api/devices/")
    assert listed.status_code == 200
    devices = listed.json()["devices"]
    assert len(devices) == 1
    assert devices[0]["device_id"] == "ESP32-001"
    assert devices[0]["status"] == "ONLINE"
    assert devices[0]["messages_received"] == 1
    assert devices[0]["last_seen"]

    fetched = client.get("/api/devices/ESP32-001")
    assert fetched.status_code == 200
    assert fetched.json()["device_id"] == "ESP32-001"


def test_device_message_count_increments(client):
    for message_id in ("device-msg-a", "device-msg-b"):
        response = client.post("/api/messages/", json={
            "device_id": "ESP32-001",
            "message_id": message_id,
            "message_type": "text",
            "content": "Ping",
        })
        assert response.status_code == 201

    device = client.get("/api/devices/ESP32-001").json()
    assert device["messages_received"] == 2


def test_unknown_device_returns_404(client):
    response = client.get("/api/devices/unknown-device")
    assert response.status_code == 404
