#!/usr/bin/env python

from distutils.core import setup

setup(name='leosacpy',
      version='0.1',
      description='Leosac Python Libray',
      author='Leosac',
      author_email='support@leosac.com',
      packages=['leosacpy', 'leosacpy.cli', 'leosacpy.tools'],
      install_requires=[
          'Click',
          'click-repl',
          'Pygments',
          'docker',
          'websockets',
          'colorama',
      ],
      entry_points='''
        [console_scripts]
        leosaccli=leosacpy.scripts.cli:cli_entry_point
        leosacws=leosacpy.scripts.ws_shell:cli
    ''',
      )
