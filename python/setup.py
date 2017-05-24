#!/usr/bin/env python

from distutils.core import setup

setup(name='leoascpy',
      version='0.1',
      description='Leosac Python Libray',
      author='xaqq',
      author_email='kapp.arno@gmail.com',
      packages=['leosacpy', 'leosacpy.cli', 'leosacpy.tools'],
      install_requires=[
          'Click',
          'click-repl',
          'Pygments',
          'docker',
          'websockets',
      ],
      entry_points='''
        [console_scripts]
        leosaccli=leosacpy.scripts.cli:cli_entry_point
    ''',
      )
