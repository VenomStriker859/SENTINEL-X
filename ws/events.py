"""Event format shared by the WebSocket layer.

Wire format (server -> client), always a JSON object:

    {"event": "<name>", "data": {...}, "timestamp": "<ISO-8601 UTC>"}

The integration interface `await websocket_manager.broadcast(event)` accepts a
dict with the event name under "event" (or "type") and a payload under "data".
"""
from datetime import datetime, timezone
from typing import Any, Dict


class EventType:
    NEW_MESSAGE = "new_message"
    ALERT = "alert"
    CONNECTED = "connected"
    PONG = "pong"
    ERROR = "error"


def build_event(event: str, data: Any = None) -> Dict[str, Any]:
    if not isinstance(event, str) or not event.strip():
        raise ValueError("event name must be a non-empty string")
    return {
        "event": event,
        "data": {} if data is None else data,
        "timestamp": datetime.now(timezone.utc).isoformat(),
    }


def normalize_event(event: Dict[str, Any]) -> Dict[str, Any]:
    """Validate a caller-supplied dict and return the wire-format event."""
    if not isinstance(event, dict):
        raise TypeError("event must be a dict")
    name = event.get("event", event.get("type"))
    return build_event(name, event.get("data"))
