from distutils.core import setup, Extension

blink1 = Extension('blink1',
                   include_dirs = ['/opt/local/include'],
                   library_dirs = ['/opt/local/lib'],
                   libraries = ['usb'],
                   sources = ['pyblink1.c', 'hiddata.c'])

setup (name = 'pyblink1',
       version = '1.0',
       description = 'Python interface to Blink(1) USB device',
       author = 'K. Adam Christensen',
       author_email = 'pope@shifteleven.com',
       ext_modules = [blink1])
