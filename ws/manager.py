"""Connection manager: tracks clients, broadcasts events, prunes dead sockets.

Only relies on the small duck-typed surface of a WebSocket
(accept / send_json / close), so it is testable without FastAPI.
"""
import asyncio
import logging
from typing import Any, Dict, Set

from .events import EventType, build_event, normalize_event

logger = logging.getLogger("sentinelx.ws")

SEND_TIMEOUT = 5.0  # seconds; a slow client must not stall a broadcast


class ConnectionManager:
    def __init__(self, send_timeout: float = SEND_TIMEOUT) -> None:
        self._connections: Set[Any] = set()
        self._send_timeout = send_timeout

    @property
    def active_count(self) -> int:
        return len(self._connections)

    async def connect(self, websocket: Any) -> None:
        await websocket.accept()
        self._connections.add(websocket)
        logger.info("client connected (%d active)", self.active_count)
        await self.send_personal(
            websocket,
            build_event(EventType.CONNECTED, {"clients": self.active_count}),
        )

    def disconnect(self, websocket: Any) -> None:
        """Idempotent; safe to call from several code paths."""
        if websocket in self._connections:
            self._connections.discard(websocket)
            logger.info("client disconnected (%d active)", self.active_count)

    async def _safe_send(self, websocket: Any, payload: Dict[str, Any]) -> bool:
        try:
            await asyncio.wait_for(websocket.send_json(payload), self._send_timeout)
            return True
        except Exception as exc:  # closed socket, timeout, runtime error...
            logger.warning("send failed (%s: %s); dropping client", type(exc).__name__, exc)
            self.disconnect(websocket)
            try:
                await websocket.close()
            except Exception:
                pass
            return False

    async def send_personal(self, websocket: Any, event: Dict[str, Any]) -> bool:
        return await self._safe_send(websocket, event)

    async def broadcast(self, event: Dict[str, Any]) -> int:
        """Send `event` to every client. Returns the number of successful sends.

        Raises ValueError/TypeError for a malformed event (caller bug); never
        raises because of a client problem.
        """
        payload = normalize_event(event)
        targets = list(self._connections)  # snapshot: set may change while awaiting
        if not targets:
            return 0
        results = await asyncio.gather(*(self._safe_send(ws, payload) for ws in targets))
        return sum(results)

    async def close_all(self) -> None:
        for ws in list(self._connections):
            self.disconnect(ws)
            try:
                await ws.close()
            except Exception:
                pass


# Singleton used by the integration interface: `await websocket_manager.broadcast(event)`
websocket_manager = ConnectionManager()
