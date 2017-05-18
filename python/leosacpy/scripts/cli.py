import click
from click_repl import register_repl


@click.group()
def cli():
    pass


@cli.group()
def db():
    pass


@db.command()
@click.option('--name', help='db name')
@click.option('--type', '-t', help='db type')
@click.argument('size')
def create(size, **kwargs):
    print('Create DB {}. Size: {}. Type {}.'.format(kwargs['name'], size, kwargs['type']))


@db.command()
def drop():
    print('Drop DB')


register_repl(cli, name='shell')
