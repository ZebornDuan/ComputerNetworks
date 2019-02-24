from SocketServer import ThreadingTCPServer, StreamRequestHandler
from Crypto.Cipher import AES
import struct
import socket
import select
import random

# SOCKS5 https://www.ietf.org/rfc/rfc1928.txt

SOCKS_VERSION = 5

NO_AUTHENTICATION = 0x00
GSS_API = 0x01
USERNAME_PASSWORD = 0x02
PRIVATE_AUTHENTICATION = 0x80
NO_ACCEPTABLE_METHODS = 0xff

IP_V4_ADDRESS = 0x01
DOMAIN_NAME = 0x03
IP_V6_ADDRESS = 0x04

BIG_PRIME = 334319
PRIMARY_ROOT = 13


class SecureSocket(object):
    def __init__(self, socket_, key):
        self.socket = socket_
        self.aes = AES.new(key)

    def receive(self, length):
        length = 16 * (length / 16 + 1) if length % 16 != 0 else length
        data = self.aes.decrypt(self.socket.recv(length))
        data = data.rstrip('\0')
        return data

    def send(self, data):
        additional_length = 0 if len(data) % 16 == 0 else 16 - len(data) % 16
        data = data + ('\0' * additional_length)
        return self.socket.send(self.aes.encrypt(data))


def fast_exponentiation(b, e, m):
    result = 1
    while e != 0:
        if (e & 1) == 1:
            result = (result * b) % m
        e >>= 1
        b = (b * b) % m
    return result


class SocksProxy(StreamRequestHandler):
    user_name = 'username'
    pass_word = 'password'

    def handle(self):
        print('accept connection from %s:%s' % self.client_address)
        try:
            header = self.connection.recv(2)
            version, n_methods = struct.unpack('!BB', header)
            assert version == SOCKS_VERSION and n_methods > 0
            key = self.authenticate(n_methods)
            if key is not None:
                secure_socket = SecureSocket(self.connection, key)
                data = secure_socket.receive(4096)
                print(len(data))
                version, command, _, address_type = struct.unpack('!BBBB', data[0:4])
                assert version == SOCKS_VERSION
                if address_type == IP_V4_ADDRESS:
                    address = socket.inet_ntoa(data[4:8])
                elif address_type == DOMAIN_NAME:
                    domain_length = ord(self.connection.recv(1)[0])
                    address = secure_socket.receive(domain_length)
                else:
                    assert False
                port = struct.unpack('!H', data[8:])[0]
                # noinspection PyBroadException
                try:
                    if command == 1:  # CONNECT
                        remote = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        remote.connect((address, port))
                        print((address, port))
                        bind_address = remote.getsockname()
                        address = struct.unpack("!I", socket.inet_aton(bind_address[0]))[0]
                        port = bind_address[1]
                        reply = struct.pack("!BBBBIH", SOCKS_VERSION, 0, 0, address_type, address, port)
                        secure_socket.send(reply)
                        self.secure_loop(secure_socket, remote)
                    else:
                        assert False
                except Exception:
                    reply = struct.pack("!BBBBIH", SOCKS_VERSION, 5, 0, address_type, 0, 0)
                    secure_socket.send(reply)
                return
            version, command, _, address_type = struct.unpack('!BBBB', self.connection.recv(4))
            assert version == SOCKS_VERSION
            if address_type == IP_V4_ADDRESS:
                address = socket.inet_ntoa(self.connection.recv(4))
            elif address_type == DOMAIN_NAME:
                domain_length = ord(self.connection.recv(1)[0])
                address = self.connection.recv(domain_length)
            else:
                assert False
            port = struct.unpack('!H', self.connection.recv(2))[0]
            # noinspection PyBroadException
            try:
                if command == 1:  # CONNECT
                    remote = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    remote.connect((address, port))
                    print((address, port))
                    bind_address = remote.getsockname()
                    address = struct.unpack("!I", socket.inet_aton(bind_address[0]))[0]
                    port = bind_address[1]
                    reply = struct.pack("!BBBBIH", SOCKS_VERSION, 0, 0, address_type, address, port)
                    self.connection.sendall(reply)
                    self.main_loop(self.connection, remote)
                else:
                    assert False
            except Exception:
                reply = struct.pack("!BBBBIH", SOCKS_VERSION, 5, 0, address_type, 0, 0)
                self.connection.sendall(reply)
        except AssertionError:
            self.server.close_request(self.request)

    def authenticate(self, n_methods):
        methods = [ord(self.connection.recv(1)) for _ in range(n_methods)]

        if PRIVATE_AUTHENTICATION in methods:
            x_a = random.randint(2, BIG_PRIME)
            y_a = fast_exponentiation(PRIMARY_ROOT, x_a, BIG_PRIME)
            self.connection.sendall(struct.pack('!BLBL', SOCKS_VERSION, BIG_PRIME, PRIMARY_ROOT, y_a))
            _, y_b = struct.unpack('!BL', self.connection.recv(5))
            key = '{0:016d}'.format(fast_exponentiation(y_b, x_a, BIG_PRIME))
            print(key)
            return key
        elif USERNAME_PASSWORD in methods:
            # RFC1929 https://www.ietf.org/rfc/rfc1928.txt
            self.connection.sendall(struct.pack("!BB", SOCKS_VERSION, USERNAME_PASSWORD))
            version = ord(self.connection.recv(1))
            if version != 1:
                pass
            username_length = ord(self.connection.recv(1))
            username = self.connection.recv(username_length).decode('utf-8')

            password_length = ord(self.connection.recv(1))
            password = self.connection.recv(password_length).decode('utf-8')
            print(username, password)
            if username == self.user_name and password == self.pass_word:
                self.connection.sendall(struct.pack("!BB", version, 0))
            else:
                self.connection.sendall(struct.pack('!BB', version, 1))
                return
        elif NO_AUTHENTICATION in methods:
            self.connection.sendall(struct.pack("!BB", SOCKS_VERSION, NO_AUTHENTICATION))
        else:
            self.connection.sendall(struct.pack("!BB", SOCKS_VERSION, NO_ACCEPTABLE_METHODS))

    @staticmethod
    def main_loop(client, remote):
        while True:
            r, _, _ = select.select([client, remote], [], [])
            if client in r:
                data = client.recv(4096)
                if remote.send(data) <= 0:
                    break
            if remote in r:
                data = remote.recv(4096)
                if client.send(data) <= 0:
                    break

    @staticmethod
    def secure_loop(remote, client):
        while True:
            r, _, _ = select.select([client, remote.socket], [], [])
            if client in r:
                data = client.recv(4096)
                if remote.send(data) <= 0:
                    break
            if remote.socket in r:
                data = remote.receive(4096)
                if client.send(data) <= 0:
                    break


if __name__ == '__main__':
    server = ThreadingTCPServer(('0.0.0.0', 9000), SocksProxy)
    server.serve_forever()

