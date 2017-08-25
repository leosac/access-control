import asyncio
import inspect
import logging
import unittest
from functools import wraps

from leosacpy.runner import RunnerConfig, Runner, \
    LeosacCachedDBRunnerFactory
from leosacpy.utils import LogMixin
from leosacpy.ws import LeosacMessage, APIStatusCode
from leosacpy.wsclient import LowLevelWSClient


class ParametrizedTestCase(unittest.TestCase):
    """ TestCase classes that want to be parametrized should
        inherit from this class.

        A `param` attribute will be set in the constructor.
    """

    def __init__(self, methodName='runTest', param=None):
        super(ParametrizedTestCase, self).__init__(methodName)
        self.param = param

    @staticmethod
    def create_suite(testcase_klass, param=None, test_name=None):
        """ Create a suite containing all tests taken from the given
            subclass, passing them the parameter 'param'.
        """
        testloader = unittest.TestLoader()
        if test_name is None:
            testnames = testloader.getTestCaseNames(testcase_klass)
        else:
            testnames = [test_name]
        suite = unittest.TestSuite()
        for name in testnames:
            suite.addTest(testcase_klass(name, param=param))
        return suite


class WSTestBase(ParametrizedTestCase, LogMixin):
    """
    The base class to use when writing tests that will
    use Leosac websocket API.

    WSTestBase inherits from ParametrizedTestCase and expects
    an a factory function for runner to be stored in self.param['runner_factory']
    """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.runner = None  # type: Runner
        self.runner_config = None  # type: RunnerConfig
        self.loop = None

        try:
            self.runner_factory = self.param['runner_factory']
        except Exception as e:
            self.runner_factory = LeosacCachedDBRunnerFactory()

        logging.basicConfig(level=logging.INFO)
        logging.getLogger().setLevel(logging.DEBUG)

    def get_runner_config(self) -> RunnerConfig:
        c = RunnerConfig()
        c.loop = self.loop
        return c

    async def get_ws_to_leosac(self, autoread=True) -> LowLevelWSClient:
        """
        Return a connected LowLevelWSClient.
        :return: LowLevelWSClient
        """
        assert self.runner, 'No Runner'
        c = LowLevelWSClient()

        url = self.runner.get_ws_address()
        self.logger.debug('Connecting to {}'.format(url))
        await c.connect(url, autoread=autoread)
        return c

    def setUp(self):
        super().setUp()
        # Setup a new asyncio loop.
        self.loop = asyncio.new_event_loop()
        asyncio.set_event_loop(None)

        if not self.runner_config:
            self.runner_config = self.get_runner_config()
        if not self.runner:
            self.runner = self.runner_factory(self.runner_config)

    def tearDown(self):
        # Clear runner and runner_config to get a fresh one
        # for next test
        self.runner = None
        self.runner_config = None


def check_return_code(expected_exit_code: int):
    def decorator(f):
        @wraps(f)
        def wrap(*args, **kwargs):
            self = args[0]
            assert isinstance(self, WSTestBase)
            f(*args, **kwargs)
            self.assertEqual(expected_exit_code, self.runner.get_return_code(),
                             'Unexpected return code')

        return wrap

    return decorator


def with_leosac_infrastructure(f):
    """
    Arrange for the test to be run alongside an available and clean
    leosac infrastructure.

    This decorator MUST be used on a method of a subclass of WSTestBase, otherwise
    it will assert.

    This decorator will invoke the test method after turner the
    runner into a Context Manager.
    """
    assert inspect.iscoroutinefunction(f), 'Function is not a coroutine.'

    @wraps(f)
    def wrap(*args, **kwargs):
        test_case = args[0]
        assert isinstance(test_case, WSTestBase)
        # TestCase class name + current function name
        full_test_name = '{}/{}'.format(test_case.__class__.__name__,
                                        f.__name__)

        # We patch the runner config to specify the fully qualified name
        # of the test. This will allows good location of log file.
        test_case.runner_config.fully_qualified_test_name = full_test_name

        async def _run_test():
            assert isinstance(test_case.runner, Runner), 'No runner !'
            async with test_case.runner as r:
                await f(*args, **kwargs)

        test_case.loop.run_until_complete(_run_test())

    wrap.with_leosac_infrastructure__ = True
    return wrap


def with_leosac_ws_client(autoread=True):
    """
    Method decorator to add a `wsclient` argument to the test method.

    The wsclient will be a connected LowLevelWSClient().
    :param autoread: See LowLevelWSClient
    """

    def decorator(f):
        @wraps(f)
        async def wrap(*args, **kwargs):
            test_case = args[0]
            assert isinstance(test_case, WSTestBase), 'Wrong type'

            c = await test_case.get_ws_to_leosac(autoread=autoread)
            kwargs['wsclient'] = c
            try:
                await f(*args, **kwargs)
            finally:
                try:
                    # Now attempt to close, ignoring exception.
                    await c.close()
                except Exception:
                    pass
        return wrap

    return decorator


def ws_authenticated_as_admin(f):
    """
    Requires @with_leosac_ws_client().

    This decorator will authenticate the ws_client as admin before
    letting the test run.
    """

    @wraps(f)
    async def wrap(*args, **kwargs):
        test_case = args[0]
        assert isinstance(test_case, WSTestBase), 'Wrong type'

        ws_client = kwargs['wsclient']
        assert isinstance(ws_client, LowLevelWSClient)

        # Authenticate as administrator, and asserts that it worked.
        msg = LeosacMessage(message_type='create_auth_token', content={
            'username': 'admin',
            'password': 'admin'
        })
        rep = await ws_client.req_rep(msg)
        if rep.status_code != APIStatusCode.SUCCESS:
            raise RuntimeError('@ws_authenticated_as_admin failed.')

        await f(*args, **kwargs)

    return wrap
