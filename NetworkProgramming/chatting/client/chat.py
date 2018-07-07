from tkinter import *
import tkinter.messagebox as tkbox
import tkinter.filedialog as tkfile
import threading
import time

class ChatWindow():
	def __init__(self, client=None, username=None, online=None):
		self.root = Tk()
		self.client = client
		self.username = username
		self.frame1 = Frame(width=500, height=320, bg='white')
		self.frame2 = Frame(width=500, height=150, bg='white')
		self.frame3 = Frame(width=500, height=30)
		self.frame4 = Frame(width=200, height=500)
		self.frame1.grid(row=0, column=0, columnspan=2, padx=1, pady=3)
		self.frame2.grid(row=1, column=0, columnspan=2, padx=1, pady=3)
		self.frame3.grid(row=2, column=0, columnspan=2)
		self.frame4.grid(row=0, column=2, rowspan=3, padx=2, pady=3)
		self.textArea = Text(self.frame1, background='cyan', borderwidth=0, state='disabled')
		self.textArea.tag_config('a', foreground='#ff0000')
		self.textArea.tag_config('b', foreground='#0000ff')
		self.inputArea = Text(self.frame2, background='lightgreen', borderwidth=0)
		self.bar = Scrollbar(self.frame1)
		self.bar.config(command=self.textArea.yview)
		self.textArea.config(yscrollcommand=self.bar.set, height=25, width=70)
		self.bar.pack(side=RIGHT,fill=Y)

		self.button1 = Button(self.frame3, text='send', width=8, command=self.send)
		self.button2 = Button(self.frame3, text='cancel', width=8)
		self.button3 = Button(self.frame3, text='search', width=8, command=self.search)
		self.button4 = Button(self.frame3, text='add', width=8, command=self.add)
		self.button5 = Button(self.frame3, text='file', width=8, command=self.file)

		self.label1 = Button(self.frame4, text='Friend List', fg='black'
			,bg='#FF00FF', height=2, width=19, font='Arial', anchor='center')

		self.frame1.grid_propagate(0)
		self.frame2.grid_propagate(0)
		self.frame3.grid_propagate(0)
		self.frame4.grid_propagate(0)

		self.textArea.pack(side=LEFT,fill=BOTH,expand=1)
		self.inputArea.grid()
		self.button1.grid(row=2,column=0)
		self.button2.grid(row=2,column=1)
		self.button3.grid(row=2,column=2)
		self.button4.grid(row=2,column=3)
		self.button5.grid(row=2,column=4)
		self.label1.grid(row=0,column=0)
		self.friend_list = []
		self.button_list = []
		self.online_list = online
		self.finish = False
		self.getprevious = False
		self.object = None

		try:
			with open('/home/reborn/Desktop/net/list/' + self.username + '.txt', 'r') as f:
				for line in f.readlines():
					self.friend_list.append(line.strip())
		except:
			pass

		if self.client is not None:
			thread = threading.Thread(target=self.client.receive, args=(self,))
			thread.setDaemon(True)
			thread.start()

		for item in self.friend_list:
			button =  Button(self.frame4, text=item, fg='black' \
				,bg='#00FF00', height=2, width=19, font='Arial', anchor='w')
			if button['text'] not in self.online_list:
				button['fg'] = 'white'
			button.grid()
			button.bind('<Button-1>', self.click)
			self.button_list.append(button)

		self.client.send("get");

	def file(self):
		filename = tkfile.askopenfilename()
		print(filename)
		if self.object is None:
			tkbox.showwarning(title='Warning', message='Please select an object')
			return
		self.client.send('file ' + filename.split('/')[-1] + ' ' +self.object['text'])
		f = open(filename, 'rb')
		while True:
			data = f.read(1024)
			if not data:
				break
			self.client.socket.sendall(data)
		f.close()
		text = self.username + ' ' \
				+ time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())) +':\n' \
				+ 'send file ' + filename.split('/')[-1] + ' to the other side \n\n'
		self.textArea['state'] = 'normal'
		self.textArea.insert(END, text, 'b')
		self.textArea.see(END)
		self.textArea['state'] = 'disabled'

	def click(self, event):
		if self.object is not None:
			self.object['bg'] = '#00FF00'
		self.object = event.widget
		self.object['bg'] = 'orange'

	def add(self):
		data = self.inputArea.get('1.0',END)
		if data[:-1].strip() == '':
			print(data[:-1].strip())
			return
		elif self.client is not None:
			print(data[:-1].strip())
			data = 'add ' + data[:-1].strip()
			self.client.send(data)
			self.inputArea.delete('1.0',END)

	def send(self):
		data = self.inputArea.get('1.0',END)
		if data[:-1] == '':
			return
		elif self.object is None:
			tkbox.showwarning(title='Warning', message='Please select an object')
		elif self.client is not None:
			text = self.username + ' ' \
				+ time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())) +':\n' \
				+ data[:-1] + '\n\n'
			self.textArea['state'] = 'normal'
			self.textArea.insert(END, text, 'a')
			self.textArea.see(END)
			self.textArea['state'] = 'disabled'
			data = 'send ' + self.object['text'] + ' ' + data[:-1]
			self.client.send(data)
			self.inputArea.delete('1.0',END)

	def search(self):
		if self.client is not None:
			self.client.send('search ')

	def handle_message(self, message):
		data = message.split(b' ')
		if data[0] == b'search':
			# print("got search result")
			text = b'reply from server:user list\n'
			for i in range(1, len(data)):
				text += b'    ' + data[i] + b'\n'
			text += b'---------------------------------------\n'
			self.textArea['state'] = 'normal'
			self.textArea.insert(END, text.decode('utf-8'), 'a')
			self.textArea.see(END)
			self.textArea['state'] = 'disabled'
		elif data[0] == b'add':
			if data[1] == b'ok':
				self.friend_list.append(data[2].decode('utf-8'))
				with open('/home/reborn/Desktop/net/list/' + self.username + '.txt', 'w') as f:
					for item in self.friend_list:
						f.write(item)
						f.write('\n')
				button =  Button(self.frame4, text=data[2].decode('utf-8'), fg='black'
					,bg='#00FF00', height=2, width=19, font='Arial', anchor='w')
				button.bind('<Button-1>', self.click)
				if button['text'] not in self.online_list:
					button['fg'] = 'white'
				button.grid()
				self.button_list.append(button)
			else:
				tkbox.showerror(title='Error', message='User does not exist')
		elif data[0] == b'get':
			# print("get new message")
			text = data[1].decode('utf-8') + ' '
			if not self.finish:
				self.getprevious = True
				self.client.send("get")
			else:
				text += time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
			text += ':\n' 
			for item in data[2:]:
				text += item.decode('utf-8') + ' '
			text = text[:-1] + '\n\n'
			self.textArea['state'] = 'normal'
			self.textArea.insert(END, text, 'b')
			self.textArea.see(END)
			self.textArea['state'] = 'disabled'
		elif data[0] == b'file':
			self.client.pause = True
			self.client.send('ready')
			f = open('./receive/' + data[2].decode('utf-8'), 'wb')
			while True:
				filedata = self.client.receive_response()
				if len(filedata) < 1024:
					f.write(filedata)
					break
				f.write(filedata)
			f.close()
			self.client.pause = False
			text = data[1].decode('utf-8') + ' '
			if not self.finish:
				self.getprevious = True
				self.client.send("get")
			else:
				text += time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
			text+= ':\nreceive file ' + data[2].decode('utf-8') + ' from ' + data[1].decode('utf-8') + '\n\n'
			self.textArea['state'] = 'normal'
			self.textArea.insert(END, text, 'b')
			self.textArea.see(END)
			self.textArea['state'] = 'disabled'
		elif data[0] == b'on' or data[0] == b'off':
			if data[0] == b'on':
				self.online_list.append(data[1].decode('utf-8'))
			else:
				self.online_list.remove(data[1].decode('utf-8'))
			for button in self.button_list:
				if button['text'] not in self.online_list:
					button['fg'] = 'white'
				else:
					button['fg'] = 'black'
		elif data[0] == b'above':
			text = '---------------------------------------\n'
			text += '       above are offline messages       \n'
			self.textArea['state'] = 'normal'
			if self.getprevious:
				self.textArea.insert(END, text, 'a')
				self.textArea.see(END)
			self.textArea['state'] = 'disabled'
			self.finish = True

if __name__ == '__main__':
	window = ChatWindow()
	window.root.mainloop()