__author__ = 'K. Adam Christensen (pope@shifteleven.com)'

import threading

from blink1 import _device


class Blink(object):

    def __init__(self):
        self._dev = _device.Blink()
        self._r, self._g, self._b = 0, 0, 0
        self._timer = None

    def set_rgb(self, r, g, b):
        self._cancel_timer()
        self._r, self._g, self._b = r, g, b
        self._dev.set_rgb(r, g, b)

    def fade_to_rgb(self, fade_ms, r, g, b):
        self._cancel_timer()
        self._r, self._g, self._b = r, g, b
        self._dev.fade_to_rgb(fade_ms, r, g, b)

    def get_rgb(self):
        return (self._r, self._g, self._b)

    def _cancel_timer(self):
        if self._timer:
            self._timer.cancel()
            self._timer = None

    def pulse(self, delay_ms, r, g, b):
        delay_s = delay_ms / 1000
        self.fade_to_rgb(delay_ms, r, g, b)
        def reset():
            self.fade_to_rgb(delay_ms, 0, 0, 0)
            self._timer = threading.Timer(delay_s,
                                          self.pulse,
                                          [delay_ms, r, g, b])
            self._timer.start()
        self._timer = threading.Timer(delay_s, reset)
        self._timer.start()
