from tornado.ioloop import IOLoop, PeriodicCallback
import tornado.web
import tornado.websocket
from tornado.escape import json_encode, json_decode

import subprocess

import logging
logging.basicConfig(level=logging.DEBUG)

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


# WebSocketControlHandler object is created for each connection
# Need some globals to be shared across connections
ball = LightBall();
clients = [] 

class WebSocketControlHandler(tornado.websocket.WebSocketHandler):
    def __init__(self, application, request, **kwargs):
        super(WebSocketControlHandler, self).__init__(application, request, **kwargs)
        self.logger = logging.getLogger("WebSocketControlHandler")
        self.logger.info("init");
                
    def check_origin(self, origin):
        return True # Allow any origin

    def open(self, *args):
        self.logger.info("open")
        clients.append(self)

    def on_close(self):
        self.logger.info("close")
        clients.remove(self)

    def on_message(self, raw_message):
        self.logger.debug(raw_message)

        try:
            message = json_decode(raw_message)
        except ValueError:
            self.logger.warn("Non-JSON message {}".format(raw_message))
            return

        self.process_message(message)
        self.update_clients()

    def process_message(self, message):
        try:
            ball.set_colour(message["colour"])
        except (KeyError, TypeError):
            self.logger.warn("Useless message {}".format(message))

    def update_clients(self):
        json_status = json_encode(self.get_status())

        for client in clients:
            client.write_message(json_status)

    def get_status(self):
        return {"colour" : ball.get_colour()}

app = tornado.web.Application([
    (r'/control', WebSocketControlHandler),
])

app.listen(8042)


#PeriodicCallback(update_clients, 1000).start()

IOLoop.instance().start()
