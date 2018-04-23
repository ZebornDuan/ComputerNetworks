from tkinter import *
from PIL import Image, ImageTk
import tkinter.messagebox as tkbox
import threading
import client
import chat

class SignInWindow(Frame):
	def __init__(self, master=None):
		Frame.__init__(self, master)
		self.root = Toplevel()
		Label(self.root, text='Username:').place(x=10, y=10)
		Label(self.root, text='Password:').place(x=10, y=50)
		Label(self.root, text='Confirm:').place(x=10, y=90)
		self.username = StringVar()
		self.entry1 = Entry(self.root, textvariable=self.username)
		self.entry1.place(x=100, y=10)
		self.password = StringVar()
		self.entry2 = Entry(self.root, show='*', textvariable=self.password)
		self.entry2.place(x=100, y=50)
		self.confirm = StringVar()
		self.entry3 = Entry(self.root, show='*', textvariable=self.confirm)
		self.entry3.place(x=100, y=90)
		Button(self.root, text='Sign Up', command=self.signin).place(x=100, y=130)
		self.root.geometry('280x180')
		self.root.title('Sign In')

	def signin(self):
		pass


class LoginWindow(Frame):
	def __init__(self, master=None):
		Frame.__init__(self, master)
		self.canvas = Canvas(master, height=220, width=450)
		image = Image.open('/home/reborn/Desktop/net/welcome.jpg')
		self.image = ImageTk.PhotoImage(image)
		self.canvas.create_image(0, 0, anchor='nw', image=self.image)
		self.canvas.pack(side='top')
		Label(master, text='Username').place(x=30, y=250)
		Label(master, text='Password').place(x=30, y=280)
		self.username = StringVar()
		self.password = StringVar()
		self.entry1 = Entry(master, textvariable=self.username, width=38)
		self.entry2 = Entry(master, textvariable=self.password, show='*', width=38)
		self.entry1.place(x=100, y=250)
		self.entry2.place(x=100, y=280)
		self.button1 = Button(master, text='Login',command=self.login, width=10)
		self.button1.place(x=80, y=310)
		self.button2 = Button(master, text='Sign in',command=self.signin, width=10)
		self.button2.place(x=270, y=310)
		self.master.geometry('450x340')
		self.master.title('Login')
		self.client = client.Client()

	def login(self):
		user = self.username.get().strip()
		word = self.password.get().strip()
		if len(user) != 0 and len(word) != 0:
			data = 'login ' + user + ' ' + word
			self.client.send(data)
			response = self.client.receive_response().split(b'\n')
			if response[0] == b'ok':
				online = [item.decode('utf-8') for item in response[1:]]
				self.master.destroy()
				chat.ChatWindow(self.client, user, online).root.mainloop()
			else:
				tkbox.showerror(title='Error', message='invaild account or password')

	def signin(self):
		signin = SignInWindow()
		signin.mainloop()

if __name__ == '__main__':
	window = LoginWindow()
	window.master.geometry('450x340')
	window.master.title('Login')
	window.master.mainloop()
	