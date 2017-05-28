
import socket   #socket模块
import codecs
import msgpack, io

HOST = '127.0.0.1'
PORT = 5000

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind((HOST,PORT)); s.listen(5)

class TcpStream(io.BufferedIOBase):
    def __init__(self, sock):
        self._sock = sock
    def read(self, size):
        return self._sock.recv(size)
    def write(self, b):
        return self._sock.send(b)
    def flush(self):
        return self._sock.flush()
    def close(self):
        return self._sock.close()

num = 0
while 1:
    conn, addr = s.accept()   #接受TCP连接，并返回新的套接字与IP地址
    print('')
    print('## Connected by', addr)
    num += 1
    try:
        data = {'num': num,
                'int':[-1, -12345678900, 12345678900],
                'float': [0.1, 0.00000000001, 1234567890.333],
                'bool': [True, False], 'nil': None}
        print('Data before sent:', data)
        tcp = TcpStream(conn)
        # conn.send(msgpack.packb(data))
        msgpack.pack(data, tcp)
        # data = conn.recv(1024)
        # if not data: break
        # print('Data received(bytes):', data)
        data = msgpack.unpack(tcp)
        print('Data received(object):', data)
    except ConnectionResetError:
        conn.close()     #关闭连接
    except msgpack.UnpackValueError:
        print('Unpack data failure')
        conn.close()     #关闭连接
    except msgpack.OutOfData:
        print('Unpack data failure')
        conn.close()     #关闭连接
