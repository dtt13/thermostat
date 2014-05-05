try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

config = {
    'description': 'Thermostat LAN Web Site',
    'author': 'Roost Project Group',
    'url': 'roosthermostat.com,
    'download_url': 'See roosthermostat.com',
    'author_email': 'roost@case.edu',
    'version': '0.1',
    'install_requires': ['nose'],
    'packages': ['WEBSITE'],
    'scripts': [],
    'name': 'Roost Website'
}

setup(**config)
