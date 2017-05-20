import json
from enum import unique, Enum
from types import SimpleNamespace


@unique
class APIStatusCode(Enum):
    SUCCESS = 0x00
    GENERAL_FAILURE = 0x01
    PERMISSION_DENIED = 0x02
    RATE_LIMITED = 0x03
    MALFORMED = 0x04
    INVALID_CALL = 0x05
    TIMEOUT = 0x06
    SESSION_ABORTED = 0x07
    ENTITY_NOT_FOUND = 0x08
    DATABASE_ERROR = 0x09
    UNKNOWN = 0x0A
    MODEL_EXCEPTION = 0x0B


class LeosacMessage:
    def __init__(self, message_type: str = '', content=None):
        self.type = message_type  # type: str
        self.content = content or {}  # type: SimpleNamespace
        self.uuid = ''  # type: str

        self.status_code = 0  # type: APIStatusCode
        self.status_string = ''  # for incoming msg

    def to_json(self):
        return json.dumps({
            'type': self.type,
            'content': self.content or {},
            'uuid': self.uuid
        })

