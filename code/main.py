from tornado.ioloop import IOLoop, PeriodicCallback
import tornado.web
import tornado.websocket
from tornado.escape import json_encode, json_decode

app = tornado.web.Application([
    (r'/control', WebSocketControlHandler),
])

app.listen(8042)

IOLoop.instance().start()
