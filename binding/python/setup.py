"""setup ddls package. """
from __future__ import absolute_import
import os
import sys
from setuptools import setup

CURRENT_DIR = os.path.dirname(__file__)
libinfo_py = os.path.join(CURRENT_DIR, 'ddls/libinfo.py')
libinfo = {'__file__': libinfo_py}
exec(compile(open(libinfo_py, "rb").read(), libinfo_py, 'exec'), libinfo, libinfo)

LIB_PATH = libinfo['find_lib_path']('ddls')
__version__ = libinfo['__version__']

setup(name='ddls',
      version=__version__,
      description=open(os.path.join(CURRENT_DIR, 'README.md')).read(),
      zip_safe=False,
      packages=['ddls', 'ddls/hpps', 'ddls/tracker', 'ddls/feeder', 'ddls/topi', 'ddls/utils'],
      data_files=[('ddls', [LIB_PATH]), ]
)
