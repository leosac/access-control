import logging
from types import SimpleNamespace
import click
import colorama
from click_repl import register_repl
import leosacpy.cli.dev.dev
from leosacpy.utils import guess_root_dir

# The debug flag is set globally.
# This is because when running "leosaccli --debug shell"
# we want all command running in the REPL to have DEBUG flag
# turned on.
sticky_debug_flag = None


@click.group()
@click.option('--debug/--no-debug', help='Debug mode', default=False)
@click.option('--root-dir', '-r', help='Leosac root directory')
@click.pass_context
def cli_entry_point(ctx, debug, root_dir):
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

cli_entry_point.add_command(leosacpy.cli.dev.dev.dev_cmd_group)

logging.basicConfig(level=logging.DEBUG)
register_repl(cli_entry_point, name='shell')
