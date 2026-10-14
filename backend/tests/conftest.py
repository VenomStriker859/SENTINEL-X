import pytest
from fastapi.testclient import TestClient

from app.main import app
from app.services.alert_manager import alert_manager
from app.services.device_manager import device_manager
from app.services.message_service import message_service


@pytest.fixture
def client():
    message_service.messages.clear()
    message_service.processed_ids.clear()
    device_manager.devices.clear()
    alert_manager.alerts.clear()

    with TestClient(app) as test_client:
        yield test_client

    message_service.messages.clear()
    message_service.processed_ids.clear()
    device_manager.devices.clear()
    alert_manager.alerts.clear()
