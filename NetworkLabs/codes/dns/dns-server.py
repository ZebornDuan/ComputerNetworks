import dpkt
import random
import socket
import contextlib
import gevent.server


class DNSServer(gevent.server.DatagramServer):
    def __init__(self, address, handler):
        super(DNSServer, self).__init__(address)
        self.handler = handler

    def serve_forever(self, stop_timeout=None):
        super(DNSServer, self).serve_forever(stop_timeout)

    def handle(self, request, address):
        self.handler(self.sendto, request, address)


class DnsResolver(object):
    def __init__(self):
        self.public_resolver = {
            'udp': [
                ('182.254.116.116', 53),
                ('114.114.114.114', 53),
                ('223.5.5.5', 53)
            ]
        }

    def __call__(self, sendto, raw_request, address):
        response = self.resolve(raw_request)
        sendto(str(response), address)

    def resolve(self, raw_request):
        print('get request')
        request = dpkt.dns.DNS(raw_request)
        print(request.id)
        dns_server = random.choice(self.public_resolver['udp'])
        udp_socket = self.get_udp_socket()
        with contextlib.closing(udp_socket):
            udp_socket.settimeout(10)
            udp_socket.sendto(raw_request, dns_server)
            response = dpkt.dns.DNS(udp_socket.recv(2048))
            return response

    @staticmethod
    def get_udp_socket():
        udp_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        udp_socket.bind(('', 8000))
        return udp_socket


if __name__ == '__main__':
    DNSServer('*:53', DnsResolver()).serve_forever()