## Introduction

This is a very simple chatting application

## Functions

- sign in (sign up part has not been supported by server yet)
- real time synchronization of online information
- add friends (get user list from the server and add someone to friend list)
- chat with friends in pure text
- send a file to one friend

## Architecture

Server: Linux C++ / **select model**

Client: Python3.x / Tkinter library to support the GUI

All the offline information such as user accounts and their friend lists is saved by file

## Preview

![preview](https://github.com/ZebornDuan/ComputerNetworks/blob/master/NetworkProgramming/chatting/server/download/1.png)
![preview](https://github.com/ZebornDuan/ComputerNetworks/blob/master/NetworkProgramming/chatting/server/download/2.png)