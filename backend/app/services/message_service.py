from app.schemas.message import IncomingMessage
from app.services.device_manager import device_manager

class MessageService:
    def __init__(self):
        self.messages = {}
        self.processed_ids = set()

    def process_message(self, message: IncomingMessage):
        if message.message_id in self.processed_ids:
            return {
                "success": False,
                "error": "Duplicate message"
            }

        self.processed_ids.add(message.message_id)
        device_manager.register_message(message.device_id)

        self.messages[message.message_id] = message.model_dump(
            mode="json"
        )

        return {
            "success": True,
            "message": self.messages[message.message_id]
        }


message_service = MessageService()