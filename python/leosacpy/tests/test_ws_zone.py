import os
import unittest

from leosacpy.runner import LeosacCachedDBRunnerFactory
from leosacpy.tests.test_helper import WSTestBase, check_return_code, \
    with_leosac_infrastructure, with_leosac_ws_client, ws_authenticated_as_admin
from leosacpy.wsclient import LowLevelWSClient, APIStatusCode, LeosacMessage


class WSZone(WSTestBase):
    """
    Test the Leosac Zone API.
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
    async def test_create_zone(self, wsclient: LowLevelWSClient = None):
        msg = LeosacMessage(message_type='zone.read', content={'zone_id': 0})
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        # We should have 0 zones.
        self.assertEqual(0, len(rep.content['data']))

        msg = LeosacMessage(message_type='zone.create', content={
            'attributes': {
                'alias': 'My Zone',
                'type': 0,
                'description': 'A very nice zone',
                'doors': [],
                'children': []
            }
        })
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)

        msg = LeosacMessage(message_type='zone.read', content={'zone_id': 0})
        rep = await wsclient.req_rep(msg)

        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        # Now we should have one zone.
        self.assertEqual(1, len(rep.content['data']))

        self.assertEqual('My Zone', rep.content['data'][0]['attributes']['alias'])


if __name__ == '__main__':
    suite = unittest.TestSuite()
    suite.addTest(WSZone.create_suite(WSZone,
                                      {
                                          'runner_factory': LeosacCachedDBRunnerFactory()}
                                      ))
    unittest.TextTestRunner(verbosity=2).run(suite)
