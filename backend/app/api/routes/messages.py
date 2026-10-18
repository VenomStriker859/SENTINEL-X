from fastapi import APIRouter, HTTPException

from app.schemas.message import IncomingMessage
from app.services.message_service import message_service
from ws.events import EventType, build_event
from ws.manager import websocket_manager


router = APIRouter(prefix="/messages", tags=["Messages"])


@router.post("/")
async def receive_message(message: IncomingMessage):
    result = message_service.process_message(message)

    if not result["success"]:
        raise HTTPException(
            status_code=409,
            detail=result["error"]
        )

    await websocket_manager.broadcast(
        build_event(EventType.NEW_MESSAGE, result["message"])
    )

    return result


@router.get("/")
def get_messages():
    return {
        "messages": list(message_service.messages.values())
    }


@router.get("/{message_id}")
def get_message(message_id: str):
    message = message_service.messages.get(message_id)

    if message is None:
        raise HTTPException(
            status_code=404,
            detail="Message not found"
        )

    return message