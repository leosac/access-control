import logging
import os
import unittest

from leosacpy.exception import APIError, APIModelException
from leosacpy.runner import LeosacCachedDBRunnerFactory
from leosacpy.tests.test_helper import WSTestBase, check_return_code, \
    with_leosac_infrastructure, with_leosac_ws_client, ws_authenticated_as_admin
from leosacpy.ws import ZoneType
from leosacpy.wsclient import LowLevelWSClient, APIStatusCode, LeosacMessage, \
    LeosacAPI


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

    @staticmethod
    def get_create_zone_msg():
        return LeosacMessage(message_type='zone.create', content={
            'attributes': {
                'alias': 'My Zone',
                'type': ZoneType.PHYSICAL,
                'description': 'A very nice zone',
                'doors': [],
                'children': []
            }
        })

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

        msg = self.get_create_zone_msg()
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)

        msg = LeosacMessage(message_type='zone.read', content={'zone_id': 0})
        rep = await wsclient.req_rep(msg)

        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        # Now we should have one zone.
        self.assertEqual(1, len(rep.content['data']))

        self.assertEqual('My Zone', rep.content['data'][0]['attributes']['alias'])
        self.assertEqual(0, rep.content['data'][0]['attributes']['type'])

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    @ws_authenticated_as_admin
    async def test_search_zone(self, wsclient: LowLevelWSClient = None):
        msg = LeosacMessage('search.zone_alias', content={'partial_name': '%'})
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        # This should return an empty array as we have no zone.
        self.assertEqual(0, len(rep.content))

        # Create a dummy zone.
        msg = self.get_create_zone_msg()
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)

        # Search again
        msg = LeosacMessage('search.zone_alias', content={'partial_name': '%'})
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        self.assertEqual(1, len(rep.content))

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    @ws_authenticated_as_admin
    async def test_zone_invalid_type(self, wsclient: LowLevelWSClient = None):
        api = LeosacAPI(client=wsclient)

        with self.assertRaises(APIModelException):
            await api.zone_create('My Zone', 42, 'A bugged zone')
        await api.close()

#    @check_return_code(0)
#    @with_leosac_infrastructure
#    @with_leosac_ws_client()
#    @ws_authenticated_as_admin
#    async def test_zone_multi_physical_parent(self, wsclient: LowLevelWSClient = None):
#        api = LeosacAPI(client=wsclient)
#        x = await api.zone_create('Parent Zone', ZoneType.PHYSICAL, 'My parent zone')


if __name__ == '__main__':
    suite = unittest.TestSuite()
    suite.addTest(WSZone.create_suite(WSZone,
                                      {
                                          'runner_factory': LeosacCachedDBRunnerFactory()}
                                      ))
    unittest.TextTestRunner(verbosity=2).run(suite)
