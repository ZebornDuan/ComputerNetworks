import socket
import sys

if __name__ == '__main__':
	task = sys.argv[1]
	filename = sys.argv[2]
	client = socket.socket()
	client.connect(('127.0.0.1', 8000))
	if task == 'get':
		client.send(bytes('g'+filename, encoding='utf-8'))
		reply = client.recv(2)
		if reply == b'no':
			print('no such file in the server directory')
			client.close()
		else:
			print('getting file ' + filename)
			client.send(bytes('ready', encoding='utf-8'))
			f = open(filename, 'wb')
			data = client.recv(4096)
			while True:
				if len(data) < 4096:
					f.write(data)
					break
				else:
					f.write(data)
			f.close()
			print('get file ' + filename + ' successfully')
	elif task == 'put':
		client.send(bytes('p'+filename, encoding='utf-8'))
		reply = client.recv(2)
		if reply == b'ok':
			print('putting file ' + filename)
			f = open(filename, 'rb')
			while True:
				data = f.read(4096)
				if not data:
					break
				client.sendall(data)
			f.close()
			print('put file ' + filename + ' successfully')