from datetime import datetime, timezone
from typing import Literal

from pydantic import BaseModel, Field, model_validator


class IncomingMessage(BaseModel):
    device_id: str = Field(min_length=1, max_length=50)
    message_id: str = Field(min_length=1, max_length=100)
    message_type: Literal["text", "image", "audio"]
    content: str | None = None

    timestamp: datetime = Field(
        default_factory=lambda: datetime.now(timezone.utc)
    )

    @model_validator(mode="after")
    def validate_message_content(self):
        if self.message_type == "text":
            if not self.content or not self.content.strip():
                raise ValueError("Text messages require non-empty content")

        if self.message_type in ("image", "audio"):
            if not self.content:
                raise ValueError(
                    "Image and audio messages require a content reference"
                )

        return self