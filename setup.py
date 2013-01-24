from distutils.core import setup, Extension

device = Extension('blink1._device',
                   include_dirs = ['/opt/local/include'],
                   library_dirs = ['/opt/local/lib'],
                   libraries = ['usb'],
                   sources = ['device.c', 'hiddata.c'])

setup(name = 'blink1',
      version = '1.0',
      description = 'Python interface to Blink(1) USB device',
      author = 'K. Adam Christensen',
      author_email = 'pope@shifteleven.com',
      packages = ['blink1'],
      package_dir = { 'blink1': '.' },
      ext_modules = [device])
