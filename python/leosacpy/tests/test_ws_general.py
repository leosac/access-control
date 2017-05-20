import asyncio
import inspect
import logging
import os
import unittest

from leosacpy.runner import LeosacFullRunner, RunnerConfig
from leosacpy.wsclient import LeosacWSClient, APIStatusCode, LeosacMessage


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

        leosac_cfg_file = '{}/leosac_config.xml' \
            .format(os.path.dirname(os.path.abspath(__file__)))
        self.runner_cfg = RunnerConfig(loop=self.loop,
                                       leosac_config_file=leosac_cfg_file)

        # Available for test using with_leosac_infrastructure decorator
        self.runner = None

    async def _get_ws_to_leosac(self, autoread=True) -> LeosacWSClient:
        """
        Return a connected LeosacWSClient.
        :return: LeosacWSClient
        """
        assert self.runner, 'No Runner'
        c = LeosacWSClient()

        url = self.runner.get_ws_address()
        self.logger.debug('Connecting to {}'.format(url))
        await c.connect(url, autoread=autoread)
        return c

    @with_leosac_infrastructure
    async def test_malformed_messaged(self, runner: LeosacFullRunner):
        wsclient = await self._get_ws_to_leosac(autoread=False)
        await wsclient.send_raw({})
        ret = await wsclient.recv()

        self.assertEqual(APIStatusCode.MALFORMED, ret.status_code)

    @with_leosac_infrastructure
    async def test_get_version(self, runner: LeosacFullRunner):
        wsclient = await self._get_ws_to_leosac()
        msg = LeosacMessage(message_type='get_leosac_version')

        response_future = await wsclient.send(msg)
        rep = await response_future

        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        self.assertEqual('0.6.3', rep.content.version)
        await wsclient.close()

if __name__ == '__main__':
    unittest.main()
