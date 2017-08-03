ball = LightBall();

class WebSocketControlHandler(tornado.websocket.WebSocketHandler):
    ...

    def update(self):
        json_status = json_encode(self.get_status())
        self.write_message(json_status)

    @staticmethod
    def update_clients():
        for client in clients:
            client.update()

    @staticmethod
    def process_message(message):
        try:
            ball.set_colour(message["colour"])
        except (KeyError, TypeError):
            logger = logging.getLogger("WebSocketControlHandler")
            logger.warn("Useless message {}".format(message))

    @staticmethod
    def get_status():
        return {"colour" : ball.get_colour()}
