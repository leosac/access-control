import asyncio
import json

import click
from click import UsageError

from leosacpy.cli.dev import run_tests, docker, doc, cc
from leosacpy.tools.source_formatter import SourceFormatter
from leosacpy.utils import guess_root_dir, AWAIT, pretty_dict
from leosacpy.ws import LeosacMessage
from leosacpy.wsclient import LowLevelWSClient, LeosacAPI


@click.group('dev')
@click.pass_context
def dev_cmd_group(ctx):
    """
    Command group aimed at Leosac developer.

    The `dev` family of function provides various utilities
    that Leosac developer may find useful.
    """
    if not ctx.obj.root_dir:
        raise UsageError('Leosac root directory not configured. Aborting.')


@dev_cmd_group.command(name='format-source')
@click.option('--clang-format', help='Path to clang-format')
@click.option('--exclude-dirs', '-ed',
              help='Directory (relative to REPO_ROOT/src) to exclude. '
                   'Flag can be repeated',
              multiple=True)
@click.pass_context
def dev_format_source(ctx, clang_format, exclude_dirs):
    """
    Format the source code of Leosac using clang-format.
    """
    fmt = SourceFormatter(clang_format, guess_root_dir() + '/.clang-format')
    fmt.format(guess_root_dir() + '/src', exclude_dirs=exclude_dirs)
    print(ctx.obj)


@dev_cmd_group.command('ws-msg')
@click.option('--authenticate',
              is_flag=True,
              help='Authenticate using available credential')
@click.argument('msg_type')
@click.argument('msg_content')
@click.pass_context
def dev_send_ws_msg(ctx, authenticate, msg_type, msg_content):
    host = ctx.obj.config.host
    username = ctx.obj.config.username
    password = ctx.obj.config.password

    loop = asyncio.new_event_loop()
    c = LeosacAPI(target=host)
    if authenticate:
        AWAIT(c.authenticate(username, password),
              loop)

    json_content = json.loads(msg_content)
    rep = AWAIT(c._req_rep(LeosacMessage(msg_type, json_content)),
                loop)
    click.echo('Status code: {}. Status string: {}'
               .format(rep.status_code, rep.status_string))
    click.echo('Response body:')
    click.echo(pretty_dict(rep.content))

    AWAIT(c.close(), loop)


dev_cmd_group.add_command(run_tests.run_tests)
dev_cmd_group.add_command(docker.docker)
dev_cmd_group.add_command(doc.doc)
dev_cmd_group.add_command(cc.cc)
