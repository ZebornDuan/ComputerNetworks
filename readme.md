# Network Programming

本仓库是一些简单的网络编程案例，尝试对基础的网络编程接口和模式的使用。所有的项目仅在同一主机下完成了服务器与客户端之间的正常通信，不保证能在复杂网络环境下的正常运行。

包含的项目如下：

- [chatting](#chatting)
- [file_transfer](#file_transfer)
- [udp_chat](#udp_chat)
- [QtGomoku](#QtGomoku)

## chatting

Linux系统环境下基于TCP的C++即时通信程序，服务器端采用select模型。客户端用python做了简单的图形界面。未进行协议原语的切分。

## file_transfer

Linux系统环境下基于TCP的C语言文件传输程序，服务器端采用epoll模型与线程池。客户端仅用作测试，与服务器进行短时连接。设置了so_linger和SO_RCVTIMEO套接字选项来防止FIN_WAIT_2攻击和僵尸连接。运行在Linux系统环境。

## udp_chat

Linux系统环境下基于UDP的C++命令行即时通讯程序。服务器与客户端之间采用“心跳”机制维持用户在线状态。以‘\n’为分界进行了简单的原语切分。

## QtGomoku

Windows环境下基于Qt套接字接口的网络五子棋游戏。服务器与客户端一体化。

## cryptology

一些密码学算法的实现。包括AES-256加解密算法、SHA3（keccak）哈希算法、求解最短线性移位寄存器（线性综合解）的BM算法。