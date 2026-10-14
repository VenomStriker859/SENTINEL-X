from typing import Annotated, Literal

from fastapi import APIRouter, HTTPException, status
from pydantic import BaseModel, Field, StringConstraints

from app.services.alert_manager import alert_manager


router = APIRouter(prefix="/alerts", tags=["Alerts"])

NonBlankString = Annotated[str, StringConstraints(strip_whitespace=True, min_length=1)]


class AlertCreate(BaseModel):
    """Payload used to create an active alert."""

    device_id: Annotated[
        str,
        StringConstraints(strip_whitespace=True, min_length=1, max_length=50),
        Field(description="Originating device ID"),
    ]
    message: Annotated[
        str,
        StringConstraints(strip_whitespace=True, min_length=1, max_length=500),
        Field(description="Human-readable alert details"),
    ]
    severity: Literal["LOW", "MEDIUM", "HIGH", "CRITICAL"] = Field(
        default="HIGH",
        description="Alert severity; uppercase values only",
    )


@router.post(
    "/",
    status_code=status.HTTP_201_CREATED,
    summary="Create an alert",
    description="Creates an active alert and returns its generated ID and timestamp.",
)
def create_alert(data: AlertCreate):
    return alert_manager.create_alert(
        device_id=data.device_id,
        message=data.message,
        severity=data.severity,
    )


@router.get("/", summary="List alerts", description="Returns all alerts currently held in memory.")
def get_alerts():
    return {"alerts": alert_manager.get_alerts()}


@router.get("/{alert_id}", summary="Get an alert by ID")
def get_alert(alert_id: str):
    alert = alert_manager.get_alert(alert_id)
    if alert is None:
        raise HTTPException(status_code=404, detail="Alert not found")
    return alert


@router.patch("/{alert_id}/resolve", summary="Resolve an alert")
def resolve_alert(alert_id: str):
    alert = alert_manager.resolve_alert(alert_id)
    if alert is None:
        raise HTTPException(status_code=404, detail="Alert not found")
    return alert
