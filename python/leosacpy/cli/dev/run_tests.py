import unittest

import click

from leosacpy.runner import LeosacCachedDBRunnerFactory, LeosacFullRunner
from leosacpy.tests.test_ws_general import WSGeneral


# Commands in this file are added manually
# in dev.py


@click.command(name='run-tests')
@click.option('--runner', '-r', type=click.Choice(['full', 'shareddb']))
@click.pass_context
def run_tests(ctx, runner):
    suite = unittest.TestSuite()

    if runner == 'full' or runner is None:
        def runner_factory(cfg): return LeosacFullRunner(cfg)
    elif runner == 'shareddb':
        runner_factory = LeosacCachedDBRunnerFactory()

    suite.addTest(WSGeneral.create_suite(WSGeneral,
                                         {'runner_factory': runner_factory}
                                         ))
    unittest.TextTestRunner(verbosity=2).run(suite)
