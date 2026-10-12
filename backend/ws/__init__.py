from .events import EventType, build_event, normalize_event
from .manager import ConnectionManager, websocket_manager

try:  # router needs FastAPI; manager/events do not
    from .router import ws_router
except ImportError:  # pragma: no cover
    ws_router = None

__all__ = [
    "EventType", "build_event", "normalize_event",
    "ConnectionManager", "websocket_manager", "ws_router",
]
