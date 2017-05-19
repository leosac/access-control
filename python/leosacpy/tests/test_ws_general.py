import json
import unittest

import time

import asyncio
import websockets
import logging

from leosacpy.runner import LeosacFullRunner, RunnerConfig


class LeosacWSClient:
    def __init__(self):
        self.ws = None

    async def connect(self, target, timeout=25):
        start_time = time.time()
        while time.time() < start_time + timeout:
            try:
                self.ws = await websockets.connect(target)
                break
            except ConnectionError as e:
                logging.debug('Failed to connected: {}'.format(e))
                await asyncio.sleep(2)

    async def send(self, obj):
        c = json.dumps(obj)
        await self.ws.send(c)

    async def recv(self):
        return await self.ws.recv()


class WSGeneral(unittest.TestCase):
    def setUp(self):
        super().setUp()
        logging.basicConfig(level=logging.INFO)
        logging.getLogger().setLevel(logging.DEBUG)

        self.loop = asyncio.new_event_loop()
        asyncio.set_event_loop(None)

        self.logger = logging.getLogger('WSGeneral')
        self.logger.setLevel(logging.DEBUG)

        self.runner_cfg = RunnerConfig(loop=self.loop)

    def test_get_version(self):
        async def run_test():
            async with LeosacFullRunner(self.runner_cfg) as r:
                url = r.get_ws_address()
                self.logger.debug('Connection to {}'.format(url))
                ws = LeosacWSClient()
                await ws.connect(r.get_ws_address())
                self.logger.debug('Connected to leosac WS')
                await ws.send({})
                ret = await ws.recv()
                print(ret)
                self.logger.info('Will stop')

        self.loop.run_until_complete(run_test())
        self.logger.info('Loop done')


if __name__ == '__main__':
    unittest.main()
