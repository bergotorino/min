from distutils.core import setup, Extension

module1 = Extension('sut_extxxx',
                    sources = ['sut_extxxx.c'],
                    libraries = ['minutils','mintmapi','python2.5'],
                    )

setup (name = 'sut_extxxx',
        version = '1.0',
        description = 'This is a demo package',
        ext_modules = [module1])
