import logging
import os
import time
import asyncio
from threading import Thread

import docker
import functools

from leosacpy.utils import get_docker_client, LogMixin


class RunnerConfig:
    """
    Hold some configuration for the runner objects.
    """

    def __init__(self,
                 loop: asyncio.AbstractEventLoop = None,
                 log_dir='/tmp',
                 leosac_config_file: str = '/tmp/test.xml'):
        self.log_dir = log_dir

        # Stream container log to stdout
        self.stream_log = True

        self.leosac_config_file = leosac_config_file

        self.loop = loop

        # Used to put log in a subdirectory of log_dir.
        self.fully_qualified_test_name = ''


class Runner:
    def get_ws_address(self) -> str:
        """
        Returns the websocket address of the Leosac server managed
        by the runner.
        """
        raise NotImplementedError()

    def get_return_code(self) -> int:
        """
        Returns the return code of Leosac server managed by the runner.
        """
        raise NotImplementedError()


class RunnerFactory:
    def get_runner(self, cfg: RunnerConfig):
        """
        Retrieve a Runner object from a RunnerConfig.
        
        Subclass implementation are allowed to cache the Runner 
        for reuse. This is how the LeosacCachedDBRunner can works.
        """
        raise NotImplementedError()


class LeosacCachedDBRunnerCache:
    pass


class DockerContainerWrapper(LogMixin):
    """
    A little wrapper around docker-py container.
    
    It provides an asyncio aware way to read the containers
    log
    
    :param container_type: What kind of container ("Leosac", "Postgres")
    """

    def __init__(self, container_type, docker_client):
        self.container_type = container_type
        self.docker_client = docker_client
        self.container = None
        self.name = ''

    def do_run_detach(self, image, stream_log=True, *args, **kwargs):
        assert self.container is None, 'Container is not None'

        # Force detached mode
        kwargs['detach'] = True

        self.logger.info('Starting {} container.'.format(self.container_type))
        self.container = self.docker_client.containers.run(image,
                                                           *args,
                                                           **kwargs)
        self.name = self.container.name
        self.logger.info('Container {} started. '
                         'Name is {}'.format(self.container_type,
                                             self.container.name))

        if stream_log:
            self._stream_log()

    def stop(self):
        """
        Stop the container.
        
        This also stop log streaming.
        """
        assert self.container, 'No container'

        try:
            self.logger.info('Stopping {} container {}'.format(self.container_type,
                                                               self.name))
            self.container.stop()
            self.logger.info('Container {} stopped'.format(self.container.name))
        except Exception as e:
            self.logger.error('Exception while stopping {} container {}: {}'
                              .format(self.container_type, self.name, e))
        finally:
            # Todo store container log.
            pass

    def _stream_log(self):
        """
        Start streaming container log.
        
        We use a background thread because the dockerpy generator is blocking.
        We stop only when the container has exited.
        """
        assert self.container, 'No container'
        g = self.container.logs(stream=True, stdout=True,
                                stderr=True)

        self.logger.info('Starting streaming log for {} '
                         'container {}'.format(self.container_type,
                                               self.name))

        def stream_log_helper(self):
            while True:
                for line in g:
                    self.logger.debug(line)
                # Wait a bit for more log to come.
                time.sleep(1)
                # Reload to have update status
                self.container.reload()
                if self.container.status == 'exited':
                    self.logger.info('Stream log of {} container '
                                     '{} is now over'.format(self.container_type,
                                                             self.name))
                    break

        t = Thread(daemon=True,
                   target=functools.partial(stream_log_helper, self))
        t.start()

    def get_return_code(self):
        """
        Retrieve the return code of the container.

        If the container is not exited, raise an error.
        """
        assert self.container, 'No container'
        self.container.reload()
        if not self.container.status == 'exited':
            raise RuntimeError('Container not exited.')
        return self.container.attrs['State']['ExitCode']


class LeosacContainer(DockerContainerWrapper):
    """
    An abstraction around the Docker Leosac container.
    
    The benefits of this is to be able to easily switch the underlying
    build (leosac binary) that runs. For example valgrind run, ASAN or TSAN, etc.
    """

    def __init__(self, config_file_path, docker_client):
        super().__init__('Leosac', docker_client)
        self.config_file_path = config_file_path

    def run_detach(self, pg_container, stream_log=True):
        """
        Run a Leosac container in detached mode.
        
        :param pg_container: An optional container object that will be --bind
         to the leosac container as the 'postgres' container.
        :return: 
        """
        kw = {'volumes': {self.config_file_path: {
            'bind': '/leosac_config/config.xml',
            'mode': 'rw'}},
            'detach': True}
        if pg_container:
            kw['links'] = {pg_container.name: 'postgres'}

        kw['stream_log'] = stream_log
        self.do_run_detach('leosac_server', **kw)

    def get_ws_address(self):
        self.container.reload()
        ip = self.container.attrs['NetworkSettings']['IPAddress']
        # Todo: Check that port match in config file maybe ?
        return 'ws://{}:8888/'.format(ip)


class LeosacCachedDBRunner(Runner, LogMixin):
    """
    A runner that cache the postgres container
    and reuse it between test.
    
    The runner is constructed by passed cache object 
    """

    def get_ws_address(self) -> str:
        assert self.leosac_container, 'No container'
        return self.leosac_container.get_ws_address()

    def get_return_code(self) -> int:
        assert self.leosac_container, 'No container'
        return self.leosac_container.get_return_code()

    def __init__(self, cfg: RunnerConfig, cache=None):
        assert isinstance(cfg, RunnerConfig), 'Wrong config type'

        self.cfg = cfg
        self.docker = get_docker_client()
        self.cache = cache  # type: Dict

        # A future that must be awaited in __aexit__ if its not None
        # This future represents the stream of containers logs
        self.containers_log_future = None

        self.db_container = None  # type: DockerContainerWrapper
        if self.cache is not None:
            self.db_container = self.cache.get('db_container')
        self.leosac_container = None  # type: LeosacContainer

    def _cache_db_container(self, db_container):
        if self.cache is not None:
            self.cache['db_container'] = db_container

    def _clean_db_if_needed(self):
        pass

    async def __aenter__(self):
        if self.db_container is None:
            self.logger.info('Will create the Postgres container for the first time.')
            self.db_container = DockerContainerWrapper('Postgres', self.docker)
            self.db_container.do_run_detach('postgres:latest', stream_log=self.cfg.stream_log)
            self._cache_db_container(self.db_container)
        else:
            # Reused from cache. Need to clean DB.
            self.logger.info('Reusing Postgres container from cache.')
            self._clean_db_if_needed()

        self.leosac_container = LeosacContainer(self.cfg.leosac_config_file, self.docker)
        self.leosac_container.run_detach(self.db_container, stream_log=self.cfg.stream_log)

        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        self.leosac_container.stop()
        # Note that we do not stop the db container


class LeosacCachedDBRunnerFactory(RunnerFactory):
    def __init__(self):
        self.cache = {}

    def get_runner(self, cfg: RunnerConfig):
        return LeosacCachedDBRunner(cfg, cache=self.cache)

    def __call__(self, *args, **kwargs):
        return self.get_runner(*args, **kwargs)


class LeosacFullRunner(Runner):
    """
    Spawn docker container for Leosac.
    
    This class must be used with code that use an asyncio
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

    def get_ws_address(self) -> str:
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

    def get_return_code(self):
        """
        Retrieve the return code of the leosac container.
        
        If the container is not exited, raise an error.
        """
        assert self.leosac_container, 'No leosac container'
        if not self.leosac_container.status == 'exited':
            raise RuntimeError('Container not exited.')
        self.leosac_container.reload()
        return self.leosac_container.attrs['State']['ExitCode']
