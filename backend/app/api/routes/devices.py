from fastapi import APIRouter, HTTPException

from app.services.device_manager import device_manager


router = APIRouter(prefix="/devices", tags=["Devices"])


@router.get(
    "/",
    summary="List observed devices",
    description="Returns devices registered by accepted incoming messages. Device state is held in memory.",
)
def get_devices():
    return {"devices": device_manager.get_devices()}


@router.get(
    "/{device_id}",
    summary="Get a device by ID",
    description="Returns device status, last-seen time, and accepted message count.",
    responses={404: {"description": "Device not found"}},
)
def get_device(device_id: str):
    device = device_manager.get_device(device_id)
    if device is None:
        raise HTTPException(status_code=404, detail="Device not found")
    return device
