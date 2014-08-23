from distutils.core import setup, Extension

module1 = Extension('spam',
                    sources = ['spam.c'])

setup (name = 'test spam',
       version = '0.1',
       description = 'This is a test package',
       ext_modules = [module1])
