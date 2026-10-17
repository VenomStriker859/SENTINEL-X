from datetime import datetime, timezone


class DeviceManager:
    def __init__(self):
        self.devices = {}

    def register_message(self, device_id: str):
        now = datetime.now(timezone.utc)

        if device_id not in self.devices:
            self.devices[device_id] = {
                "device_id": device_id,
                "status": "ONLINE",
                "last_seen": now.isoformat(),
                "messages_received": 0
            }

        device = self.devices[device_id]
        device["status"] = "ONLINE"
        device["last_seen"] = now.isoformat()
        device["messages_received"] += 1

        return device

    def get_devices(self):
        return list(self.devices.values())

    def get_device(self, device_id: str):
        return self.devices.get(device_id)


device_manager = DeviceManager()