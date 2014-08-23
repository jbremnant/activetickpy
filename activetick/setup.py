from distutils.core import setup, Extension

atmodule = Extension('_activetick',
            sources = ['activetick.cpp', '../src/Helper.cpp', '../src/Session.cpp', '../src/Requestor.cpp'],
            include_dirs = ['../include', '../include/ActiveTickServerAPI', '../src'],
            libraries = ['ActiveTickServerAPI'],
            library_dirs = ['.', '../lib'],
           )

setup (name = 'ActiveTick Py',
       version = '0.1',
       description = 'Python interface to ActiveTick C++ library',
       ext_modules = [atmodule])
