import logging
import os
import subprocess
from types import SimpleNamespace

import click

# The "cc" command requires a build-dir parameter
# that it adds to the context for subcommand to use.
from click import UsageError

from leosacpy.utils import get_docker_client, guess_root_dir


# Commands in this file are added manually
# in dev.py


@click.group()
@click.pass_context
@click.option('--build-dir',
              type=str,
              nargs=1,
              help='Directory that will be mounted as build root. '
                   'Defaults to /tmp/leosac_cc_build')
def cc(ctx, build_dir):
    """
    The cc (cross-compile) command is a frontend to the
    leosac_cross_compile container.
    """
    ctx.obj.cc = SimpleNamespace()

    if build_dir is None:
        build_dir = '/tmp/leosac_cc_build'
        try:
            os.mkdir(build_dir)
        except FileExistsError:
            pass

    abs_build_dir = os.path.abspath(build_dir)
    if not os.path.isdir(build_dir):
        raise UsageError(
            'Specified build directory {} is not valid.'.format(abs_build_dir))

    ctx.obj.cc.build_dir = abs_build_dir


def run_in_container(build_dir, repo_root_dir, command,
                     deploy_key=None):
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

    if deploy_key:
        volumes_cfg[deploy_key] = {
            'bind': '/ssh_deploy_key',
            'mode': 'ro'
        }

    c = get_docker_client().containers.run('leosac_cross_compile',
                                           '"' + command + '"',
                                           volumes=volumes_cfg,
                                           detach=True,
                                           entrypoint=['/bin/bash', '-c'])
    print('Build output:')
    for log_line in c.logs(stream=True, stdout=True, stderr=True):
        print(log_line.decode('utf8'), end='')


@cc.command()
@click.pass_context
def cmake(ctx):
    """
    Run the CMake command to prepare the build
    directory.
    """
    build_dir = ctx.obj.cc.build_dir
    assert isinstance(build_dir, str)

    run_in_container(build_dir, guess_root_dir(),
                     'cd /leosac_arm_build; '
                     'cmake -DCMAKE_SYSROOT=/opt/rpi_fakeroot '
                     '-DCMAKE_BUILD_TYPE=Debug  '
                     '-DCMAKE_TOOLCHAIN_FILE=/leosac/cmake/rpi-cross.cmake /leosac')


@cc.command()
@click.option('--job', '-j',
              type=int,
              help='Number of make job.')
@click.pass_context
def make(ctx, job):
    """
    Run `make` to build Leosac.
    """
    build_dir = ctx.obj.cc.build_dir
    assert isinstance(build_dir, str)

    nb_job = job or 4
    run_in_container(build_dir, guess_root_dir(),
                     'cd /leosac_arm_build && make -j{}'.format(nb_job))


@cc.command(name='dev-push',
            short_help='Deploy build to remote host.')
@click.option('--host', '-h', required=True,
              help='Address of the remote host')
@click.option('--directory', '-d', required=True,
              help='Target directory on the remote host.')
@click.option('--key', '-k', required=True,
              help='Path to the SSH key to connect to host.')
@click.option('--user', '-u', required=True,
              help='SSH user to log with.')
@click.pass_context
def dev_push(ctx, host, directory, key, user):
    """
    Deploy (SCP) the build binaries to a given host.
    Note that only Leosac binary and libraries are pushed, not cross-compiled
    system libraries.
    """
    build_dir = ctx.obj.cc.build_dir
    opt = '-a --delete -r -v -e \\"ssh -o StrictHostKeyChecking=no -i /ssh_deploy_key\\" '
    cmd = 'rsync {} /leosac_arm_build/*.so /leosac_arm_build/leosac {}@{}:{}'. \
        format(opt, user, host, directory)
    run_in_container(build_dir, guess_root_dir(), cmd, deploy_key=key)


@cc.command('fakeroot-package')
@click.pass_context
def fakeroot(ctx):
    """
    Make a tarball of fakeroot (fixed after container is built)
    """
    build_dir = ctx.obj.cc.build_dir
    assert isinstance(build_dir, str)

    # Copy current libstdc++ to the fakeroot
    add_libstd_cmd = 'cp /usr/arm-linux-gnueabihf/lib/libstdc++.so.6.0.22 ' \
                     '/opt/rpi_fakeroot/lib && ' \
                     'ln -s /opt/rpi_fakeroot/lib/libstdc++.so.6.0.22 ' \
                     '/opt/rpi_fakeroot/lib/libstdc++.so.6 '
    tar_cmd = 'tar cvf /leosac_arm_build/fakeroot.tar /opt/rpi_fakeroot'
    cmd = add_libstd_cmd + ' && ' + tar_cmd
    run_in_container(build_dir, guess_root_dir(), cmd)


@cc.command('package')
@click.pass_context
def package(ctx):
    build_dir = ctx.obj.cc.build_dir

    run_in_container(build_dir, guess_root_dir(),
                     'cd /leosac_arm_build && make package')
