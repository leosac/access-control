import os
import unittest

from leosacpy.runner import LeosacCachedDBRunnerFactory
from leosacpy.tests.test_helper import WSTestBase, check_return_code, \
    with_leosac_infrastructure, with_leosac_ws_client
from leosacpy.wsclient import LowLevelWSClient, APIStatusCode, LeosacMessage, \
    LeosacAPI


class WSGeneral(WSTestBase):
    def setUp(self):
        super().setUp()

    def get_runner_config(self):
        cfg = super().get_runner_config()

        leosac_cfg_file = '{}/leosac_config.xml' \
            .format(os.path.dirname(os.path.abspath(__file__)))
        cfg.leosac_config_file = leosac_cfg_file
        cfg.stream_log = False
        return cfg

    @check_return_code(0)
    @with_leosac_infrastructure
    async def test_get_version2(self):
        api = LeosacAPI(target=self.runner.get_ws_address())
        self.assertEqual('0.6.3', await api.get_version(short=True))
        await api.close()

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    async def test_get_version(self, wsclient: LowLevelWSClient = None):
        msg = LeosacMessage(message_type='get_leosac_version')
        rep = await wsclient.req_rep(msg)

        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        self.assertEqual('0.6.3', rep.content['version_short'])

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client(autoread=False)
    async def test_malformed_message(self, wsclient: LowLevelWSClient):
        await wsclient.send_raw({})
        ret = await wsclient.recv()

        self.assertEqual(APIStatusCode.MALFORMED, ret.status_code)


class WSAuth(WSTestBase):
    def setUp(self):
        super().setUp()

    def get_runner_config(self):
        cfg = super().get_runner_config()

        leosac_cfg_file = '{}/leosac_config_sqlite.xml' \
            .format(os.path.dirname(os.path.abspath(__file__)))
        cfg.leosac_config_file = leosac_cfg_file
        cfg.stream_log = True
        return cfg

    @check_return_code(0)
    @with_leosac_infrastructure
    async def test_default_admin_credentials(self):
        api = LeosacAPI(target=self.runner.get_ws_address())
        self.assertEqual(True, await api.authenticate('admin', 'admin'))
        await api.close()


if __name__ == '__main__':
    suite = unittest.TestSuite()
    runner_p = {'runner_factory': LeosacCachedDBRunnerFactory()}

    suite.addTest(WSAuth.create_suite(WSAuth, runner_p))
    #suite.addTest(WSGeneral.create_suite(WSGeneral, runner_p))
    unittest.TextTestRunner(verbosity=2).run(suite)
