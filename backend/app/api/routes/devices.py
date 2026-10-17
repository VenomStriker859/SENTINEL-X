from fastapi import APIRouter, HTTPException

from app.services.device_manager import device_manager


router = APIRouter(prefix="/devices", tags=["Devices"])


@router.get("/")
def get_devices():
    return {
        "devices": device_manager.get_devices()
    }


@router.get("/{device_id}")
def get_device(device_id: str):
    device = device_manager.get_device(device_id)

    if device is None:
        raise HTTPException(
            status_code=404,
            detail="Device not found"
        )

    return device