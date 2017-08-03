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

    def update(self):
        json_status = json_encode(self.get_status())
        self.write_message(json_status)

    def process_message(self, message):
        try:
            ball.set_colour(message["colour"])
        except (KeyError, TypeError):
            self.logger.warn("Useless message {}".format(message))

    def update_clients(self):
        for client in clients:
            client.update()

    def get_status(self):
        return {"colour" : ball.get_colour()}
