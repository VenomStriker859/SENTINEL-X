from datetime import datetime, timezone
from uuid import uuid4


class AlertManager:
    def __init__(self):
        self.alerts = {}

    def create_alert(
        self,
        device_id: str,
        message: str,
        severity: str = "HIGH"
    ):
        alert_id = str(uuid4())

        alert = {
            "alert_id": alert_id,
            "device_id": device_id,
            "message": message,
            "severity": severity,
            "status": "ACTIVE",
            "created_at": datetime.now(timezone.utc).isoformat()
        }

        self.alerts[alert_id] = alert
        return alert

    def get_alerts(self):
        return list(self.alerts.values())

    def get_alert(self, alert_id: str):
        return self.alerts.get(alert_id)

    def resolve_alert(self, alert_id: str):
        alert = self.alerts.get(alert_id)

        if alert is None:
            return None

        alert["status"] = "RESOLVED"
        return alert


alert_manager = AlertManager()