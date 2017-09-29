import os

from leosacpy.tests.test_helper import WSTestBase, check_return_code, \
    with_leosac_infrastructure, with_leosac_ws_client, ws_authenticated_as_admin
from leosacpy.wsclient import LowLevelWSClient, APIStatusCode, LeosacMessage


class WSWiegandReader(WSTestBase):
    """
    Test the Websocket API of the Piface Digital module.
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

    async def assert_n_reader(self, n, wsclient):
        msg = LeosacMessage(message_type='wiegand-reader.read',
                            content={'reader_id': 0})
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        # We should have N GPIOs.
        self.assertEqual(n, len(rep.content['data']))

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    @ws_authenticated_as_admin
    async def test_naked_reader(self, wsclient: LowLevelWSClient = None):
        """
        Test creating a reader with no GPIO assigned to it.
        """
        await self.assert_n_reader(0, wsclient)

        # Create
        msg = LeosacMessage('wiegand-reader.create', content={
            'attributes':
                {
                    'name': 'My Reader',
                }
        })
        rep = await wsclient.req_rep(msg)
        reader_id = rep.content['data']['id']
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)

        await self.assert_n_reader(1, wsclient)

        # Read back and make sure
        msg = LeosacMessage(message_type='wiegand-reader.read', content={
            'reader_id': reader_id})
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)

        # Check attributes values
        self.assertEqual('My Reader', rep.content['data']['attributes']['name'])

        rep = await wsclient.req_rep(LeosacMessage('wiegand-reader.update',
                                     content={'reader_id': reader_id,
                                              'attributes': {
                                                  'name': 'New Name'
                                              }}))
        self.assertEqual('New Name', rep.content['data']['attributes']['name'])

        # Delete
        rep = await wsclient.req_rep(LeosacMessage('wiegand-reader.delete',
                                                   {'reader_id': reader_id}))
        await self.assert_n_reader(0, wsclient)
