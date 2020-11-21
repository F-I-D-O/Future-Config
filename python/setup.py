import setuptools
from setuptools import setup

# with open("README.rst", "r") as fh:
#     long_description = fh.read()

setup(
	name='fconfig',
	version='1.1.0',
	description='Python config loader for Future Config format.',
	author='F.I.D.O.',
	author_email='david.fido.fiedler@gmail.com',
	license='MIT',
	packages=setuptools.find_packages(),
	url = 'https://github.com/F-I-D-O/Future-Config/',
	install_requires=['Mako', 'setuptools', 'typing', 'pytest'],
	python_requires='>=3',
	package_data={'fconfig.templates': ['*.txt']}
)
