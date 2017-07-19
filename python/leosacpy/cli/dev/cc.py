import os
import subprocess
from types import SimpleNamespace

import click

# The "cc" command requires a build-dir parameter
# that it adds to the context for subcommand to use.
from leosacpy.utils import get_docker_client, guess_root_dir


# Commands in this file are added manually
# in dev.py


@click.group()
@click.pass_context
@click.option('--build-dir',
              type=str,
              required=True,
              nargs=1)
def cc(ctx, build_dir):
    """
    The cc (cross-compile) command is a frontend to the
    leosac_cross_compile container.
    """
    ctx.obj.cc = SimpleNamespace()

    abs_build_dir = os.path.abspath(build_dir)
    if not os.path.isdir(build_dir):
        print('Specified build directory {} is not valid.'.format(abs_build_dir))
        return
    ctx.obj.cc.build_dir = abs_build_dir


def start_cc_container(build_dir, repo_root_dir, command,
                       bash_entrypoint=False):
    """
    Helper function that start an instance of the Leosac
    Cross Compile container and return an handle to it.

    :param bash_entrypoint: If true, use /bin/bash as entrypoint instead of
    the default script.
    :param build_dir: Directory on the host where to perform the build.
    :param repo_root_dir: Directory where the root of Leosac repository lives.
    :param command: The command to pass to the container
    """

    assert isinstance(build_dir, str)
    assert isinstance(repo_root_dir, str)
    assert isinstance(command, (list, str)), 'Is {} instead'.format(type(command))

    volumes_cfg = {
        build_dir: {
            'bind': '/leosac_arm_build',
            'mode': 'rw'
        },
        repo_root_dir: {
            'bind': '/leosac',
            'mode': 'ro'
        }
    }
    if not bash_entrypoint:
        return get_docker_client().containers.run('leosac_cross_compile',
                                                  command,
                                                  volumes=volumes_cfg,
                                                  detach=True)
    else:
        return get_docker_client().containers.run('leosac_cross_compile',
                                                  command,
                                                  volumes=volumes_cfg,
                                                  detach=True,
                                                  entrypoint=['/bin/bash', '-c'])


@cc.command()
@click.pass_context
def cmake(ctx):
    """
    Run the CMake command to prepare the build
    directory.
    """
    build_dir = ctx.obj.cc.build_dir
    assert isinstance(build_dir, str)

    c = start_cc_container(build_dir, guess_root_dir(), 'cmake')
    print('Build output:')
    for log_line in c.logs(stream=True, stdout=True, stderr=True):
        print(log_line.decode('utf8'), end='')


@cc.command()
@click.pass_context
def make(ctx):
    """
    Run `make` to build Leosac.
    """
    build_dir = ctx.obj.cc.build_dir
    assert isinstance(build_dir, str)

    c = start_cc_container(build_dir, guess_root_dir(), 'build')
    print('Build output:')
    for log in c.logs(stream=True, stdout=True, stderr=True):
        print(log.decode('utf8'), end='')


@cc.command()
@click.option('--host', '-h', required=True)
@click.option('--all', '-a',
              help='Deploy everything (leosac + fakeroot)',
              default=False)
@click.pass_context
def deploy(ctx, host, all):
    """

    Deploy (SCP) the build to a given host.
    The build will be SCP'd into /opt/leosac_fakeroot (for libraries)
    and /opt/leosac for leosac.
    """
    build_dir = ctx.obj.cc.build_dir
    assert isinstance(build_dir, str)

    if all:
        cmd = ['deploy_all', host]
    else:
        cmd = ['deploy', host]
    c = start_cc_container(build_dir, guess_root_dir(), cmd)
    print('Build output:')
    for log_line in c.logs(stream=True, stdout=True, stderr=True):
        print(log_line.decode('utf8'), end='')


@cc.command('make-tarball')
@click.pass_context
def make_tarball(ctx):
    """
    Make a tarball of the build output.
    """
    build_dir = ctx.obj.cc.build_dir
    assert isinstance(build_dir, str)

    tar_cmd = "'tar cvf /leosac_arm_build/release_fakeroot.tar /opt/rpi_fakeroot'"
    c = start_cc_container(build_dir, guess_root_dir(),
                           tar_cmd,
                           bash_entrypoint=True)
    print('Build output:')
    for log in c.logs(stream=True, stdout=True, stderr=True):
        print(log.decode('utf8'), end='')
