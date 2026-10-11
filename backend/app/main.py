from fastapi import FastAPI

from app.api.routes.messages import router as messages_router
from app.api.routes.devices import router as devices_router
from app.api.routes.alerts import router as alerts_router

app = FastAPI(
    title="SentinelX Backend",
    description="Backend API for the SentinelX communication system",
    version="0.1.0"
)


@app.get("/api/health")
def health_check():
    return {
        "project": "SentinelX",
        "status": "online"
    }


app.include_router(messages_router, prefix="/api")
app.include_router(devices_router, prefix="/api")
app.include_router(alerts_router, prefix="/api")