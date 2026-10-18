from fastapi.testclient import TestClient
from app.main import app

client = TestClient(app)

with client.websocket_connect("/ws") as ws:
    welcome = ws.receive_json()
    print("WELCOME:", welcome["event"])

    ws.send_text("ping")

    response = ws.receive_json()
    print("PING RESPONSE:", response["event"])

    assert welcome["event"] == "connected"
    assert response["event"] == "pong"

print("WEBSOCKET INTEGRATION: PASSED")