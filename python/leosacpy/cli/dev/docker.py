import asyncio
import click
import colorama
import logging

from leosacpy.runner import RunnerConfig, LeosacFullRunner
from leosacpy.utils import guess_root_dir, get_docker_api_client


# Commands in this file are added manually
# in dev.py


@click.group()
@click.pass_context
def docker(ctx):
    pass


AVAILABLE_IMAGES = ['main', 'server', 'cross_compile']


def clean_output_build_line(raw_line):
    """
    Line is a dict.

    We returns a tuple of (bool, str) that means: (hasError, cleanString)
    """
    if 'stream' in raw_line:
        line = raw_line['stream']
        if line.endswith('\n'):
            return True, line[:-1]
        return True, line
    elif 'errorDetail' in raw_line:
        print(raw_line)
        errcode = raw_line['errorDetail'].get('code', 'N/A')
        errstr = raw_line['errorDetail']['message']
        line = 'An error occurred: CODE: {}. MESSAGE: {}'.format(errcode, errstr)
        return False, line
    elif 'status' in raw_line:
        line = raw_line['status']
        if line.endswith('\n'):
            line = line[:-1]
        return False, line
    return True, "LEOSAC_DOCKER_UNKNOWN_MESSAGE"


@docker.command(name='build')
@click.argument('images',
                nargs=-1,
                type=click.Choice(AVAILABLE_IMAGES))
@click.option('--nocache',
              is_flag=True,
              help='Force a fresh build, not using the docker cache.')
@click.pass_context
def build(ctx, images, nocache):
    """

    Build the specified ('main', 'server', or 'cross_compile'), or all
    leosac related images.
    """
    dc = get_docker_api_client()

    for image in (images or AVAILABLE_IMAGES):
        dockerfile = 'docker/cli/Dockerfile.{}'.format(image)
        build_output = dc.build(path=guess_root_dir(),
                                dockerfile=dockerfile,
                                tag='leosac_{}'.format(image),
                                decode=True,  # Better stream output
                                nocache=nocache)

        success = True
        for line in build_output:
            has_error, clean_line = clean_output_build_line(line)
            if not has_error:
                success = False
            print('Building {}: {}'.format(image,
                                           clean_line))

        if success:
            print('Built {}'.format(image))
        else:
            print('Error building {}'.format(image))
    pass


@docker.command()
@click.option('--with-pgsql',
              is_flag=True,
              help='Run a PGSQL container alongside the Leosac container. '
                   'This is required for now.')
@click.argument('config_file')
@click.pass_context
def run(ctx, with_pgsql, config_file):
    """
    Run the server Leosac container.

    Optionally, this server container can be started alongside
    a PostgreSQL container in order to run a full Leosac stack.
    """
    asyncio.get_event_loop().run_until_complete(_run(with_pgsql,
                                                     config_file))


async def _run(with_pgsql, config_file):
    loop = asyncio.get_event_loop()
    if with_pgsql:
        cfg = RunnerConfig(loop=loop,
                           leosac_config_file=config_file)
        async with LeosacFullRunner(cfg) as r:
            print(colorama.Fore.RED +
                  'Leosac server WS address: {}'.format(r.get_ws_address()))
            while True:
                # Just hit CTRL+C at some point to quit.
                try:
                    await asyncio.sleep(10)
                except KeyboardInterrupt:
                    break
    else:
        print('Sorry not supported yet.')
