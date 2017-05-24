import os
import shutil
import subprocess

import functools

from leosacpy.utils import assert_isinstance, LogMixin


class SourceFormatter(LogMixin):
    """
    A Wrapper around clang-format to format
    Leosac source code.
    """

    def __init__(self, clang_format_bin: str, clang_format_config: str):
        """

        :param clang_format_bin: Path to clang-format binary
        :param clang_format_config: UNUSED.
        """
        if clang_format_bin is not None:
            assert_isinstance(clang_format_bin, str)
        assert_isinstance(clang_format_config, str)

        self.bin = self._find_clang_format(clang_format_bin)
        self.config = clang_format_config

        self.logger.debug('Using clang-format at: {} '
                          'with config file at: {}'.format(self.bin, self.config))

    def format(self, root_dir, extensions=None, exclude_dirs=None):
        """
        Run the format recursively from root_dir, formatting file
        with ``extensions``
        
        Directories in exclude_dirs will excluded.
        """
        if extensions is None:
            extensions = ['.hpp', 'cpp']
        if exclude_dirs is None:
            exclude_dirs = []

        abs_exclude_dirs = tuple(map(functools.partial(os.path.join, root_dir),
                                     exclude_dirs))
        self.logger.info('Will exclude directories: {}'.format(list(abs_exclude_dirs)))
        files_to_format = []
        for root, dirs, files in os.walk(root_dir):
            # Check if we should exclude that
            if root.startswith(abs_exclude_dirs):
                continue

            for f in files:
                if f.endswith(tuple(extensions)):
                    files_to_format.append(os.path.join(root, f))
        subprocess.check_call([self.bin, '-i', '-style=file'] + files_to_format)

    def _find_clang_format(self, to_check) -> str:
        cmd = shutil.which(to_check or 'clang-format-3.9')
        if not cmd:
            self.log_and_raise(RuntimeError, 'Cannot find clang-format binary')
        return cmd
