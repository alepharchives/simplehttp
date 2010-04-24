
import socket

from message import Request, Response
from unreader import SocketUnreader, IterUnreader

class Parser(object):
    def __init__(self, mesg_class, source):
        self.mesg_class = mesg_class
        if isinstance(source, socket.socket):
            self.unreader = SocketUnreader(source)
        else:
            self.unreader = IterUnreader(source)
    
    def __iter__(self):
        return self
    
    def next(self):
        ret = self.mesg_class(self.unreader)
        if not ret:
            raise StopIteration()
        return ret

class RequestParser(Parser):
    def __init__(self, *args, **kwargs):
        super(RequestParser, self).__init__(Request, *args, **kwargs)

class ResponseParser(Parser):
    def __init__(self, *args, **kwargs):
        super(ResponseParser, self).__init__(Response, *args, **kwargs)
