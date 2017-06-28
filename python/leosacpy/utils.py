import logging
import os

import docker
import logging


def guess_root_dir() -> str:
    prev, test = None, os.path.abspath(os.path.curdir)
    while prev != test:
        if os.path.isdir(os.path.join(test, '.git')):
            return test
        prev, test = test, os.path.abspath(os.path.join(test, os.pardir))
    return ''


def get_docker_client():
    """
    Retrieve a docker client
    :return: docker client
    """
    # todo use from_env()
    client = docker.DockerClient(base_url='unix://var/run/docker.sock')
    return client


def get_docker_api_client():
    """
    Retrieve a low level docker API client.

    """
    client = docker.APIClient(base_url='unix://var/run/docker.sock')
    return client


def assert_isinstance(obj, types):
    assert isinstance(obj, types), '{} is not a {}'.format(obj, types)


class LogMixin:
    """
    A class that defines a ``logger`` property.
    """

    @property
    def logger(self):
        name = '.'.join([__name__, self.__class__.__name__])
        return logging.getLogger(name)

    def log_and_raise(self, etype, *args):
        e = etype(*args)
        self.logger.error('An error occured: {}'.format(e))
        raise e
