import logging
from time import sleep

import docker

from leosacpy.utils import get_docker_client


class LeosacFullRunner:
    def __init__(self):
        super().__init__()
        self.logger = logging.getLogger('LeosacFullRunner')
        self.docker = get_docker_client()

        i = self.docker.images.list()
        print(i)
        self.db_container = None  # type: docker.Container
        self.leosac_container = None  # type: docker.Container

    def get_ws_address(self):
        attr = self.docker.api.inspect_container(self.leosac_container.name)
        ip = attr['NetworkSettings']['IPAddress']
        return 'ws://{}:8888/'.format(ip)

    def _start_containers(self):
        self.logger.info('Starting Postgres container')
        pg = self.docker.containers.run('postgres:latest', detach=True)
        self.logger.info('Started Postgres container. Name is {}'.format(pg.name))

        self.logger.info('Starting Leosac container')
        leosac = self.docker.containers.run("leosac_server",
                                            volumes={'/tmp/test.xml': {
                                                'bind': '/leosac_config/config.xml',
                                                'mode': 'rw'}},
                                            links={
                                                pg.name: 'postgres'},
                                            detach=True)
        self.logger.info('Started Leosac container. Name is {}'.format(leosac.name))
        return pg, leosac

    def __enter__(self):
        self.db_container, self.leosac_container = self._start_containers()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type:
            return

        if self.db_container:
            print(self.db_container.logs(stdout=True))
            self.db_container.stop()
        if self.leosac_container:
            self.leosac_container.stop()
