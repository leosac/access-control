import asyncio
import inspect
import json
import logging
import time
import unittest
import websockets

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


def with_leosac_infrastructure(f):
    """
    Arrange for the test to be run alongside an available and clean
    leosac infrastructure.
    
    An additional parameter is passed to the test function, an instance
    of LeosacFullRunner that allows the test to retrieve information about
    the infrastructure (docker containers) setup for it.
    
    Note: The def must be defined with "async def".
    Note: This adds huge overhead of starting container etc.    
    """
    assert inspect.iscoroutinefunction(f), 'Function is not a coroutine.'

    def wrap(self):
        # TestCase class name + current function name
        full_test_name = '{}/{}'.format(self.__class__.__name__,
                                        f.__name__)

        # We patch the runner config to specify the fully qualified name
        # of the test. This will allows good location of log file.
        self.runner_cfg.fully_qualified_test_name = full_test_name
        async def _run_test():
            async with LeosacFullRunner(self.runner_cfg) as r:
                self.runner = r
                await f(self, r)

        self.loop.run_until_complete(_run_test())

    return wrap


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

    @with_leosac_infrastructure
    async def test_get_version(self, runner: LeosacFullRunner):
        url = runner.get_ws_address()
        self.logger.debug('Connection to {}'.format(url))
        ws = LeosacWSClient()
        await ws.connect(runner.get_ws_address())
        self.logger.debug('Connected to leosac WS')
        await ws.send({})
        ret = await ws.recv()
        print(ret)
        self.logger.info('Will stop')

    @with_leosac_infrastructure
    async def test_get_version2(self, runner: LeosacFullRunner):
        await asyncio.sleep(5)


if __name__ == '__main__':
    unittest.main()
