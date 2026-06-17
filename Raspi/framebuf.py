# MICROPYTHON FRAMEBUFFER LIBRARY
# (Versión oficial mínima compatible con SSD1306)

class FrameBuffer:
    def __init__(self, buffer, width, height, format, stride=0):
        if format != MONO_VLSB:
            raise ValueError("Only MONO_VLSB supported")
        self.buffer = buffer
        self.width = width
        self.height = height
        self.format = format

    def fill(self, c):
        b = 0xFF if c else 0x00
        for i in range(len(self.buffer)):
            self.buffer[i] = b

    def pixel(self, x, y, c=None):
        if c is None:
            return (self.buffer[x + (y >> 3) * self.width] >> (y & 7)) & 1
        if c:
            self.buffer[x + (y >> 3) * self.width] |= (1 << (y & 7))
        else:
            self.buffer[x + (y >> 3) * self.width] &= ~(1 << (y & 7))

    def text(self, string, x, y, c=1):
        for char in string:
            self.char(char, x, y, c)
            x += 8

    def char(self, char, x, y, c=1):
        from micropython import const
        FONT = (
