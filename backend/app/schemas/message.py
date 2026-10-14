from datetime import datetime, timezone
from typing import Annotated, Literal

from pydantic import BaseModel, Field, StringConstraints, model_validator


Identifier50 = Annotated[str, StringConstraints(strip_whitespace=True, min_length=1, max_length=50)]
Identifier100 = Annotated[str, StringConstraints(strip_whitespace=True, min_length=1, max_length=100)]


class IncomingMessage(BaseModel):
    """Validated message received from a SentinelX device."""

    device_id: Identifier50 = Field(description="Originating device ID")
    message_id: Identifier100 = Field(description="Unique message ID used for duplicate detection")
    message_type: Literal["text", "image", "audio"] = Field(description="Message payload type")
    content: str | None = Field(
        default=None,
        description="Text body or reference to image/audio content",
    )
    timestamp: datetime = Field(
        default_factory=lambda: datetime.now(timezone.utc),
        description="UTC time associated with the message",
    )

    @model_validator(mode="after")
    def validate_message_content(self):
        if self.message_type == "text":
            if not self.content or not self.content.strip():
                raise ValueError("Text messages require non-empty content")

        if self.message_type in ("image", "audio"):
            if not self.content or not self.content.strip():
                raise ValueError("Image and audio messages require a content reference")

        return self
