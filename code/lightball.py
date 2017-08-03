class LightBall(object):

    colours = {
        "#FF0033" : "R1",
        "#FF6600" : "R2",
        "#FF9933" : "R3",
        "#FF9966" : "R4",
        "#FFFF99" : "R5",
        "#00CC00" : "G1",
        "#33CCCC" : "G2",
        "#00CCFF" : "G3",
        "#0066FF" : "G4",
        "#3366FF" : "G5",
        "#3333FF" : "B1",
        "#6666FF" : "B2",
        "#9966FF" : "B3",
        "#FF00FF" : "B4",
        "#9900FF" : "B5",
        "#FFFFFF" : "W",
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
