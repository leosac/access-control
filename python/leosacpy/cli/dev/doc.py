from subprocess import call
import click
from leosacpy.utils import guess_root_dir


# Commands in this file are added manually
# in dev.py


@click.group()
@click.pass_context
def doc(ctx):
    pass


@doc.command(name='build')
@click.pass_context
def build(ctx):
    """

    Build Leosac documentation.
    """

    call('cd {} && doxygen doc/Doxyfile'.format(ctx.obj.root_dir),
         shell=True)
