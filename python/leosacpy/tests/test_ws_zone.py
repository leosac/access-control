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

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    @ws_authenticated_as_admin
    async def test_zone_multiple_physical_parent(self,
                                                 wsclient: LowLevelWSClient = None):
        """
        A zone cannot have multiple physical parent. Test that the server
        refuse those.
        """
        api = LeosacAPI(client=wsclient)

        child_id = await api.zone_create('Child', ZoneType.PHYSICAL, '')
        parent_id = await api.zone_create('Parent', ZoneType.PHYSICAL, '',
                                          children=[child_id])

        with self.assertRaises(APIModelException):
            # This will add a second parent to Child zone, and should fail.
            await api.zone_create('SecondParent', ZoneType.PHYSICAL, '',
                                  children=[child_id])

        await api.close()

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    @ws_authenticated_as_admin
    async def test_zone_no_cycle(self,
                                 wsclient: LowLevelWSClient = None):
        """
        A zone hierarchy cannot have cycle.

        The following should be rejected:
            Z0 parentOf Z1
            Z1 parentOf Z2
            Z2 parentOf Z0
        """
        api = LeosacAPI(client=wsclient)

        z2 = await api.zone_create('Zone2', ZoneType.LOGICAL, '')
        z1 = await api.zone_create('Zone1', ZoneType.LOGICAL, '', children=[z2])
        z0 = await api.zone_create('Zone0', ZoneType.LOGICAL, '',
                                   children=[z1])

        with self.assertRaises(APIModelException):
            # This will add a second parent to Child zone, and should fail.
            await api.zone_edit(z2, 'Zone2', ZoneType.LOGICAL, '',
                                children=[z0])
        await api.close()


if __name__ == '__main__':
    suite = unittest.TestSuite()
    suite.addTest(WSZone.create_suite(WSZone,
                                      {
                                          'runner_factory': LeosacCachedDBRunnerFactory()}
                                      ))
    unittest.TextTestRunner(verbosity=2).run(suite)
