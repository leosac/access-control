import click

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
    line = raw_line['stream']
    if line.endswith('\n'):
        return line[:-1]
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
