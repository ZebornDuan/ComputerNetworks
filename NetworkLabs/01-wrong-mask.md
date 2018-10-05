## 问题描述
一个局域网内部的两台计算机的A、B的子网应该是192.168.26.0/24，网关192.168.26.2，如下图：
![topo](https://github.com/DBullet/learn-git/blob/test/image001.png) 
其中B的子网掩码本应该是255.255.255.0，现在错误地配置成了255.255.255.224，请问A和B之间是否能正常通信。

## 分析与结论
在其他配置均正确的情况下，A和B之间仍然能够正常通信。分两种情况进行分析。
1. A向B发送数据，B接收来自A的数据
B的子网掩码错误并不影响A向B发送数据以及B接收来自A的数据，A向B发送数据时，首先判断B的IP地址与自身在同一个子网内，则在获取B的Mac地址后会直接将数据发送给B，而B也正常接收。
2. B向A发送数据，A接收来自B的数据
当B向A发送数据是，B通过自身的子网掩码判断目的地A的IP地址与自身是否在同一个子网内，由于B的子网掩码配置发生错误，B认为A与自身不在同一个子网内，因此，B会将发送给A的数据发送给网关，由网关进行路由选择，而网关会正确地把发给A的数据转发给A，因此A仍然能收到来自B的数据，不同的是，B误以为A与自身不在同一个子网，而将数据发送给网关，经由网关转发给A，而正常情况下，B会直接将数据发送给同一个子网内的A。

综合以上分析可以得出结论：A和B之间仍然能够正常通信。
## 实验验证
- 实验环境配置为：
在Win7系统下开启VMWare下的两台虚拟机，一台系统为Ubuntu 17.04，另一台系统为Kali 2017.02。网络配置均为NAT，即两台虚拟机以物理机为网关组成了一个局域网。
- 初始正常情况下，两台主机的IP地址和网关配置如下：
	- Kali： IP 192.168.30.131  gateway：192.168.30.2  Mask：255.255.255.0
	- Ubuntu： IP 192.168.30.128  gateway：192.168.30.2  Mask：255.255.255.0
![03](https://github.com/DBullet/learn-git/blob/test/image003.png)
![05](https://github.com/DBullet/learn-git/blob/test/image005.png)
#### 实验操作
现在将Ubuntu系统主机的IP地址更改为192.168.30.3，子网掩码不变。更改后的配置如下：

![07](https://github.com/DBullet/learn-git/blob/test/image007.png) 

此时，Kali和Ubuntu能够正常通信。

![09](https://github.com/DBullet/learn-git/blob/test/image009.png)
![11](https://github.com/DBullet/learn-git/blob/test/image011.png) 

进一步，将Ubuntu的子网掩码改为255.255.255.224。更改后的配置如下：

![13](https://github.com/DBullet/learn-git/blob/test/image013.png)

此时发现，Ubuntu与Kali不能正常通信，Kali能够正常访问外网，而Ubuntu不能访问外网，但能够与同一个子网下的网关正常通信。

![15](https://github.com/DBullet/learn-git/blob/test/image015.png)
![17](https://github.com/DBullet/learn-git/blob/test/image017.png) 

从Ubuntu不能访问外网而能和同一个子网内的网关正常通信，说明Ubuntu的网关配置尚且不完善，确实，在更改了Ubuntu的掩码后，还未给Ubuntu配置网关，此时Ubuntu不能与Kali通信，也说明了Ubuntu并没有把Kali作为与自身在同一个子网下的主机。这与之前的分析是相符的。通过进一步的确认也能够验证Ubuntu网关未正确配置的推断的正确性。

![19](https://github.com/DBullet/learn-git/blob/test/image019.png)

将Ubuntu的网关配置正确。
![21](https://github.com/DBullet/learn-git/blob/test/image021.png)

配置正确后能够看到Ubuntu和Kali又能够正常通信了，且Ubuntu也能正常访问外网了。

![23](https://github.com/DBullet/learn-git/blob/test/image023.png)
![25](https://github.com/DBullet/learn-git/blob/test/image025.png)

由网关配置正确后，Kali和Ubuntu才能正常通信可以验证之前的分析，即由Ubuntu发向Kali的数据需要经过网关的转发。

#### 实验总结
- 用到的Linux下的网络操作命令
	- ifconfig：查看本机网络配置
	- sudo ifconfig [interface] [ip] netmask [netmask]:更改本机IP地址和子网掩码
	- ip route show:查看本机IP地址与网关
	- sudo route add default gw [ip]:配置本机的默认网关
- ICMP协议
	- ICMP协议（Internet Control Message Protocol）是Internet控制消息协议，用于在IP主机、路由器之间传递控制消息。这些控制消息虽然不是用户数据，但是，对于保证用户数据的正确传输有着重要的作用
	- 从功能上划分，ICMP报文可分为2大类：ICMP差错报文和ICMP查询报文
	- 差错报告报文包括：目的不可达、源主机消亡、超时、参数问题、重定向
	- 查询报文包括：回应请求和应答（ping）、时间戳和时间戳应答、地址掩码请求和应答、路由器通告和请求
- ICMP协议的典型应用
	- 网络诊断命令ping
		- ping命令通过向目标主机发送ICMP回应请求报文来测试网络的连通性
		- ping命令的常见响应
			- 正确收到响应报文，显示报文的源地址、序号、ttl和时间，说明本机与目标主机之间能够正常通信，网络连接正常
			- Destination Net Unreachable，目标主机不可达，即报文所经过的路由器的路由表中不存在目标IP，可能是因为目标主机不存在或无法正常通信
			- Request Timed Out，请求超时，说明到达目标主机的路由存在，但是未在一定时间内收到对方的响应，可能是因为对方主机拒收数据，也可能是因为响应报文在回传给本地主机的路径上出现了问题
	- 路由跟踪命令traceroute(Windows系统下的tracert命令)
		- traceroute命令可以选择使用ICMP协议和UDP协议
		- traceroute是用来检测发出数据包的主机到目标主机之间所经过的网关数量的工具。traceroute的原理是试图以最小的TTL（存活时间）发出探测包来跟踪数据包到达目标主机所经过的网关，然后监听一个来自网关ICMP的应答。发送数据包的大小默认为38个字节
		- traceroute程序完整过程：首先它发送一份TTL字段为1的IP数据包给目的主机，处理这个数据包的第一个路由器将TTL值减1，然后丢弃该数据报，并给源主机发送一个ICMP报文（“超时”信息，这个报文包含了路由器的IP地址，这样就得到了第一个路由器的地址），然后traceroute发送一个TTL为2的数据报来得到第二个路由器的IP地址，继续这个过程，直至这个数据报到达目的主机
		- 如果路径上某台路由器禁用了ICMP Echo Reply，以隐藏自身的信息，那么traceroute命令仅能通过ICMP TTL-expired报文知道数据经过了多少跳到达目的地，而不能知道每一跳的详细信息，这时会用‘*’来显示
		- 本次实验中不通过traceroute命令来判断是否经过了网关，因为traceroute命令是以路径上的路由器为单位，同一个局域网内的报文即使通过了网关，但没有出网关，依然视为仅通过一跳到达

