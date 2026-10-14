from fastapi import APIRouter, HTTPException, status

from app.schemas.message import IncomingMessage
from app.services.message_service import message_service


router = APIRouter(prefix="/messages", tags=["Messages"])


@router.post(
    "/",
    status_code=status.HTTP_201_CREATED,
    summary="Receive a message",
    description="Accepts text, image, or audio metadata. Reusing a message_id returns HTTP 409.",
    responses={409: {"description": "Duplicate message ID"}},
)
def receive_message(message: IncomingMessage):
    result = message_service.process_message(message)
    if not result["success"]:
        raise HTTPException(status_code=409, detail=result["error"])
    return result


@router.get("/", summary="List messages", description="Returns messages currently held in memory.")
def get_messages():
    return {"messages": list(message_service.messages.values())}


@router.get("/{message_id}", summary="Get a message by ID")
def get_message(message_id: str):
    message = message_service.messages.get(message_id)
    if message is None:
        raise HTTPException(status_code=404, detail="Message not found")
    return message
