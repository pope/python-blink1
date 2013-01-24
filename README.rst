=============
python-blink1
=============

python-blink1 is a python extension to the `blink(1)`_ USB device.

Requirements
------------

First and foremost, you need a `blink(1) device`_. 

Next, you need to have ``libusb`` installed.

For MacPorts::

  $ sudo port install libusb-compat

Usage
-----

Simple::

  import blink1

  b = blink1.Blink()
  b.set_rgb(255, 0, 0)

.. _blink(1): https://github.com/todbot/blink1
.. _blink(1) device: http://thingm.com/products/blink-1.html
