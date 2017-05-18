from contextlib import contextmanager

import docker


def get_docker_client():
    """
    Retrieve a docker client
    :return: docker client
    """
    # todo use from_env()
    client = docker.DockerClient(base_url='unix://var/run/docker.sock')
    return client

@contextmanager
def timeout(duration):
    pass