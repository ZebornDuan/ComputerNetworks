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