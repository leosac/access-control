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


AVAILABLE_IMAGES = ['main', 'main2', 'server', 'cross_compile']


def clean_output_build_line(raw_line):
    """
    Line is a dict. We returns its "stream" key and 
    remove its last \n
    """
    if 'stream' in raw_line:
        line = raw_line['stream']
        if line.endswith('\n'):
            return line[:-1]
    else:
        errcode = raw_line['errorDetail']['code']
        errstr = raw_line['errorDetail']['message']
        line = 'An error occurred: CODE: {}. MESSAGE: {}'.format(errcode, errstr)
    return line


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
    
    Build the specified ('main', 'main2', 'server', or 'cross_compile'), or all
    leosac related images.
    """
    dc = get_docker_api_client()

    for image in (images or AVAILABLE_IMAGES):
        dockerfile = 'docker/Dockerfile.{}'.format(image)
        build_output = dc.build(path=guess_root_dir(),
                                dockerfile=dockerfile,
                                decode=True,  # Better stream output
                                nocache=nocache)

        for line in build_output:
            print('Building {}: {}'.format(image,
                                           clean_output_build_line(line)))
        print('Built {}'.format(image))
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
