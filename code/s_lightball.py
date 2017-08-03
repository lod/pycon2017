class LightBall(object):

    colours = {
        "#FFFFFF" : "W",
        ...
    }

    def __init__(self, colour="#FFFFFF"):
        self.colour = colour # #rgb string
        self._send_button("ON")
        self.set_colour(colour)

    def __del__(self):
        self._send_button("OFF")

    def get_colour(self):
        return self.colour

    def set_colour(self, colour):
        try:
            button = self.colours[colour]
            self._send_button(button)
            self.colour = colour
        except KeyError:
            logging.getLogger("LightBall").warn("Bad colour {}".format(colour))

    def _send_button(self, button):
        logging.getLogger("LightBall").info("irsend {}".format(button))
        subprocess.call(["/usr/bin/irsend", "SEND_ONCE", "ball", button])
