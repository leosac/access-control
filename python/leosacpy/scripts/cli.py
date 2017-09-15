import logging
from types import SimpleNamespace

import asyncio
import click
import colorama
from click_repl import register_repl
import leosacpy.cli.dev.dev
import leosacpy.cli.pfdigital.pfdigital
from leosacpy.cli.cli_config import load_config_file
from leosacpy.utils import guess_root_dir

# The debug flag is set globally.
# This is because when running "leosaccli --debug shell"
# we want all command running in the REPL to have DEBUG flag
# turned on.
from leosacpy.wsclient import LeosacAPI

sticky_debug_flag = None


@click.group()
@click.option('--debug/--no-debug', help='Debug mode', default=False)
@click.option('--root-dir', '-r', help='Leosac root directory')
@click.option('--host', '-h', help='Leosac server endpoint.')
@click.pass_context
def cli_entry_point(ctx, debug, root_dir, host):
    # Default log level to warning
    logging.getLogger().setLevel(logging.WARNING)

    colorama.init(autoreset=True)
    ctx.obj = SimpleNamespace()

    global sticky_debug_flag
    if sticky_debug_flag is None:
        print('Setting sticky_debug_flag to {}'.format(debug))
        sticky_debug_flag = debug

    ctx.obj.DEBUG = sticky_debug_flag
    logging.info('DEBUG mode: {}'.format(ctx.obj.DEBUG))

    ctx.obj.root_dir = root_dir or guess_root_dir()
    if not ctx.obj.root_dir:
        logging.warning('Running without a Leosac root directory.')

    ctx.obj.config = load_config_file()
    if host:
        ctx.obj.config.host = host


@cli_entry_point.command(name='test-credentials')
@click.argument('username')
@click.argument('password')
@click.pass_context
def test_credential(ctx, username, password):
    c = LeosacAPI(target='ws://{}'.format(ctx.obj.host))

    v = asyncio.get_event_loop().run_until_complete(
        c.authenticate(username, password))
    asyncio.get_event_loop().run_until_complete(c.close())

    print('Auth result: {}'.format(v))


@cli_entry_point.command(name='restart')
@click.pass_context
def restart(ctx):
    host = ctx.obj.config.host
    username = ctx.obj.config.username
    password = ctx.obj.config.password

    c = LeosacAPI(target=host)

    asyncio.get_event_loop().run_until_complete(c.authenticate(username, password))
    asyncio.get_event_loop().run_until_complete(c.restart())
    asyncio.get_event_loop().run_until_complete(c.close())


cli_entry_point.add_command(leosacpy.cli.dev.dev.dev_cmd_group)
cli_entry_point.add_command(leosacpy.cli.pfdigital.pfdigital.pfdigital_cmd_group)

logging.basicConfig(level=logging.DEBUG)
register_repl(cli_entry_point, name='shell')
