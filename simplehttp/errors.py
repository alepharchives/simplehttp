
class ParseException(Exception):
    pass

class NoMoreDataError(ParseException):
    def __init__(self, buf):
        self.buf = buf
    def __str__(self):
        return "No more data after: %r" % self.buf

class InvalidRequestLine(ParseException):
    def __init__(self, req):
        self.req = req
        self.code = 400

    def __str__(self):
        return "Invalid HTTP request line: %r" % self.req

class InvalidHeaderError(ParseException):
    def __init__(self, hdr):
        self.hdr = hdr
    
    def __str__(self):
        return "Invalid HTTP Header: %r" % self.hdr

class InvalidChunkSizeError(ParseException):
    def __init__(self, data):
        self.data = data
    
    def __str__(self):
        return "Invalid chunk size: %r" % self.data

class ChunkMissingTerminatorError(ParseException):
    def __init__(self, term):
        self.term = term
    
    def __str__(self):
        return "Invalid chunk terminator is not '\\r\\n': %r" % self.term