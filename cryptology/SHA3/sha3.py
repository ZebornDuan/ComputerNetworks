# -*- coding:utf-8 -*-
# python 2.7

from __future__ import print_function
from math import log
from operator import xor
from copy import deepcopy

RoundConstants = [
  0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,
  0x000000000000808B, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
  0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
  0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003,
  0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
  0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
]

RotationConstants = [
  [  0,  1, 62, 28, 27, ],
  [ 36, 44,  6, 55, 20, ],
  [  3, 10, 43, 25, 39, ],
  [ 41, 45, 15, 21,  8, ],
  [ 18,  2, 61, 56, 14, ]
]

Masks = [(1 << i) - 1 for i in xrange(65)]

class KeccakState(object):
    W = 5
    H = 5
    
    @staticmethod
    def zero():
        return [[0] * KeccakState.W for x in xrange(KeccakState.H)]
    
    @staticmethod
    def format(state):
        rows = []
        def format_x(x):
            return '%016x' % x
        for y in xrange(KeccakState.H):
            row = []
            for x in xrange(KeccakState.W):
                row.append(format_x(state[x][y]))
            rows.append(' '.join(row))
        return '\n'.join(rows)
    
    @staticmethod
    def lane2bytes(s, w):
        o = []
        for b in xrange(0, w, 8):
            o.append((s >> b) & 0xff)
        return o
    
    @staticmethod
    def bytes2lane(b_):
        r = 0
        for b in reversed(b_):
            r = r << 8 | b
        return r
    
    @staticmethod
    def bytes2string(b_):
        return ''.join(map(chr, b_))
        # chr() : ASCII -> char
    
    @staticmethod
    def string2bytes(s_):
        return map(ord, s_)
        # ord() : char -> ASCII

    def __init__(self, bitrate, b):
        self.bitrate = bitrate
        self.b = b
        self.bitrate_bytes = (int(self.bitrate) + 7) / 8
        self.lanew = self.b // 25
        self.s = KeccakState.zero()
    
    def __str__(self):
        return KeccakState.format(self.s)
    
    def absorb(self, b_):        
        b_ += [0] * ((int(self.b - self.bitrate) + 7) / 8)
        i = 0
        for y in xrange(self.H):
            for x in xrange(self.W):
                self.s[x][y] ^= KeccakState.bytes2lane(b_[i:i + 8])
                i += 8
    
    def squeeze(self):
        return self.get_bytes()[:self.bitrate_bytes]
    
    def get_bytes(self):
        out = [0] * ((int(self.b) + 7) / 8)
        i = 0
        for y in xrange(self.H):
            for x in xrange(self.W):
                    v = KeccakState.lane2bytes(self.s[x][y], self.lanew)
                    out[i:i+8] = v
                    i += 8
        return out
    
    def set_bytes(self, b_):
        i = 0
        for y in xrange(self.H):
            for x in xrange(self.W):
                self.s[x][y] = KeccakState.bytes2lane(b_[i:i+8])
                i += 8

class KeccakSponge(object):
    def __init__(self, bitrate, width, padfn, permfn):
        self.state = KeccakState(bitrate, width)
        self.padfn = padfn
        self.permfn = permfn
        self.buffer = []
        
    def copy(self):
        return deepcopy(self)
        
    def absorb_block(self, bb):
        assert len(bb) == self.state.bitrate_bytes
        self.state.absorb(bb)
        self.permfn(self.state)
    
    def absorb(self, s):
        self.buffer += KeccakState.string2bytes(s)
        while len(self.buffer) >= self.state.bitrate_bytes:
            self.absorb_block(self.buffer[:self.state.bitrate_bytes])
            self.buffer = self.buffer[self.state.bitrate_bytes:]
    
    def absorb_final(self):
        padded = self.buffer + self.padfn(len(self.buffer), self.state.bitrate_bytes)
        self.absorb_block(padded)
        self.buffer = []
        
    def squeeze_once(self):
        rc = self.state.squeeze()
        self.permfn(self.state)
        return rc
    
    def squeeze(self, l):
        Z = self.squeeze_once()
        while len(Z) < l:
            Z += self.squeeze_once()
        return Z[:l]

class KeccakHash(object):
    def __init__(self, bitrate_bits, capacity_bits, output_bits):
        def multirate_padding(used_bytes, align_bytes):
            padding_length = align_bytes - used_bytes
            if padding_length == 0:
                padding_length = align_bytes
            if padding_length == 1:
                return [0x81]
            else:
                return [0x01] + ([0x00] * (padding_length - 2)) + [0x80]

        def keccak_f(state):
            def rotate_left(value, left, bits):
                top = value >> (bits - left)
                bottom = (value & Masks[bits - left]) << left
                return bottom | top
            def round(A, RC):
                # theta
                C = [reduce(xor, A[x]) for x in xrange(state.W)]
                D = [0] * state.W
                for x in xrange(state.W):
                    D[x] = C[(x - 1) % state.W] ^ rotate_left(C[(x + 1) % state.W], 1, state.lanew)
                    for y in xrange(state.H):
                        A[x][y] ^= D[x]
                # rho and pi
                B = state.zero()
                for x in xrange(state.W):
                    for y in xrange(state.H):
                        B[y % state.W][(2 * x + 3 * y) % state.H] = rotate_left(A[x][y], RotationConstants[y][x], state.lanew)    
                # chi
                for x in xrange(state.W):
                    for y in xrange(state.H):
                        A[x][y] = B[x][y] ^ ((~ B[(x + 1) % state.W][y]) & B[(x + 2) % state.W][y])
                
                # iota
                A[0][0] ^= RC

            l = int(log(state.lanew, 2))
            nr = 12 + 2 * l
            
            for ir in xrange(nr):
                round(state.s, RoundConstants[ir])
        self.sponge = KeccakSponge(bitrate_bits, bitrate_bits + capacity_bits,
                                   multirate_padding,
                                   keccak_f)
        
        self.digest_size = (int(output_bits) + 7) / 8
        self.block_size = (int(bitrate_bits) + 7) / 8
    
    def __repr__(self):
        return '<KeccakHash with r=%d, c=%d, image=%d>' % (self.sponge.state.bitrate, \
               self.sponge.state.b - self.sponge.state.bitrate, \
               self.digest_size * 8)
    
    def copy(self):
        return deepcopy(self)
    
    def update(self, s):
        self.sponge.absorb(s)
    
    def digest(self):
        finalised = self.sponge.copy()
        finalised.absorb_final()
        digest = finalised.squeeze(self.digest_size)
        return KeccakState.bytes2string(digest)
    
    def hexdigest(self):
        return self.digest().encode('hex')
    
    @staticmethod
    def preset(bitrate_bits, capacity_bits, output_bits):
        def create(initial_input = None):
            h = KeccakHash(bitrate_bits, capacity_bits, output_bits)
            if initial_input is not None:
                h.update(initial_input)
            return h
        return create

if __name__ == '__main__':
    Keccak256 = KeccakHash.preset(1088, 512, 256)
    message = b'hello world'
    print(Keccak256(message).hexdigest())

    # Keccak224 = KeccakHash.preset(1152, 448, 224)
    # Keccak384 = KeccakHash.preset(832, 768, 384)
    # Keccak512 = KeccakHash.preset(576, 1024, 512)
