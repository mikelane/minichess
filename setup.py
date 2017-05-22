from setuptools import setup, find_packages
from os import path

here = path.abspath(path.dirname(__file__))

setup(
    name='minichess',
    version='1.3.2',
    packages=find_packages(exclude=['contrib', 'docs', 'tests*']),
    url='https://github.com/mikelane/minichess',
    license='MIT',
    author='Michael Lane',
    author_email='mikelane@gmail.com',
    description='An AI for minitchess',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Students',
        'Topic :: Education :: Artificial Intelligence',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Pypy2 :: 5.7'
    ],
    keywords='combinatorial games ai artificial intelligence education',
    install_requires=[
        'cython',
        'numpy',
        'pyzmq',
        'future',
        'six',
        'configparser',
        'nose'
    ],
    setup_requires=[
        'cython',
        'numpy',
        'pyzmq',
        'future',
        'six',
        'configparser',
        'nose'
    ]
)