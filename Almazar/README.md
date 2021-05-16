## The Search for Almazar (Winston M. Llamas, 1981)

_The Search for Almazar_ was written in BASIC by Winston Llamas,
a computer science student at RPI, circa 1981.
For the original source code of _Almazar_ (or as close as I've
been able to get), see https://github.com/Quuxplusone/Almazar .

Here, in this directory, is a port of _Almazar_ written in C
by Arthur O'Dwyer, in May 2021. I based the code mainly on
Bob Liddelow's SIG/M version, but based the message texts
on Llamas's _80 Micro_ version. (Liddelow's texts were obviously
abbreviated, and he'd discarded the short room descriptions
entirely.)

This is _not_ a perfectly faithful port:

- _80 Micro_'s messages are all-caps, except for the intro text.
    I've lowercased them, _not_ adhering closely to Liddelow's choices.

- I've "tersened" some short room descriptions in _Adventure_ style;
    for example, _80 Micro_ says `YOU'RE IN A THREE WAY JUNCTION.`
    but I say `You're at three-way junction.`

- I've copyedited lots of punctuation and prepositions, and typos,
    and the spelling of `PHYRANIAS` (piranhas).

- I've fixed an original bug where if you were carrying the bottle
    of water when you died, it would _not_ get randomly scattered
    with the rest of your possessions.
