import os

from leosacpy.tests.test_helper import WSTestBase, check_return_code, \
    with_leosac_infrastructure, with_leosac_ws_client, ws_authenticated_as_admin
from leosacpy.ws import ZoneType
from leosacpy.wsclient import LowLevelWSClient, APIStatusCode, LeosacMessage


class WSPifaceDigital(WSTestBase):
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

    async def assert_n_gpio(self, n, wsclient):
        msg = LeosacMessage(message_type='pfdigital.gpio.read',
                            content={'gpio_id': 0})
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        # We should have N GPIOs.
        self.assertEqual(n, len(rep.content['data']))

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    @ws_authenticated_as_admin
    async def test_CRUD_gpio(self, wsclient: LowLevelWSClient = None):
        await self.assert_n_gpio(0, wsclient)

        # Create
        msg = LeosacMessage('pfdigital.gpio.create', content={
            'attributes':
                {
                    'name': 'My GPIO',
                    'hardware_address': 0
                }
        })
        rep = await wsclient.req_rep(msg)
        gpio_id = rep.content['data']['id']
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)

        await self.assert_n_gpio(1, wsclient)

        # Read back and make sure
        msg = LeosacMessage(message_type='pfdigital.gpio.read', content={'gpio_id': gpio_id})
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)

        # Check attributes values
        self.assertEqual('My GPIO', rep.content['data']['attributes']['name'])
        self.assertEqual(0, rep.content['data']['attributes']['hardware_address'])

        rep = await wsclient.req_rep(LeosacMessage('pfdigital.gpio.update',
                                     content={'gpio_id': gpio_id,
                                              'attributes': {
                                                  'hardware_address': 42
                                              }}))
        self.assertEqual('My GPIO', rep.content['data']['attributes']['name'])
        self.assertEqual(42, rep.content['data']['attributes']['hardware_address'])

        # Delete
        rep = await wsclient.req_rep(LeosacMessage('pfdigital.gpio.delete', {'gpio_id': gpio_id}))
        await self.assert_n_gpio(0, wsclient)
