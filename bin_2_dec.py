import struct
import sys
from codecs import decode


def bin_2_dec(num):
    bf = int_2_bytes(int(num, 2), 8)
    return struct.unpack('>d', bf)[0]


def int_2_bytes(n, length):
    return decode('%%0%dx' % (length << 1) % n, 'hex')[-length:]


print(bin_2_dec(sys.argv[1]))
