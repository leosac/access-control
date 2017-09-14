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

    @check_return_code(0)
    @with_leosac_infrastructure
    @with_leosac_ws_client()
    @ws_authenticated_as_admin
    async def test_create_gpio(self, wsclient: LowLevelWSClient = None):
        msg = LeosacMessage(message_type='pfdigital.gpio.read', content={'gpio_id': 0})
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        # We should have 0 GPIOs.
        self.assertEqual(0, len(rep.content['data']))

        msg = LeosacMessage('pfdigital.gpio.create', content={
            'attributes':
                {
                    'name': 'My GPIO'
                }
        })
        rep = await wsclient.req_rep(msg)
        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)

        msg = LeosacMessage(message_type='pfdigital.gpio.read', content={'gpio_id': 0})
        rep = await wsclient.req_rep(msg)

        self.assertEqual(APIStatusCode.SUCCESS, rep.status_code)
        # Now we should have one zone.
        self.assertEqual(1, len(rep.content['data']))

        self.assertEqual('My GPIO', rep.content['data'][0]['attributes']['name'])
        self.assertEqual(0, rep.content['data'][0]['attributes']['type'])
