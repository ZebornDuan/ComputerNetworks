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
NO_ACCEPTABLE_METHODS = 0xff

IP_V4_ADDRESS = 0x01
DOMAIN_NAME = 0x03
IP_V6_ADDRESS = 0x04

BIG_PRIME = 334319
PRIMARY_ROOT = 13

SERVER_ADDRESS = ('127.0.0.1', 9000)


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


class SocksClient(StreamRequestHandler):

    def handle(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.connect(SERVER_ADDRESS)
        print('accept connection from %s:%s' % self.client_address)
        try:
            header = self.connection.recv(2)
            version, n_methods = struct.unpack('!BB', header)
            assert version == SOCKS_VERSION and n_methods > 0
            methods = self.connection.recv(n_methods)
            server_socket.sendall(struct.pack('!BB', version, n_methods + 1))
            server_socket.sendall(methods + struct.pack('!B', 0x80))
            parameters = server_socket.recv(10)
            version, big_prime, primary_root, y_a = struct.unpack('!BLBL', parameters)
            print(version, big_prime, primary_root, y_a)
            x_b = random.randint(2, big_prime)
            y_b = fast_exponentiation(primary_root, x_b, big_prime)
            key = '{0:016d}'.format(fast_exponentiation(y_a, x_b, big_prime))
            server_socket.sendall(struct.pack('!BL', version, y_b))
            self.connection.sendall(struct.pack("!BB", SOCKS_VERSION, NO_AUTHENTICATION))
            self.main_loop(self.connection, server_socket, key)
            return
        except AssertionError:
            self.server.close_request(self.request)

    @staticmethod
    def main_loop(client, remote, key):
        secure_socket = SecureSocket(remote, key)
        while True:
            r, _, _ = select.select([client, remote], [], [])
            if client in r:
                data = client.recv(4096)
                if secure_socket.send(data) <= 0:
                    break
            if remote in r:
                data = secure_socket.receive(4096)
                if client.send(data) <= 0:
                    break

if __name__ == '__main__':
    server = ThreadingTCPServer(('0.0.0.0', 8000), SocksClient)
    server.serve_forever()
