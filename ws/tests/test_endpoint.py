"""End-to-end tests with FastAPI's TestClient. Needs: pip install fastapi httpx pytest"""
import pytest

fastapi = pytest.importorskip("fastapi")
from fastapi import FastAPI
from fastapi.testclient import TestClient

from ws import ConnectionManager, ws_router, websocket_manager


@pytest.fixture
def client():
    app = FastAPI()
    app.include_router(ws_router)

    @app.post("/_test/broadcast")  # test-only stand-in for the teammate's REST layer
    async def _broadcast(event: dict):
        return {"delivered": await websocket_manager.broadcast(event)}

    with TestClient(app) as c:
        yield c
    websocket_manager._connections.clear()


def test_connect_receives_welcome(client):
    with client.websocket_connect("/ws") as ws:
        msg = ws.receive_json()
        assert msg["event"] == "connected"


def test_ping_pong(client):
    with client.websocket_connect("/ws") as ws:
        ws.receive_json()
        ws.send_text("ping")
        assert ws.receive_json()["event"] == "pong"


def test_broadcast_to_multiple_clients(client):
    with client.websocket_connect("/ws") as a, client.websocket_connect("/ws") as b:
        a.receive_json(); b.receive_json()
        r = client.post("/_test/broadcast", json={"event": "alert", "data": {"level": "high"}})
        assert r.json()["delivered"] == 2
        for ws in (a, b):
            msg = ws.receive_json()
            assert msg["event"] == "alert" and msg["data"] == {"level": "high"}


def test_disconnect_removes_client(client):
    with client.websocket_connect("/ws") as ws:
        ws.receive_json()
        assert websocket_manager.active_count == 1
    assert websocket_manager.active_count == 0
