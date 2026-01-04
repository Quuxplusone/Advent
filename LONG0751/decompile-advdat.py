#!/usr/bin/env python

# This script reads from 'advtxt.bin' and 'advvar.bin' in the current
# working directory, and dumps the entire decompiled ADVDAT file
# to stdout. The file as printed does have trailing spaces; I've
# removed those trailing spaces in the copy checked into this repo.


def stringify(c):
  return chr(c) if (32 <= c <= 126) else '.'

def as_string(value, clyde = b'\0\0\0\0\0'):
  stringified = stringify(((value >> 29) & 0x7F) ^ clyde[0])
  stringified += stringify(((value >> 22) & 0x7F) ^ clyde[1])
  stringified += stringify(((value >> 15) & 0x7F) ^ clyde[2])
  stringified += stringify(((value >>  8) & 0x7F) ^ clyde[3])
  stringified += stringify(((value >>  1) & 0x7F) ^ clyde[4])
  return stringified

def from_string(s):
  value = 0
  value |= (ord(s[0]) << 29)
  value |= (ord(s[1]) << 22)
  value |= (ord(s[2]) << 15)
  value |= (ord(s[3]) <<  8)
  value |= (ord(s[4]) <<  1)
  return value

def as_signed(value):
  return (value - 0x1000000000) if (value & 0x800000000) else value

# Convert octets to raw 36-bit words.
#
def octets_to_raw(s):
  assert(len(s) % 5 == 0)
  raw_words = []
  for i in range(0, len(s) // 5):
    assert(0x00 <= s[i*5+0] <= 0x7f)
    assert(0x00 <= s[i*5+1] <= 0x7f)
    assert(0x00 <= s[i*5+2] <= 0x7f)
    assert(0x00 <= s[i*5+3] <= 0x7f)
    assert(0x00 <= s[i*5+4] <= 0xff)
    value = ((s[i*5+4] & 0x80) >> 7)  # the last bit is swizzled weird
    value |= ((s[i*5+4] & 0x7F) << 1)
    value |= s[i*5+3] << 8
    value |= s[i*5+2] << 15
    value |= s[i*5+1] << 22
    value |= s[i*5+0] << 29
    raw_words += [value]
  return raw_words

# The raw format is actually not quite what we want.
# It's chunked into chunks of 128-or-fewer words each,
# with a length marker at the front of each chunk.
# Remove those length markers.
#
def raw_to_cooked(raw_words):
  cooked_words = []
  left_in_chunk = 126
  for value in raw_words[2:]:
    if left_in_chunk == 0:
      assert((value & 0xfffffff00) == 0x010000000), value
      left_in_chunk = (value & 0xff)
      assert(1 <= left_in_chunk <= 128), value
      if (left_in_chunk == 128): left_in_chunk = 127
    else:
      cooked_words += [value]
      left_in_chunk -= 1
  assert(left_in_chunk == 0) # end-of-file should coincide with end-of-chunk
  return cooked_words

# Now chunk the words by their COMMON sections.
#
def cooked_to_sections(cooked_words):
  sections = {}
  counter = -100
  for value in cooked_words:
    counter += 1
    stringified = as_string(value)
    if stringified.endswith('CO') and stringified != 'FALCO':
      current_section = stringified + 'M'
      sections[current_section] = []
      counter = 0
    elif counter == 1:
      # This should be the trailing 'M' of a new section.
      assert(value == 0x9a8102040)
      assert(stringified == 'M    ')
    else:
      sections[current_section] += [value]
  return sections

def dump_sections(sections):
  for name, contents in sections.items():
    print('constexpr long %s[%d] = {' % (name, len(contents)))
    for i, value in enumerate(contents):
      print('  0x%09x, // %s %11d - # %d' % (value, as_string(value), as_signed(value), i))
    print('};\n')

class Words:
  def __init__(self, ktab, atab):
    self.ktab = ktab
    self.atab = atab

  def spell(self, w):
    if (w == 9999):
      return 'ANY  '
    for i, k in enumerate(self.ktab):
      if (w == k):
        word = (as_string(self.atab[2*i]) + as_string(self.atab[2*i+1])).rstrip()
        if len(word) >= 8:
          word = '%-16s' % word
        return word
    assert(False), w

def dump_descriptions(ltext, advtxt):
  for loc, offset in enumerate(ltext):
    printing = False
    for (first, i, line) in advtxt:
      if (i == offset):
        assert(first)
        printing = True
      elif printing and first:
        break
      if printing:
        print('%-8d%s' % (loc+1, line))

def dump_long_descriptions(ltext, advtxt):
  print('1')
  dump_descriptions(ltext, advtxt)
  print('-1      END')

def dump_short_descriptions(stext, advtxt):
  print('2')
  dump_descriptions(stext, advtxt)
  print('-1')

def dump_object_descriptions(ptext, advtxt):
  print('5')
  # There doesn't seem to be any direct bookkeeping of the prop messages;
  # the PSPEAK routine just walks through advtxt on-demand.
  for obj, start in enumerate(ptext):
    if start == 0:
      continue
    end = 0
    for i in range(1, 100):
      end = ptext[obj+i] if obj+i < len(ptext) else 13826
      if end != 0:
        break
    i = 0
    while (advtxt[i][1] < start):
      i += 1
    assert(advtxt[i][1] == start)
    # First print the object number
    print('%-8d%s' % (obj+1, advtxt[i][2]))
    # Now print its prop descriptions, up to but not including "end"
    propvalue = -1
    while True:
      i += 1
      if advtxt[i][1] >= end:
        assert(advtxt[i][1] == end)
        break
      if advtxt[i][0]: propvalue += 1
      print('%04d    %s' % (propvalue * 1000, advtxt[i][2]))
  print('-1')

def dump_messages(rtext, advtxt):
  print('6')
  dump_descriptions(rtext, advtxt)
  print('-1')

def dump_vocabulary(ktab, atab):
  print('3')
  assert(len(ktab)*2 == len(atab)), (len(ktab), len(atab))
  for i, k in enumerate(ktab):
    word = as_string(atab[2*i]) + as_string(atab[2*i+1])
    print('%-8d%s' % (k, word.rstrip(' ')))
  print('-1')

def dump_travel_table(keys, travl, words):
  print('4', end='')
  for loc in range(len(keys)):
    start = keys[loc]
    end = keys[loc+1] if loc+1 < len(keys) else len(travl)
    assert(0 <= start <= end <= len(travl)), (loc, start, end)
    olddest = -1
    # Each room's entries should end with a negated value.
    if start < end:
      assert(all(as_signed(vv) > 0 for vv in travl[start:end-1])), loc
      assert(as_signed(travl[end-1]) < 0), loc
    for vv in travl[start:end]:
      vv = as_signed(vv)
      if (vv < 0): vv = -vv
      dest = vv // 1000
      word = vv % 1000
      if dest != olddest:
        print('\n%-8d%-8d' % (loc+1, dest), end='')
      print('%-8s' % words.spell(word), end='')
      olddest = dest
  print('\n-1')

def dump_transitive_action_errors(txt):
  print('8')
  # E.g. the first entry is (1,24), so the default message associated
  # with action 2001 "GET" is message 24, "You are already carrying it!"
  for i, msgnum in enumerate(txt):
    print('%-8d%-8d' % (i+1, msgnum))
  print('-1')

def dump_room_attributes(loccon):
  print('9', end='')
  rooms_with_bit = {}
  for b in range(0, 36):
    rooms_with_bit[b] = []
    for i, mask in enumerate(loccon):
      if (mask & (1 << b)):
        rooms_with_bit[b] += [i+1]
  for b, rooms in rooms_with_bit.items():
    for i, loc in enumerate(rooms):
      if i % 9 == 0:
        print('\n%-8d' % b, end='')
      print('%-8d' % loc, end='')
  print('\n-1')

def dump_rating_table(cval, ctext):
  print('10')
  for (val, text) in zip(cval, ctext):
    print('%-8d%s' % (val, text))
  print('-1')

def dump_hints_section(turns, points, questions, answers):
  print('11')
  for h in range(1, 25):
    if (questions[h] != 0) or (turns[h] != 0):
      print('%-8d%-8d%-8d%-8d%-8d' % (h, turns[h], points[h], questions[h], answers[h]))
  print('-1')

def dump_preposition_table(vkey, ptab, words):
  print('14', end='')
  for verb in range(1,63):
    offset = vkey[verb-1]
    if offset == 0:
      continue
    previous_preposition = 0
    j = 0
    while True:
      w = as_signed(ptab[offset-1])
      if (w < 0): w = -w
      preposition = (w // 10000)
      noun = (w % 10000)
      assert (1 <= preposition <= 10), as_signed(ptab[offset-1])
      if preposition != previous_preposition:
        previous_preposition = preposition
        j = 0
      if (j % 8 == 0):
        print('\n%-8s%-8s' % (words.spell(2000 + verb), words.spell(4000 + preposition)), end='')
      print('%-8s' % words.spell(noun), end='')
      j += 1
      if (as_signed(ptab[offset-1]) < 0): break
      offset += 1
  print('\n-1')

def dump_object_attributes(objcon):
  print('15', end='')
  objects_with_bit = {}
  for b in range(0, 36):
    objects_with_bit[b] = []
    for i, mask in enumerate(objcon):
      if (mask & (1 << b)):
        objects_with_bit[b] += [i+1]
  for b, objects in objects_with_bit.items():
    for i, objnum in enumerate(objects):
      if i % 10 == 0:
        print('\n%-8d' % b, end='')
      print('%-8d' % objnum, end='')
  print('\n-1')

def dump_adjective_table(adjkey, adjtab, words):
  print('16')
  for i in range(1,88):
    offset = adjkey[i-1]
    if offset == 0:
      continue
    print('%-16s' % words.spell(5000 + i), end='')
    while True:
      w = as_signed(adjtab[offset-1])
      if (w < 0): w = -w
      print('%-8s' % (words.spell(1000 + w)), end='')
      if (as_signed(adjtab[offset-1]) < 0): break
      offset += 1
    print('')
  print('-1')

# Open the file and slurp in the octets.
#
def advvar_to_sections():
  with open('advvar.bin', 'rb') as f:
    octets = f.read()
  raw = octets_to_raw(octets)
  cooked = raw_to_cooked(raw)
  sections = cooked_to_sections(cooked)
  return sections

def raw_to_lines(raw, f):
  lines = []
  i = 1
  while (i < len(raw)):
    lineno = len(lines) + 1
    v = as_signed(raw[i])
    assert(v != 0)
    # The length "v" is negated if it's the first line of a message.
    is_first_line = (v < 0)
    if v < 0: v = -v
    assert(1 <= v <= 20), v # it's a line length divided by 5
    print('LINE %d ==> %d' % (lineno, as_signed(raw[i])), file=f)
    line = ''
    seed = from_string('~jxDf')
    for j in range(1, v+1):
      encrypted = raw[i+j]
      decrypted = encrypted ^ seed
      print('%5d. %09x  decrypted: %09x - %s' % (i+j,
        encrypted, decrypted, as_string(decrypted)), file=f)
      line += as_string(decrypted)
      seed = ((as_signed(decrypted) * 16807) % 0x7fffffff) & 0xfffffffff
      if (decrypted & 0x800000000): seed += 0xf80000001
    lines += [(is_first_line, i+1, line)]
    i += v+1
  assert(i == len(raw))
  return lines

def advtxt_to_lines():
  with open('advtxt.bin', 'rb') as f:
    octets = f.read()
  raw = octets_to_raw(octets)
  return raw_to_lines(raw, open('/dev/null', 'w'))


sections = advvar_to_sections()
advtxt = advtxt_to_lines()

if False:
  # Dump the uninterpreted contents of ADVTXT.
  for l,(first,i,line) in enumerate(advtxt):
    print('%-8d%-8d%s' % (l, -i if first else i, line))

if False:
  # Dump the uninterpreted contents of ADVVAR.
  dump_sections(sections)

if True:
  # section 1
  ltext = sections['LFXCOM'][631:935]
  dump_long_descriptions(ltext, advtxt)

  # section 2
  stext = sections['LFXCOM'][947:]
  dump_short_descriptions(stext, advtxt)

  # section 3
  ktab = sections['VOCCOM'][0:802]
  atab = sections['VOCCOM'][1001:2605]
  dump_vocabulary(ktab, atab)

  # section 4
  keys = sections['LFXCOM'][315:619]
  travl = sections['TRVCOM'][0:1623]
  dump_travel_table(keys, travl, Words(ktab, atab))

  # section 5
  ptext = sections['TXTCOM'][312:530]
  dump_object_descriptions(ptext, advtxt)

  # section 6
  rtext = sections['TXTCOM'][537:1078]
  dump_messages(rtext, advtxt)

  # section 7
  def explode(v):
    v = as_signed(v)
    negate = (v < 0)
    v = abs(v)
    return (v // 1000000, (v // 1000) % 1000, (v % 1000) * (-1 if negate else 1))

  # The FIXD data appears twice; I don't know why.
  assert sections['OBJCOM'][0:217] == sections['OFXCOM'][0:217]

  plac = [as_signed(v) for v in sections['OBJCOM'][1575:1792]]
  fixd = [as_signed(v) for v in sections['OBJCOM'][0:217]]
  weight = [v % 1000 for v in sections['OFXCOM'][1588:1805]]
  xpts, xprop, xloc = zip(*[explode(v) for v in sections['OFXCOM'][912:1129]])

  print('7')
  # The object names in ADVDAT don't appear as such in ADVTXT;
  # they're just comments. Substitute the objects' official inventory names.
  # This also lets us skip non-existent objects.
  ptext = sections['TXTCOM'][312:530]
  for i in range(0, 217):
    if ptext[i] == 0: continue # no such object
    name = next(txt[2] for txt in advtxt if txt[1] == ptext[i])
    name = name[:12]
    print('%-8d%-8d%-8d%-8d%-16s%-8d%-8d%-8d' % (
      i+1, plac[i], fixd[i], weight[i], name, xpts[i], xprop[i], xloc[i]))
  print('-1      -1      -1      -1      END LIST        -1      -1      -1')

  # section 8
  txt = sections['TXTCOM'][0:62]
  dump_transitive_action_errors(txt)

  # section 9
  hintlc = sections['LFXCOM'][0:314]
  loccon = sections['LOCCOM'][631:945]
  loccon = [(a | b) for (a,b) in zip(loccon, hintlc)]
  dump_room_attributes(loccon)

  # section 10
  cval = sections['TXTCOM'][87:98]
  ctext = advtxt[2500:2510]
  dump_rating_table(cval, ctext)

  # section 11
  turns = sections['HNTCOM'][52:78]
  points = sections['HNTCOM'][77:100]
  questions = sections['HNTCOM'][102:]
  answers = sections['HNTCOM'][127:]
  dump_hints_section(turns, points, questions, answers)

  # sections 12 and 13 don't exist anymore

  # section 14
  ktab = sections['VOCCOM'][0:802]
  atab = sections['VOCCOM'][1001:2605]

  vkey = sections['PRPCOM'][0:75]
  ptab = sections['PRPCOM'][75:]
  dump_preposition_table(vkey, ptab, Words(ktab, atab))

  # section 15
  objcon = sections['OBJCOM'][1350:1567]
  dump_object_attributes(objcon)

  # section 16
  ktab = sections['VOCCOM'][0:802]
  atab = sections['VOCCOM'][1001:2605]
  adjkey = sections['ADJCOM'][0:87]
  adjtab = sections['ADJCOM'][100:300]
  dump_adjective_table(adjkey, adjtab, Words(ktab, atab))
