This is Arthur O'Dwyer's manual transcription from a sheaf of
fan-fold paper given to me by Mike Willegal in December 2025.

It's a printout of a version of WOOD0350 ported to SEL32/RTM.
The SEL 32 was a machine produced by Systems Engineering Laboratories
starting in 1975; RTM was its operating system, the
"Real Time Monitor."

The printout itself was printed on the morning of 1979-03-21.


==Authorship==

Comments in the Fortran code credit the following authors
with porting the game. There are no intentional differences
in the *content* of this game: in that respect it is
vanilla WOOD0350.

- Gary Palter at MIT produced a relatively portable port of Adventure,
    accompanied by a "conversion guide" for new machines,
    sometime before 1978.
- Dick Reynolds ported this into "HCSD version 01.112777" (presumably 1977-11-27).
    I don't know what "HCSD" stands for; perhaps Harris County
    (Georgia) or Hillsborough County (Florida) School District.
- Ned Horvath ported this version to SEL32/RTM (1978-08-01).
- C. Norwood suppressed a few compiler warnings (1978-10-11).

As of 2026-01-05, the Adventure Family Tree taxonomizes
only PALT0350; it appears that PALT0350 begat Reynolds' HCSD0350
begat Horvath and Norwood's HORV0350.


==Notes on the transcription process==

This transcription has "curatorial markings" that need to be
removed before you could really do anything technical with it.
You can filter out those markings, and filter out the printer's
running header lines, like this:

    cat transcription.txt \
      | grep -v '^==p' \
      | grep -v '^   21MAR79'

You will still need to split the transcription into individual
files, and filter out the extraneous job-control stuff: look
for lines starting with `$ASSIGN1`, for example.

Caveat lector: This transcription was not typed in 100% by hand. Instead,
I started by copying this repository's copy of WOOD0350 into the transcript,
and then edited it to match the printout. This saved a ton of time, but
it means there are *two* possible sources of error here: first, I might
have made a typo, and second, I might have accidentally left something
matching WOOD0350 which was supposed to have changed. However, I don't
think there are any errors. If you are pretty sure you've found an error,
let me know, and I'll pay you $5 if you're right.

I found no changes in the first three data sections. In section 4,
Palter(?) adds "G" and "T" as synonyms for GET/TAKE,
"SLAY" for KILL, "Q" for QUIT. Palter removes "?" for HELP.

The code printout was truncated at 72 columns. (It had line numbers
in the right margin; I have omitted those in this transcription.)
The data printout was truncated at 80 columns.
So, anything you see beyond that point (such as the eighth exit from
room 108 in the data file) is technically just conjectural. However,
in all cases the conjecture is copied straight from some more-complete
port, and in most cases it's copied straight from WOOD0350. In no case
did I just "invent" text beyond the 72nd or 80th column.

Truncating the data file at 80 columns on print-out is historically
significant. ANON0501, MCDO0551, and ROBE0665, among others,
seem to have suffered 80-column truncation at some point in their
history, because room 10 (the cobble crawl) is absent from their list
of rooms with bitflag 0 (light). Less observably, rooms 51 and 87
are absent from their list of rooms where the "maze" hint may be offered.
It is conceivable, but I judge it very unlikely, that HORV0350
also had its data file truncated like this.
