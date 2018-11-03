from scapy.all import *
import os, re, json
import signal, time

def set_timeout(duration, callback):
    def wrap(function):
        def handle(signum, frame):
            raise RuntimeError
 
        def task(*args, **kwargs):
            try:
                signal.signal(signal.SIGALRM, handle)
                signal.alarm(duration)
                r = function(*args, **kwargs)
                signal.alarm(0)
                return r
            except RuntimeError:
                callback()
        return task
    return wrap
 
def after_timeout():
    print 'request timeout'
    return None

def get_dns_server_address():
    dns_result = os.popen('nslookup www.baidu.com').read()
    pattern = re.compile(r'Server:.*?(([0-9]{1,3}\.){3}[0-9]{1,3})')
    return pattern.findall(dns_result)[0][0]

def try_parsing(server):
    target = 'www.baidu.com'
    dns_query = IP(dst=server)/UDP(dport=53)/DNS(rd=1,qd=DNSQR(qname=target))
    response = sr1(dns_query, timeout=2)
    if response is None or response.getlayer(DNS) is None:
        return False
    elif response.getlayer(DNS).fields['an'] is not None:
        return True
    else:
        return False

# set timeout by the parameter also works    
@set_timeout(3, after_timeout)
def getAR(target):
    dns_server = get_dns_server_address()
    dns_query = IP(dst=dns_server)/UDP()/DNS(rd=1,qd=DNSQR(qname=target, qtype=2))
    # qtype=2 -- query type = NS

    response = sr1(dns_query)
    authority_response = response.getlayer(DNS).fields['ar']
    if authority_response is None:
        return None
    packet_layer_index = 0
    authority_server_list = []
    # fail to find a method to get the lays of the packer :(
    while True:
        try:
            layer = authority_response[packet_layer_index]
            rname = layer.fields['rrname']
            rdata = layer.fields['rdata']
            authority_server_list.append((rname, rdata))
            packet_layer_index += 1
        except IndexError:
            break
    print authority_server_list
    return authority_server_list

def get_edu_domains(domain_list_file):
    with open(domain_list_file, 'r') as f:
        edu_domain_names = f.readlines()
    return edu_domain_names

def authoritative_server_measure():
    edu_domain_names = get_edu_domains('edu_domain.txt')
    results = {domian_name.strip(): getAR(domian_name.strip()) for domian_name in edu_domain_names}
    with open('results.txt', 'w') as g:
        g.write(json.dumps(results))

def analysis():
    with open('results.txt', 'r') as r:
        results = json.loads(r.read())
    count = {0: 0}
    for key, value in results.items():
        if value is None:
        	count[0] += 1
        else:
        	count[len(value)] = count.get(len(value), 0) + 1
    import matplotlib.pyplot as plt
    plt.bar(count.keys(), count.values())
    plt.show()

def get_parsing_server():
    servers = []
    for i in range(256):
		target = '166.111.8.{}'.format(i) # change to your own address
		if try_parsing(target):
			servers.append(target)
    print servers
    return servers

def try_to_get_zone_file():
    with open('results.txt', 'r') as r:
        results = json.loads(r.read())
    all_servers, result_servers = [], []
    for servers in results.values():
        if servers is not None:
            server_addresses = [item[1] for item in servers if len(servers) == 1]
            all_servers.extend(server_addresses)
    all_servers = set(all_servers)
    print len(all_servers)
    command = 'dig @{} AXFR .'
    # TODO: optimize by multi threads
    for server in all_servers:
        output = os.popen(command.format(server)).read()
        print output
        if output.find('failed') == -1:
            result_servers.append(server)
    print result_servers
    return result_servers

if __name__ == '__main__':
    authoritative_server_measure()
    analysis()
    try_to_get_zone_file()