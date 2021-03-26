import struct
import sys
from codecs import decode


def bin_2_dec(num):
    bf = int_2_bytes(int(num, 2), 8)
    return struct.unpack('>d', bf)[0]


def int_2_bytes(n, length):
    return decode('%%0%dx' % (length << 1) % n, 'hex')[-length:]


def dec_2_bin(num):
    number = float(num)
    [d] = struct.unpack('>Q', struct.pack('>d', number))
    return '{:064b}'.format(d)


print(dec_2_bin(sys.argv[1]))
# print(bin_2_dec(dec_2_bin('972.072072072072')))
