try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

config = {
    'description': 'Thermostat LAN Web Site',
    'author': 'Garrett Burd',
    'url': 'URL to get it at.',
    'download_url': 'Where to download it.',
    'author_email': 'gjb49@case.edu',
    'version': '0.1',
    'install_requires': ['nose'],
    'packages': ['WEBSITE'],
    'scripts': [],
    'name': 'Website'
}

setup(**config)
