## 问题描述

在清华大学校园网无线网络环境下，A用校园网账号认证登录，B在A的附近连接了同一个WiFi路由器，但没有用校园网账号认证登录。如果B修改自己的IP和Mac地址假冒A的身份，可以做什么？

备注：清华大学无线校园网是一种分段计费网络，每位师生需要用自己的学号或工号认证后才能上网。对于一般的局域网，实验中的现象除了计费的影响之外也基本相同。

## 实验过程
实验环境配置：
- 一台Mac OS 10.13主机
- 一台Win7系统主机
- Win7系统主机接入清华大学无线校园网Tsinghua并登录认证，IP地址为183.172.160.204/21
- Mac OS系统主机接入同一无线网但未进行登录认证
![029](https://github.com/DBullet/UCore/blob/master/homework/image029.png)

#### 实验步骤
 
将Mac OS的IP地址和Mac地址配置为已经登录的Win7主机的IP地址和Mac地址，然后重新启动网卡。

![037](https://github.com/DBullet/UCore/blob/master/homework/image037.png)
![039](https://github.com/DBullet/UCore/blob/master/homework/image039.png)

接着用Mac OS访问外网，发现最初不能访问，之后又可以访问，然后间歇性会出现丢包现象。可见，相同IP地址和Mac地址的Mac OS和Win7系统两台主机处在一种竞争的状态，网络交换机通过向后学习算法会实时更新mac地址对应的端口，即哪台主机发送数据，接下来发送给该IP地址和Mac地址的数据就会被发送给哪台主机。

![041](https://github.com/DBullet/UCore/blob/master/homework/image041.png)
 
通过下面的实验可以直观的表现Win7系统在IP地址和Mac地址被冒用后出现的数据缺失现象。
通过在Win7系统上同时登录网页版微信，并用移动端发送消息，正常情况下消息会被同步到web端，在IP被冒用之后，可以看到消息同步失败，即同步的数据被转发到了Mac OS主机，Win7主机出现了数据缺失。这条消息最终还是会被成功同步，原因可能是同步消息存在确认机制，Mac OS不会进行确认，而当Win7竞争到交换机的端口映射时重传的消息被收到。但是仍然可以看到消息同步的延迟比以往大了很多。

![043](https://github.com/DBullet/UCore/blob/master/homework/image043.png)
![045](https://github.com/DBullet/UCore/blob/master/homework/image045.jpg)

因此，在IP和Mac地址被冒用的情况下，假冒者可以通过冒用的地址访问互联网，但是当被冒用者也处于活跃状态的话，两者会进行竞争，使得两者都出现数据缺失，网络服务质量均不佳。而在计费服务的网络，冒用者则可以使自身的网络流量计费被记录在被冒用者进行登录认证的账号上。

#### 实验总结
- 用到的Linux网路命令
	- sudo ifconfig [interface] ether [mac-address]:更改本机网卡的Mac地址
	- sudo ifconfig [interface] up/down:关闭/开启本机网卡
- 实验原理
	- 网桥(交换机)的向后学习算法 