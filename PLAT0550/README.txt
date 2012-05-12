This bundle contains Dave Platt's original source of Adv550 (written in
A-code) and F77 versions of his munger and executive, previously
thought to be irretrieavably lost.

The A-code source is the unmodified original dated 18 Sep 1984, as
preserved in the IF Archive together with the PL6 versions of the munger
and the executive.  The F77 sources are derived from the Prime F77
implementation, as distributed in mid-80s.  They have been
reconstructed by stripping from the Glaxo 1989 versions (recently found in a
private archive) all of my later additions and inprovements. 

The original PL6 sources can be found in the PL6 sub-directory. 
Unfortunately, they are not of much practical use these days. 

The F77 sources are known to compile and work using GNU g77 and SGI f77.  All
comment lines beginning with C* rather than just C, were added by me, to
enhance code readability.  I am unaware of any other significant
differences from Dave Platt's original code. 

The munger (munge.f and munge.i) is the utility which reads the A-code
source (the name of the source file being hard-wired into it as
ADVENTURE.ACODE) and writes out four pseudo-binary files ADVINIT1.DAT
through to ADVINIT4.DAT.  It can be built with "g77 munge.f -o munge". 

The four advinit files contain respectively:

1. The tokenised pseudo-binary of the game code.
2. The (lightly) encoded text file.
3. Pseudo-binary symbol table.
4. Game vocabulary.

NB: I've preserved the original upper casing of the file names, however
odd this looks on modern machines.

The executive (source in exec.f and exec.i) is effectively an
interpretor, which reads in these files and processes the tokenised
code. To build it use "g77 exec.f -o exec"

This version of the source follows the original practice of *not*
reading the text file into memory, but accessing the relevant text
record by formatted reads from the text file.

Please note that some functionality of the executive (the HOURS, NEWS
and SVAR subroutines) are system implementation dependent, and hence are
provided merely as functional stubs.  In theory this could (e.g.) mean
that a time delay in restoring a game (which is coded for in Dave
Platt's A-code source), would make restoring a game impossible. 
Fortunately the A-code source also sets the mandatory delay as zero,
presumably for that very reason.  However, the game as we played it in
the 80s did have an enforced delay, though I cannot remember its length. 

                                 Mike Arnautov, 26 Jan 2003
