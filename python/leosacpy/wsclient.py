import asyncio
import json
import logging
import time
import uuid
from typing import Union

import websockets

from leosacpy.exception import InvalidMessageException, APIError, APIModelException
from leosacpy.utils import LogMixin, assert_isinstance
from leosacpy.ws import LeosacMessage, APIStatusCode


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

    if 'content' in payload and isinstance(payload['content'],
                                           (dict, list, type(None))):
        msg.content = payload.get('content') or {}
    else:
        raise InvalidMessageException(payload)

    if 'status_code' in payload and isinstance(payload['status_code'], int):
        msg.status_code = APIStatusCode(payload['status_code'])

    if 'status_string' in payload and isinstance(payload['status_string'], str):
        msg.status_string = payload['status_string']

    return msg


class LowLevelWSClient:
    """
    Implementation of a low level Leosac client.
    
    Provide method to send and receive websocket message.
        
    In default mode, you can asynchronously get the response to a message
    by calling `await()` on the future returned by send().
    """

    def __init__(self):
        # None or a coroutine that read and dispatch
        # Awaited in close() if needed
        self.autoread = None
        self.ws = None

        # Map outgoing messages' UUID to a future to resolves
        # when the response arrives.
        self.uuid_to_future = {}

    @property
    def connected(self):
        return self.ws is not None

    async def connect(self, target, timeout=45, autoread=True):
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
        last_ex = None
        start_time = time.time()
        while time.time() < start_time + timeout:
            try:
                self.ws = await websockets.connect(target)
                last_ex = None
                break
            except ConnectionError as e:
                logging.debug('Failed to connect: {}'.format(e))
                last_ex = e
                await asyncio.sleep(2)

        if last_ex:
            raise last_ex

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
            except websockets.ConnectionClosed as e:
                # May be normal.
                logging.info('Websocket connection closed')
                raise e
            except Exception as e:
                logging.exception('Error when reading and dispatching')
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

    async def req_rep(self, msg: LeosacMessage) -> LeosacMessage:
        """
        Send the ``msg`` and wait for the response.
        """
        fut = await self.send(msg)
        return await fut

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


class LeosacAPI(LogMixin):
    """
    A high level API to leosac
    """

    def __init__(self, target: str = None, client: LowLevelWSClient = None):
        if isinstance(client, LowLevelWSClient):
            self.client = client
        else:
            self.client = LowLevelWSClient()

        if target:
            assert_isinstance(target, str)
        self.target = target
        self.logger.info('LeosacAPI target: {}'.format(self.target))

    async def _send(self, msg: LeosacMessage):
        if not self.client.connected:
            await self.client.connect(self.target)

        assert self.client.connected, 'Still not connected'
        return await self.client.send(msg)

    async def _req_rep(self, msg: LeosacMessage,
                       require_success=True) -> LeosacMessage:
        """
        Send and wait for response.
        
        Is require_success is True, then an APIError is raised if the status
        code is not SUCCESS
        """
        fut = await self._send(msg)
        msg = await fut
        if require_success and not msg.status_code == APIStatusCode.SUCCESS:
            if msg.status_code == APIStatusCode.MODEL_EXCEPTION:
                raise APIModelException(msg)
            else:
                raise APIError(msg)
        return msg

    async def get_version(self, short=False):
        """
        Retrieve version of leosac server
        :bool short: Returns the short version if true.
        """
        rep = await self._req_rep(LeosacMessage(message_type='get_leosac_version'))
        if short:
            return rep.content['version_short']
        return rep.content['version']

    async def authenticate(self, username, password):
        """
        Attempt to authenticate the user with username/password credentials
        """
        assert isinstance(username, str)
        assert isinstance(password, str)

        try:
            rep = await self._req_rep(LeosacMessage(message_type='create_auth_token',
                                                    content={
                                                        'username': username,
                                                        'password': password
                                                    }))
            if rep.content['status'] == 0:
                return True
        except APIError as e:
            return False

    async def zone_create(self, alias, zone_type, desc,
                          doors=None, children=None):
        if doors is None:
            doors = []
        if children is None:
            children = []
        rep = await self._req_rep(LeosacMessage(message_type='zone.create',
                                                content={
                                                    'attributes': {
                                                        'alias': alias,
                                                        'type': zone_type,
                                                        'description': desc,
                                                        'doors': doors,
                                                        'children': children
                                                    }
                                                }))
        return rep

    async def restart(self):
        """
        Request that the Leosac server perform a restart.
        """
        rep = await self._req_rep(LeosacMessage(message_type='restart',
                                                content={}))
        if rep.content['status'] == 0:
            return True

    async def close(self):
        """
        When done using the API object, call close() to clean
        resource.
        
        todo: Alternatively, use with context manager
        """
        if self.client:
            await self.client.close()
        self.client = None
