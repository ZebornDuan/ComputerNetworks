# DNS测量与分析

## DNS服务测量

### 问题描述
1. 通过DNS查询获取所有edu.cn域名的权威服务器名称和地址。由于 edu.cn 的区域文件不公开，本实验中直接对提供的[edu.cn域名列表](https://github.com/ZebornDuan/ComputerNetworks/tree/master/NetworkLabs/codes/dns/edu_domain.txt)文件内的域名进行扫描。
2. 在 CERNET 地址范围内(可参考 [AS4538 的地址范围](https://bgp.he.net/AS4538#_prefixes))，通过53端口探测、发送DNS解析请求的方式，发现开放的DNS解析服务器。
3. 对一些DNS安全性问题进行测试。

### 实验环境
Mac OS主机一台与Windows主机一台(主机的系统并不影响实验)，配置有Python + Scapy环境

### 实验过程
#### DNS服务器发现
该实验主要通过构造特定的DNS查询报文来获取响应，并从响应中提取出所需要的信息。
首先通过scapy来观察DNS报文的格式。
对于其中各个字段的解析可以参考这篇[DNS报文格式分析](https://blog.csdn.net/tianxuhong/article/details/74922454)。实验中DNS报头直接采用默认值即可，需要填充的是类型为DNSQRField的qd字段。DNSQRField由三个字段构成，查询名、查询类型和查询类。查询名即为要查询的域名，常用的查询类型有A(由域名获得IPv4地址)、AAAA(查询域名服务器)、NS(查询域名服务器)，而查询类通常为1，表示是Internet数据。实验要获取权威服务器的名称和地址，于是选定查询类型为NS(其实将类型置为A，在响应报文中也会携带域名服务器的信息)。

![DNS packet](https://github.com/DBullet/UCore/blob/master/lab4/Lark20181021-150657.png)

![DNSQR](https://github.com/DBullet/UCore/blob/master/lab4/Lark20181021-193115.png)

观察响应报文的格式，可以发现字段类型为DNSRRField的字段有an(应答资源记录)、ns(授权资源记录)、ar(附加资源记录)三个，an字段是指定的查询类型的查询结果，如果查询类型为A，那么则会把域名及对应的IP地址放在这个字段。ns字段里包含了查询所对应的权威服务器的名称，ar字段则包含了这些权威服务器的详细地址信息。如果查询类型类型为NS，可以看到an字段与查询相同名称但类型为A的查询响应的ns字段相同，而ns字段则为空。

![DNSRR](https://github.com/DBullet/UCore/blob/master/lab4/Lark20181021-193119.png)

因此只需要从响应报文的ar字段中提取出权威服务器的地址信息即可。同样的，可以通过在一定地址范围内向指定IP地址的53端口发送DNS查询请求来发现哪些IP地址的主机上运行了DNS服务，从而来发现地址范围内的解析服务器。例如在166.111.8.0/24范围内进行扫描可以发现该地址范围内的解析服务器有166.111.8.28和166.111.8.29两台。为了避免权威域名服务器的相应，可以将请求的地址设置为CERNET范围外的地址，这样运行着权威服务器的主机便会拒绝DNS请求。下图显示了CERNET范围内权威域名服务器的名称与数量对应关系分布。

![CERNET范围内权威域名服务器的名称与数量对应关系分布](https://github.com/DBullet/UCore/blob/master/lab4/Figure_1.png)

#### DNS安全性测量
DNS最佳实践(RFC2812)表明，一个域名应该至少有一主一从两台权威服务器负责解析，且两台服务器最好分布在不同的网络和地理位置。如果域名只有一台权威服务器，或两台服务器在同一位置，则有潜在的可用性风险。代码./codes/dns/dns.py

1. 测试权威服务器的区域传输安全性

从之前的统计数据来看，CERNET范围内有不少主机都只有一台权威DNS服务器。对这些服务器，这部分实验会试探是否能够获取这些服务器的区域文件。
区域传输通常用于主从域名服务器之间的同步。某些域名的管理员并不希望将域名的区域文件(zone file，其中记录了这个域下所有的子域名和地址)公开。若 权威服务器不进行检查，任何用户都可以通过AXFR类型的DNS请求下载到完整的区域文件。
例如通过向192.33.4.12(根服务器-C)发送DNS根(.)的AXFR请求，就可以拿到完整的DNS根区域文件(dig @192.33.4.12 AXFR .)(一些网络环境禁止了这类请求，比如清华大学校园网，但是服务器上的这个安全漏洞仍然存在)。
![zone file](https://github.com/DBullet/UCore/blob/master/lab4/Lark20181028203954.png)
通过对之前探测到的只有一台权威服务器的域名进行测试，并没有发现这些能够获得完整DNS区域文件的服务器。

2. 测试DNSSEC的配置

DNSSEC全称Domain Name System Security Extensions，即DNS安全扩展，是由IETF提供的一系列DNS安全认证的机制（可参考RFC2535）。它提供一种可以验证应答信息真实性和完整性的机制，利用密码技术，使得域名解析服务器可以验证它所收到的应答(包括域名不存在的应答)是否来自于真实的服务器，或者是否在传输过程中被篡改过。通过DNSSEC的部署，可以增强对DNS域名服务器的身份认证，进而帮助防止DNS缓存污染等攻击。DNSSEC给解析服务器提供了防止上当受骗的武器，是实现DNS安全的重要一步和必要组成部分。

DNSSEC通过公钥密码技术对DNS中的信息创建密码签名，为DNS信息同时提供认证和信息完整性检查，它的实施步骤如下：
- DNS服务器收到DNS查询请求后，用散列函数将要回复DNS报文的内容进行散列运算，得到“内容摘要”，使用私匙加密后再附加到DNS报文中；
- DNS查询请求者接收到报文后，利用公匙解密收到的“内容摘要”，再利用散列函数计算一次DNS查询请求报文中的“内容摘要”，两者对比；
- 若相同，就可以确认接收到的DNS信息是正确的DNS响应；若验证失败，则表明这一报文可能是假冒的，或者在传输过程、缓存过程中被篡改了。

这部分实验通过对配置有DNSSEC的权威域名服务器的查询来测试常用的公共解析域名服务器是否支持DNSSEC。目前国内一般的权威域名服务器和解析域名服务器鲜有配置DNSSEC的服务器，而国内外的政府域名的权威服务器几乎都配置有DNSSEC。
向Google的公共域名解析服务器8.8.8.8查询中国政府的域名(china.gov)(其他国家的政府域名england.gov,america.gov或美国的支付软件域名paypal.com),可以看到除了一般的资源记录之外，还有类型为RRSIG(Resource Record Signature)的响应，这就是权威域名服务器对自身响应生成的认证码。而用国内的公共解析域名服务器查询，结果中的内容就没有这部分认证码，说明该解析域名服务器并没有配置DNSSEC。对于没有配置DNSSEC的权威域名服务器返回的结果，由于本身就缺少签名，所以即使用配置有DNSSEC的解析域名服务器去查询，也不会收到认证码部分。
![dnssec 8.8.8.8](https://github.com/DBullet/UCore/blob/master/lab4/Lark20181028-203247.png)
![dnssec 114.114.114.114](https://github.com/DBullet/UCore/blob/master/lab4/Lark20181028-203242.png)

3. 测试端口随机化

DNS解析服务器会缓存已经请求过的解析结果，当一个解析请求已经被解析服务器缓存且未过期时，解析服务器会直接返回缓存中的结果，而不会向权威服务器发起查询请求。缓存污染攻击就是针对DNS解析服务器缓存的攻击手段，即通过向DNS解析服务器返回错误的查询结果，使得服务器将错误的信息缓存，从而使DNS查询失败。对于从权威DNS服务器发回的响应，解析服务器只会验证transaction ID和发起请求端口号是否一致，只有一致便认为是正确的响应并接受存入缓存，对于之后到来的相同域名响应包则丢弃。transaction ID是DNS查询请求中一个16bit的域，通常是随机的，然而只依靠随机的transaction ID，根据生日攻击的原理，攻击者仍然有很大的概率与transaction ID发生碰撞而成功发起缓存污染攻击。为了降低缓存污染攻击成功的概率，通常DNS解析服务器应该采用端口随机化技术，随机的用不同的端口向权威服务器发起查询请求。而没有配置端口随机化的DNS解析服务器，当请求的端口被攻击者获取后，就很容易遭到缓存污染攻击。
![port test](https://github.com/DBullet/UCore/blob/master/lab4/Lark20181028-204115.png)
可以通过`dig porttest.dns-oarc.net TXT @dns_server_ip`命令来查询DNS解析服务器是否采用了端口随机化技术。如查询114.114.114.114可以看到服务器在26次查询中使用了26个不同的端口，并给出了这些端口的标准差。

Nmap工具中也提供了能够测试DNS服务器是否采用端口随机化工具的脚本。参考[dns-random-srcport](https://nmap.org/nsedoc/scripts/dns-random-srcport.html)。

下面的实验来对一台没有采用端口随机化的解析服务器进行Kaminsky攻击的模拟。首先在Windows主机上搭建一个十分简易的DNS解析服务器。这台服务器的工作仅仅是将收到的DNS查询请求转发给公共DNS服务器，并将收到的请求再转发给查询的主机。这台主机的服务运行在固定端口8000，且IP地址为101.5.224.165。代码见./codes/dns/dns-server.py。

接着在Mac OS主机上开启两个进程，一个会不断对DNS解析服务器发起向不存在域名的解析请求，正常情况下其收到的都将是没有应答的解析失败的响应。然而，此时同时有一个进程在向DNS服务器不停发送DNS响应数据包，且其中的应答数据正是这个不存在域名的解析结果，并将解析结果置为一个错误的ip地址。两个进程在构造DNS报文时都采用随机的transaction id，在发送请求的进程中，设置当收到有结果的解析响应后打印出响应并退出进程。也就是说，当请求进程退出时，证明攻击已经成功，即两个进程的transaction id发生碰撞，攻击进程的虚假响应被请求进程所接收。

以上的实验只是对Kaminsky攻击原理进行模拟，实际上实验中的DNS服务器并不按照正常的解析服务器的工作方式工作，它并没有缓存(当然也可以简单的对缓存进行模拟)。且Kaminsky攻击攻击成功时应当是解析服务器错误的把攻击者的响应当成了权威服务器的响应接收，在这次实验中是查询者把被服务器转发的来自攻击者的错误响应接收。实验代码见./codes/dns/KaminskyAttack.py

为了减少实验时间，可以缩小transaction id的范围(DNS 报文中的范围是[0, 65535])，例如采用[1, 5]的范围进行模拟实验，可以看到在不到3s的时间内攻击遍成功(攻击成功的时间是不确定的，有时会偏长)。

![query](https://github.com/DBullet/UCore/blob/master/homework/Lark20181103153437.png)
![attack success](https://github.com/DBullet/UCore/blob/master/homework/Lark20181103-153614.png)

通过在简易的服务器上把收到的查询请求的transaction id打印出来也可以看到，采用nslookup进行查询时，查询报文的transaction id是从1开始递增的，而采用dig进行查询时transaction id是随机的，nslookup的可预测的transaction id相比较是不太安全的。实验中直接采用scapy构造DNS数据包的形式进行查询时，Windows主机上会出现socket error: Errno(10054)的错误，这个错误的解释可以参考[stack overflow的解答](https://stackoverflow.com/questions/2576926/python-socket-error-on-udp-data-receive-10054)。使用dig 或 nslookup
查询时不会出现这个错误。


4. DNS污染与劫持

在中国大陆区域，GFW禁止了对于海外一部分站点的访问，DNS劫持就是GFW原理的一部分。这部分实验主要观察GFW对DNS的劫持。DNS请求被劫持到国内的服务器上之后，由于这些服务器的数据已经被污染了，所以一些域名的查询就会直接返回错误的结果。对于部分域名，及时能够获得其正确的解析结果，GFW还会对一些IP地址直接进行拦截，防止对这些域名的访问。

首先多次尝试用不同公共DNS服务器对被封锁的域名google.com的查询(包括一次通过tcp的查询)。查询结果如下。这些返回的域名通过查询都是在google的地址范围内，可是却都无法ping通，通过traceroute的查询结果可以推测，GFW在出口网关的路由器上对这些返回的ip地址进行了拦截。

值得注意的是，设置解析服务器为114.114.114.114和8.8.8.8的两次查询结果返回了相同的ip地址，通过对这个地址查询可以得到这个ip地址可能位于台湾地区。多次通过8.8.8.8的查询google.com，可以观察到返回的地址在216.58.--.--和172.217.--.--两个范围内。一次通过8.8.8.8的对facebook.com的查询还返回了twitter范围内的地址。可以推测即使指定了域名解析服务器为google的公共域名服务器8.8.8.8，所得到的响应也并非是真正来自8.8.8.8的响应，这些请求可能已经被劫持到国内其他的解析服务器，并伪装成8.8.8.8返回。

![dns query](https://github.com/DBullet/learn-git/blob/master/readme1/Lark20181103164758.png)
![dns query](https://github.com/DBullet/learn-git/blob/master/readme1/Lark20181103164827.png)
![dns query](https://github.com/DBullet/learn-git/blob/master/readme1/Lark20181103164902.png)
![dns query](https://github.com/DBullet/learn-git/blob/master/readme1/Lark20181103165331.png)
![dns query](https://github.com/DBullet/learn-git/blob/master/readme1/Lark20181103170746.png)

绕过国内的DNS污染和劫持的方法通常是经过VPN将请求都发送给国外的服务器。在[ChinaDNS](https://github.com/shadowsocks/ChinaDNS)这个项目中，对绕过DNS污染和劫持的方法有一定总结。项目中也提到，即使DNS解析正确，GFW也可以通过对ip地址的拦截来防止对一些站点的访问。因此，要想完全绕过GFW的阻挡，仅仅获得正确的DNS解析是不够的，通常还是需要完善的VPN软件。(使用ChinaDNS需要shadowsocks账号)

通过ChinaDNS获得的google.com域名解析结果为216.58.203.35，可以观察到这个ip地址在被劫持的DNS所返回的地址范围内，然而不通过VPN还是无法访问这个IP地址，可见这个正确的IP地址在出口网关处已经被拦截了。而实际上通过VPN可以看出，这个IP地址确实是正确的google.com的域名解析结果。
![google.com](https://github.com/DBullet/learn-git/blob/master/readme1/Lark20181103180419.png)

## DNS相关内容参考

- [DNS详解](https://blog.csdn.net/kim_weir/article/details/78465641)
- [DNS中的正向解析与反向解析 及 nslookup命令使用](https://blog.csdn.net/guyue35/article/details/50464495)。
- [Linux命令：使用dig命令解析域名](https://blog.csdn.net/reyleon/article/details/12976889/)。
- [Build your own DNS server on Linux](https://opensource.com/article/17/4/build-your-own-name-server)



