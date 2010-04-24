# Echo server program
import socket
import sys

from simplehttp import RequestParser
from gunicorn.http.request import Request

def simple(conn, info):
    p = RequestParser(conn)
    for r in p:
        r.body.read()
        conn.send("HTTP/1.0 200 OK\r\nContent-Length: 0\r\n\r\n")
        if r.should_close():
            break
    conn.close()

def gunicorn(conn, info):
    r = Request(conn, info, ('127.0.0.1', 8000), {"debug": False})
    r.read()
    conn.send("HTTP/1.0 200 OK\r\nContent-Length: 0\r\n\r\n")
    conn.close()

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('127.0.0.1', 8000))
    sock.listen(2048)

    while True:
        (conn, info) = sock.accept()
        simple(conn, info)
        #gunicorn(conn, info)

if __name__ == '__main__':
    main()
