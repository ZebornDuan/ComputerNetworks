import socket

class Client(object):
	def __init__(self):
		self.ip_address = ('127.0.0.1', 9000)
		self.socket = socket.socket()
		self.socket.connect(self.ip_address)
		self.pause = False

	def receive(self, master):
		while True:
			if not self.pause:
				data = self.socket.recv(1024)
				print(data)
				master.handle_message(data)

	def send(self, data):
		self.socket.send(bytes(data, encoding='utf-8'))

	def receive_response(self):
		data = self.socket.recv(1024)
		return data