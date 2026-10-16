"""FastAPI router exposing the WebSocket endpoint.

Integration (in the FastAPI app):
    from ws import ws_router, websocket_manager
    app.include_router(ws_router)
    ...
    await websocket_manager.broadcast({"event": "alert", "data": {...}})
"""
from fastapi import APIRouter, WebSocket, WebSocketDisconnect

from .events import EventType, build_event
from .manager import websocket_manager

ws_router = APIRouter()


@ws_router.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket) -> None:
    await websocket_manager.connect(websocket)
    try:
        while True:
            message = await websocket.receive_text()
            # Clients only need keep-alive; everything else is server -> client.
            if message.strip().lower() == "ping":
                await websocket_manager.send_personal(websocket, build_event(EventType.PONG))
    except WebSocketDisconnect:
        pass
    finally:
        websocket_manager.disconnect(websocket)
