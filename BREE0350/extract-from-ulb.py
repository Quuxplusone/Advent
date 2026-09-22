"""Extract every module of an RSX-11M LBR universal library (.ULB).
usage: extract-from-ulb.py <adv.ulb> <outdir>
"""
import struct, sys, os, re

R50 = " ABCDEFGHIJKLMNOPQRSTUVWXYZ$.%0123456789"
def r50(w):
    return R50[w // 1600 % 40] + R50[w // 40 % 40] + R50[w % 40]

lib, out = sys.argv[1], sys.argv[2]
d = open(lib, 'rb').read()
W = lambda o: struct.unpack('<H', d[o:o+2])[0]
assert d[0] == 2, 'not a universal library'
mnt_size, mnt_blk, mnt_alloc = d[0x1a], W(0x1c), W(0x1e)
os.makedirs(out, exist_ok=True)

mods = []
for i in range(mnt_alloc):
    o = (mnt_blk - 1) * 512 + i * mnt_size
    a, b, blk, byt = struct.unpack('<HHHH', d[o:o+8])
    if a == 0xFFFF or (a == 0 and b == 0): continue
    mods.append(((r50(a) + r50(b)).strip(), blk, byt))

print('%-7s %-4s %3s %-11s %5s %4s %7s %6s  %s' % ('module', 'type', 'ver', 'inserted', 'rtyp', 'rsiz', 'bytes', 'lines', 'notes'))
MON = 'JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC'.split()
for name, blk, byt in sorted(mods, key=lambda m: (m[1], m[2])):
    o = (blk - 1) * 512 + byt
    hsize, status = d[o], d[o+1]
    msize = (W(o+4) << 16) | W(o+6)
    yr, mo, dy = W(o+8), W(o+10), W(o+12)
    ftype, ver = r50(W(o+14)).strip(), W(o+16)
    rtyp, ratt, rsiz = d[o+0x22], d[o+0x23], W(o+0x24)
    hibk = (W(o+0x26) << 16) | W(o+0x28)
    efbk = (W(o+0x2a) << 16) | W(o+0x2c)
    ffby = W(o+0x2e)
    data0 = o + 512                      # header occupies the module's first block
    eof = (efbk - 1) * 512 + ffby
    raw = d[data0:data0 + eof]
    notes = []
    if status: notes.append('status=%d' % status)
    if msize != 512 + hibk * 512: notes.append('msize %d != 512+%d blocks' % (msize, hibk))
    assert rtyp == 2, (name, rtyp)       # R.VAR
    lines, i, longest = [], 0, 0
    while i + 2 <= len(raw):
        n = struct.unpack('<H', raw[i:i+2])[0]; i += 2
        if n > rsiz: notes.append('record %d > rsiz at %d' % (n, i)); break
        lines.append(raw[i:i+n]); longest = max(longest, n)
        i += (n + 1) & ~1
    if i != len(raw) and i != len(raw) + 1: notes.append('parse ended %d, eof %d' % (i, len(raw)))
    if longest != rsiz: notes.append('longest %d vs rsiz %d' % (longest, rsiz))
    bad = sum(1 for l in lines for c in l if (c < 32 and c not in (9, 12)) or c > 126)
    if bad: notes.append('%d odd bytes' % bad)
    fn = '%s.%s' % (name, ftype)
    with open(os.path.join(out, fn), 'wb') as fh:
        fh.write(b''.join(l + b'\n' for l in lines))
    slack = d[data0 + eof: o + msize]
    if slack.strip(b'\0'): notes.append('slack %d' % len(slack))
    print('%-7s %-4s %3d %2d-%s-%02d %3d/%d %4d %7d %6d  %s' % (name, ftype, ver, dy, MON[mo-1], yr, rtyp, ratt, rsiz, eof, len(lines), '; '.join(notes)))
print(len(mods), 'modules ->', out)
