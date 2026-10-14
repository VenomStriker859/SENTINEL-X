from fastapi import FastAPI

from app.api.routes.messages import router as messages_router
from app.api.routes.devices import router as devices_router
from app.api.routes.alerts import router as alerts_router


app = FastAPI(
    title="SentinelX Backend API",
    description=(
        "REST API for SentinelX device messages and alerts. "
        "Current message, device, and alert data is held in memory and is reset on restart."
    ),
    version="0.2.0",
    contact={"name": "SentinelX Team"},
    openapi_tags=[
        {"name": "Health", "description": "Service availability checks."},
        {"name": "Messages", "description": "Receive and retrieve device messages."},
        {"name": "Devices", "description": "Inspect devices observed through incoming messages."},
        {"name": "Alerts", "description": "Create, inspect, and resolve alerts."},
    ],
)


@app.get("/api/health", tags=["Health"], summary="Health check")
def health_check():
    """Return a lightweight service status response."""
    return {"project": "SentinelX", "status": "online"}


app.include_router(messages_router, prefix="/api")
app.include_router(devices_router, prefix="/api")
app.include_router(alerts_router, prefix="/api")
