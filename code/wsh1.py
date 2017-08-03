clients = [] 

class WebSocketControlHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True # Allow any origin

    def open(self, *args):
        clients.append(self)
        self.update()

    def on_close(self):
        clients.remove(self)

    def on_message(self, raw_message):
        self.logger.debug(raw_message)

        try:
            message = json_decode(raw_message)
        except ValueError:
            logger = logging.getLogger("WebSocketControlHandler")
            logger.warn("Non-JSON message {}".format(raw_message))
            return

        self.process_message(message)
        self.update_clients()

    ...
