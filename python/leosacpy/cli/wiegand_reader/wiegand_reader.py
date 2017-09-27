import asyncio

import click

from leosacpy.exception import APIError
from leosacpy.utils import AWAIT, pretty_dict
from leosacpy.ws import LeosacMessage, APIStatusCode
from leosacpy.wsclient import LowLevelWSClient, LeosacAPI


@click.group('wiegand-reader')
@click.option('--host', '-h')
@click.pass_context
def wiegand_reader_cmd_group(ctx, host):
    """
    Command group to interact with Wiegand Reader module.

    """
    if host:
        ctx.obj.config.host = host


@wiegand_reader_cmd_group.command('list')
@click.pass_context
def list_gpio(ctx):
    host = ctx.obj.config.host
    username = ctx.obj.config.username
    password = ctx.obj.config.password

    loop = asyncio.new_event_loop()
    api = LeosacAPI(host)
    try:
        AWAIT(api.authenticate(username, password), loop)

        msg = LeosacMessage('wiegand-reader.read', {'reader_id': 0})
        rep = AWAIT(api._req_rep(msg), loop)
        for reader in rep.content['data']:
            click.echo(pretty_dict(reader))
    except APIError as e:
        click.echo('API Error: Status Code: {}. Message: {}'.format(
            e.status_code(), e.status_string()))
    finally:
        AWAIT(api.close(), loop)


@wiegand_reader_cmd_group.command('create')
@click.option('--name', required=True)
@click.option('--mode')
@click.option('--gpio-high', type=int)
@click.option('--gpio-low', type=int)
@click.option('--enabled', is_flag=True)
@click.pass_context
def create_reader(ctx, name, mode, gpio_high, gpio_low, enabled):
    host = ctx.obj.config.host
    username = ctx.obj.config.username
    password = ctx.obj.config.password

    loop = asyncio.new_event_loop()
    api = LeosacAPI(host)
    try:
        AWAIT(api.authenticate(username, password), loop)

        msg = LeosacMessage('wiegand-reader.create',
                            {'attributes': {
                                'name': name,
                                'mode': mode,
                                'gpio_high_id': gpio_high,
                                'gpio_low_id': gpio_low,
                                'enabled': enabled
                            }})
        rep = AWAIT(api._req_rep(msg), loop)
        if rep.status_code == APIStatusCode.SUCCESS:
            click.echo('Created Wiegand reader {}'.format(
                pretty_dict(rep.content['data']['id'])))
    except APIError as e:
        click.echo('API Error: Status Code: {}. Message: {}'.format(
            e.status_code(), e.status_string()))
    finally:
        AWAIT(api.close(), loop)
