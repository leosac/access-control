import asyncio
import inspect
import logging
import os
import unittest
from functools import wraps

from leosacpy.runner import LeosacFullRunner, RunnerConfig
from leosacpy.wsclient import LowLevelWSClient, APIStatusCode, LeosacMessage, \
    LeosacAPI


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

    @wraps(f)
    def wrap(*args, **kwargs):
        test_case = args[0]
        assert isinstance(test_case, unittest.TestCase)
        # TestCase class name + current function name
        full_test_name = '{}/{}'.format(test_case.__class__.__name__,
                                        f.__name__)

        # We patch the runner config to specify the fully qualified name
        # of the test. This will allows good location of log file.
        test_case.runner_cfg.fully_qualified_test_name = full_test_name

        async def _run_test():
            async with LeosacFullRunner(test_case.runner_cfg) as r:
                test_case.runner = r
                kwargs['runner'] = r
                await f(*args, **kwargs)

        test_case.loop.run_until_complete(_run_test())

    wrap.with_leosac_infrastructure__ = True
    return wrap


def with_leosac_ws_client(f):
    @wraps(f)
    async def wrap(*args, **kwargs):
        test_case = args[0]
        assert isinstance(test_case, WSGeneral), 'Wrong type'

        c = await test_case._get_ws_to_leosac()
        kwargs['wsclient'] = c
        await f(*args, **kwargs)
        await c.close()

    return wrap


def check_return_code(expected_exit_code: int):
    def decorator(f):
        @wraps(f)
        def wrap(*args, **kwargs):
            self = args[0]
            assert isinstance(self, WSGeneral)
            f(*args, **kwargs)
            self.assertEqual(expected_exit_code, self.runner.get_return_code(),
                             'Unexpected return code')

        return wrap

    return decorator


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

    async def _get_ws_to_leosac(self, autoread=True) -> LowLevelWSClient:
        """
        Return a connected LeosacWSClient.
        :return: LeosacWSClient
        """
        assert self.runner, 'No Runner'
        c = LowLevelWSClient()

        url = self.runner.get_ws_address()
        self.logger.debug('Connecting to {}'.format(url))
        await c.connect(url, autoread=autoread)
        return c

    @check_return_code(0)
    @with_leosac_infrastructure
    async def test_get_version2(self, runner: LeosacFullRunner=None):
        api = LeosacAPI(target=runner.get_ws_address())
        self.assertEqual('0.6.3', await api.get_version())
        await api.close()

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client
    async def test_get_version(self, runner=None, wsclient: LowLevelWSClient=None):
        msg = LeosacMessage(message_type='get_leosac_version')
        rep = await wsclient.req_rep(msg)

        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        self.assertEqual('0.6.3', rep.content.version)

    @check_return_code(0)
    @with_leosac_infrastructure
    async def test_malformed_messaged(self, runner: LeosacFullRunner = None):
        wsclient = await self._get_ws_to_leosac(autoread=False)
        await wsclient.send_raw({})
        ret = await wsclient.recv()

        self.assertEqual(APIStatusCode.MALFORMED, ret.status_code)


if __name__ == '__main__':
    unittest.main()
