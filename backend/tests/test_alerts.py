import pytest


def test_create_list_get_and_resolve_alert(client):
    created = client.post("/api/alerts/", json={
        "device_id": "ESP32-001",
        "message": "Perimeter breach",
        "severity": "CRITICAL",
    })
    assert created.status_code == 201
    alert = created.json()
    assert alert["alert_id"]
    assert alert["status"] == "ACTIVE"
    assert alert["severity"] == "CRITICAL"
    alert_id = alert["alert_id"]

    listed = client.get("/api/alerts/")
    assert listed.status_code == 200
    assert len(listed.json()["alerts"]) == 1

    fetched = client.get(f"/api/alerts/{alert_id}")
    assert fetched.status_code == 200
    assert fetched.json()["alert_id"] == alert_id

    resolved = client.patch(f"/api/alerts/{alert_id}/resolve")
    assert resolved.status_code == 200
    assert resolved.json()["status"] == "RESOLVED"


@pytest.mark.parametrize("severity", ["bad", "critical", ""])
def test_invalid_severity_returns_validation_error(client, severity):
    response = client.post("/api/alerts/", json={
        "device_id": "ESP32-001",
        "message": "Test alert",
        "severity": severity,
    })
    assert response.status_code == 422


def test_missing_required_fields_are_rejected(client):
    response = client.post("/api/alerts/", json={"device_id": "ESP32-001"})
    assert response.status_code == 422


def test_unknown_alert_returns_404(client):
    assert client.get("/api/alerts/not-real").status_code == 404
    assert client.patch("/api/alerts/not-real/resolve").status_code == 404
