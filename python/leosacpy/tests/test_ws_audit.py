import os
import unittest

from leosacpy.runner import LeosacCachedDBRunnerFactory
from leosacpy.tests.test_helper import WSTestBase, check_return_code, \
    with_leosac_infrastructure, with_leosac_ws_client, ws_authenticated_as_admin
from leosacpy.wsclient import LowLevelWSClient, APIStatusCode, LeosacMessage


class WSAudit(WSTestBase):
    """
    Test the Audit Log API.
    """

    def setUp(self):
        super().setUp()

    def get_runner_config(self):
        cfg = super().get_runner_config()

        leosac_cfg_file = '{}/leosac_config.xml' \
            .format(os.path.dirname(os.path.abspath(__file__)))
        cfg.leosac_config_file = leosac_cfg_file
        cfg.stream_log = True
        return cfg

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    @ws_authenticated_as_admin
    async def test_get_audit_negative_page(self, wsclient: LowLevelWSClient = None):
        msg = LeosacMessage(message_type='audit.get', content={
            'p': -1,
            'ps': 25,
            'enabled_types': ['Leosac::Audit::WSAPICall']
        })
        # Request negative page size. Expect INVALID_ARGUMENT
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.INVALID_ARGUMENT, rep.status_code)

        msg = LeosacMessage(message_type='audit.get', content={
            'p': 1,
            'ps': 25,
            'enabled_types': ['Leosac::Audit::WSAPICall']
        })
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)


if __name__ == '__main__':
    suite = unittest.TestSuite()
    suite.addTest(WSAudit.create_suite(WSAudit,
                                       {
                                           'runner_factory': LeosacCachedDBRunnerFactory()}
                                       ))
    unittest.TextTestRunner(verbosity=2).run(suite)
