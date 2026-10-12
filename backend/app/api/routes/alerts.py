from fastapi import APIRouter, HTTPException
from pydantic import BaseModel, Field

from app.services.alert_manager import alert_manager


router = APIRouter(prefix="/alerts", tags=["Alerts"])


class AlertCreate(BaseModel):
    device_id: str = Field(min_length=1, max_length=50)
    message: str = Field(min_length=1, max_length=500)
    severity: str = "HIGH"


@router.post("/")
def create_alert(data: AlertCreate):
    if data.severity not in ["LOW", "MEDIUM", "HIGH", "CRITICAL"]:
        raise HTTPException(
            status_code=400,
            detail="Invalid severity"
        )

    return alert_manager.create_alert(
        device_id=data.device_id,
        message=data.message,
        severity=data.severity
    )


@router.get("/")
def get_alerts():
    return {"alerts": alert_manager.get_alerts()}


@router.get("/{alert_id}")
def get_alert(alert_id: str):
    alert = alert_manager.get_alert(alert_id)

    if alert is None:
        raise HTTPException(status_code=404, detail="Alert not found")

    return alert


@router.patch("/{alert_id}/resolve")
def resolve_alert(alert_id: str):
    alert = alert_manager.resolve_alert(alert_id)

    if alert is None:
        raise HTTPException(status_code=404, detail="Alert not found")

    return alert