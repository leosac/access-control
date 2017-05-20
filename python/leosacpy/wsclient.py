import json
import time
import logging
import asyncio
import uuid
from enum import Enum, unique
from types import SimpleNamespace
from typing import Union

import websockets

from leosacpy.exception import InvalidMessageException


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
        self.content = content or {}  # type: dict
        self.uuid = ''  # type: str

        self.status_code = 0  # type: APIStatusCode
        self.status_string = ''  # for incoming msg

    def to_json(self):
        return json.dumps({
            'type': self.type,
            'content': self.content or {},
            'uuid': self.uuid
        })


def _message_from_dict(payload: dict) -> LeosacMessage:
    assert isinstance(payload, dict), 'Payload is not a dict but is {}'.format(
        type(payload))
    msg = LeosacMessage()

    if 'type' in payload and isinstance(payload['type'], str):
        msg.type = payload['type']
    else:
        raise InvalidMessageException(payload)

    if 'uuid' in payload and isinstance(payload['uuid'], str):
        msg.uuid = payload['uuid']
    else:
        raise InvalidMessageException(payload)

    if 'content' in payload and isinstance(payload['content'], (dict, type(None))):
        root = payload.get('content') or {}

        def convert(d):
            target = SimpleNamespace(**d)
            for k, v in d.items():
                if isinstance(v, dict):
                    target[k] = convert(v)
            return target

        ns = convert(root)
        msg.content = ns
    else:
        raise InvalidMessageException(payload)

    if 'status_code' in payload and isinstance(payload['status_code'], int):
        msg.status_code = APIStatusCode(payload['status_code'])

    if 'status_string' in payload and isinstance(payload['status_string'], str):
        msg.status_string = payload['status_string']

    return msg


class LeosacWSClient:
    """
    Implementation of Leosac client websocket API
    """

    def __init__(self):
        # None or a coroutine that read and dispatch
        # Awaited in close() if needed
        self.autoread = None
        self.ws = None

        # Map outgoing messages' UUID to a future to resolves
        # when the response arrives.
        self.uuid_to_future = {}

    async def connect(self, target, timeout=25, autoread=True):
        """
        Connect to target.
        :param target: URL to connect to ('ws://blah:1234')
        :param timeout:  Timeout before aborting connection attempt
        :param autoread: Should the client automatically read message. See below
        
        If ``autoread`` is true, the client will setup a coroutine that will
         continuously read from the connection and will dispatch messages.
        This is generally the behavior you wait, because it allows to 
        `await` on the future returned of value of `send()`. Otherwise it would
        never completes unless you call read_and_dispatch_once() yourself.
        
        Don't call recv() yourself if autoread is True.
        """
        start_time = time.time()
        while time.time() < start_time + timeout:
            try:
                self.ws = await websockets.connect(target)
                break
            except ConnectionError as e:
                logging.debug('Failed to connected: {}'.format(e))
                await asyncio.sleep(2)

        if autoread:
            self.autoread = asyncio.ensure_future(self.read_and_dispatch())

    async def close(self):
        """
        Close the socket, awaiting the autoread coroutine if needed.
        """
        if self.ws:
            await self.ws.close()
            self.ws = None

            if self.autoread:
                try:
                    await self.autoread
                except ConnectionError:
                    # Excepted since we just closed the connection
                    pass
                except Exception as e:
                    logging.warning('Exception when awaiting autoread '
                                    'coroutine: {}'.format(e))
                self.autoread = None

    async def read_and_dispatch_once(self):
        """
        Read from the socket and dispatch the matching by resolving the corresponding
        future, if any.
        :return: 
        """
        msg = await self.recv()
        if not self.dispatch_message(msg):
            logging.warning('TODO: UNHANDLED MESSAGE. Required if opportunistic '
                            'message from server. not yet')

    async def read_and_dispatch(self):
        while True:
            try:
                await self.read_and_dispatch_once()
            except Exception as e:
                logging.error('Error when reading and dispatching: '.format(e))
                raise e

    async def send(self, msg: LeosacMessage) -> asyncio.Future:
        """
        Send a LeosacMessage to the server.
        
        This function returns a future that will resolve when
        the response for this message (uuid matching) is received from
        the server.
        
        :param msg: 
        :return: 
        """
        assert isinstance(msg, LeosacMessage)

        # Add uuid if needed
        if not msg.uuid:
            msg.uuid = uuid.uuid4().hex

        assert msg.uuid not in self.uuid_to_future.keys()
        future = asyncio.Future()
        self.uuid_to_future[msg.uuid] = future
        await self.send_raw(msg.to_json())

        return future

    def dispatch_message(self, msg: LeosacMessage):
        """
        Dispatch the message by resolving the future associated
        with the message's uuid.
        
        :return: true if the message was dispatched, false otherwise.
        """
        fut = self.uuid_to_future.get(msg.uuid)
        if fut:
            fut.set_result(msg)
            return True
        else:
            logging.debug('Message had no matching future.')
        return False

    async def send_raw(self, obj: Union[str, dict]):
        """
        Send something to leosac.
        
        Note: Prefer using send() instead
        
        The behavior differ based on the type of `obj`.
            + str: send as is.
            + dict: serialize to json and send.
        
        :param obj: 
        """
        assert self.ws

        payload = obj if isinstance(obj, str) else json.dumps(obj)
        await self.ws.send(payload)

    async def recv(self) -> LeosacMessage:
        """
        Read a message from the websocket.
        
        Note: Prefer using autoread=True and not calling this.
        
        :return: A LeosacMessage
        """
        payload = await self.ws.recv()
        return _message_from_dict(json.loads(payload))
