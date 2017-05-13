try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

config = {
        'description': 'minichess AI',
        'author': 'Michael Lane',
        'url': 'http://github.com/mikelane/',
        'download_url': 'http://github.com/mikelane/minichess',
        'author_email': 'mikelane@gmail.com',
        'version': '0.1',
        'install_requires': ['nose'],
        'packages': ['minichess'],
        'scripts': [],
        'name': 'minichess'
}

setup(**config)

