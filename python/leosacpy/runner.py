import logging
import os
import time
import asyncio
import docker
import functools

from leosacpy.utils import get_docker_client


class RunnerConfig:
    """
    Hold some configuration for the runner objects.
    """

    def __init__(self,
                 loop: asyncio.AbstractEventLoop = None,
                 log_dir='/tmp',
                 leosac_config_file: str = '/tmp/test.xml'):
        self.log_dir = log_dir

        # Write container stderr/stdout to file.
        self.log_containers = True

        self.leosac_config_file = leosac_config_file

        self.loop = loop

        # Used to put log in a subdirectory of log_dir.
        self.fully_qualified_test_name = ''


class LeosacFullRunner:
    """
    Spawn docker container for Leosac.
    
    This class must be object in code that use an asyncio
    event loop because it relies on it to stream container
    log to file.    
    """

    def __init__(self, cfg: RunnerConfig):
        assert isinstance(cfg, RunnerConfig), 'Wrong config type'

        self.cfg = cfg
        self.logger = logging.getLogger('LeosacFullRunner')
        self.docker = get_docker_client()

        # A future that must be awaited in __aexit__ if its not None
        # This future represents the stream of containers logs
        self.containers_log_future = None

        self.db_container = None  # type: docker.Container
        self.leosac_container = None  # type: docker.Container

    def get_ws_address(self):
        self.leosac_container.reload()
        ip = self.leosac_container.attrs['NetworkSettings']['IPAddress']
        return 'ws://{}:8888/'.format(ip)

    def _stream_containers_logs(self):
        """
        Setup code to stream container logs to file.
        
        :return: A future that will resolve once containers are stopped. 
        """

        def _stream_for_container(container, container_type: str):
            docker_log_generator = container.logs(stream=True, stdout=True,
                                                  stderr=True)

            # Configure Python logger to log to file.
            logger = logging.getLogger('container.{}'.format(container_type))

            target_log_file = '{}/{}/{}'.format(self.cfg.log_dir,
                                                self.cfg.fully_qualified_test_name,
                                                container_type)
            complete_log_directory = os.path.dirname(target_log_file)
            # Create log directory if needed
            if not os.path.exists(complete_log_directory):
                os.makedirs(complete_log_directory, exist_ok=True)

            logging_handler = logging.FileHandler(target_log_file)
            logging_handler.setLevel(logging.DEBUG)
            logger.addHandler(logging_handler)

            # Iterate on log entries.
            while True:
                for log_line in docker_log_generator:
                    logger.info(log_line)
                time.sleep(2)
                container.reload()
                self.logger.info('Current status {}'.format(container.status))
                if container.status == 'exited':
                    break
            self.logger.info('Read log done.')

        c0 = self.cfg.loop.run_in_executor(None,
                                           functools.partial(_stream_for_container,
                                                             self.leosac_container,
                                                             'leosac'))
        c1 = self.cfg.loop.run_in_executor(None,
                                           functools.partial(_stream_for_container,
                                                             self.db_container,
                                                             'postgres'))
        self.containers_log_future = asyncio.gather(c0, c1)

    def _start_containers(self):
        self.logger.info('Starting Postgres container')
        pg = self.docker.containers.run('postgres:latest', detach=True)
        self.logger.info('Started Postgres container. Name is {}'.format(pg.name))

        self.logger.info('Starting Leosac container')
        leosac = self.docker.containers.run("leosac_server",
                                            volumes={self.cfg.leosac_config_file: {
                                                'bind': '/leosac_config/config.xml',
                                                'mode': 'rw'}},
                                            links={
                                                pg.name: 'postgres'},
                                            detach=True)
        self.logger.info('Started Leosac container. Name is {}'.format(leosac.name))
        return pg, leosac

    async def __aenter__(self):
        self.db_container, self.leosac_container = self._start_containers()
        self._stream_containers_logs()
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        try:
            if self.db_container:
                self.logger.info('Stopping PGSQL container: {}'
                                 .format(self.db_container.name))
                self.db_container.stop()
        except Exception as e:
            self.logger.error('Exception while stopping PGSQL container: {}'
                              .format(e))

        try:
            if self.leosac_container:
                self.logger.info('Stopping Leosac container: {}'
                                 .format(self.leosac_container.name))
                self.leosac_container.stop()
        except Exception as e:
            self.logger.error('Exception while stopping PGSQL container: {}'
                              .format(e))

        if self.containers_log_future:
            await self.containers_log_future
        self.docker = None
