This is a source file distribution for the game FUNADV as implemented
in VAX Fortan.  (Well, ok, there are two pascal subroutines, which are
there mainly because they had already been written to support other
code, and I didn't bother to redo them in fortran.  Maybe for the
next version.)  The game is a direct decendant from Adventure, and
contains many of the same places...or anyway, so they might appear.
The game should prove refreshing to those who enjoy the traditional
adventure. 

This game came from Georgia Tech, and does contain a few references
that are based on the Tech computing environment in the late 1970's and
early 1980's.  These include references to the "PSGAMES FORUM" program,
which was a primative conferencing system.  One notable feature of this
program was that it held three days worth of messages.  The process of
setting up for the new day was the "Update", which was started by 
being the first user to send a message on a new day.  Users would compete
for this "honor";  getting the update, especally when many users were
trying for it, was called "snarfing" the update.

"Pete Bogman" (a handle) was a frequent user of this system who could 
have easily taught creative grammar and spelling to Biff.  Pete was also
not noted for his domestic skills.  His sister was known as "Sweat Peat".

Dwight Rudolph was a systems programmer who was somewhat of a cult figure
to the admiring undergraduates.  Besides being a Cyber Wizard in every
sense of the word, he possessed a certian "savior faire" and dry wit
that made him a defacto role model for the many system programmer
wantabes, of which I was one at the time.  

An example of Dwight's comments, from the forum code, follow:

***       THIS CODE WAS WRITTEN FOR USE WITH THE FORUM PROGRAM SUPPORTED
*         BY THE SCHOOL OF PSYCHOLOGY, GEORGIA INSTITUTE OF TECHNOLOGY. 
*         IT IS NOT COPYRIGHTED, POLYUNSATURATED, OR GUARANTEED IN ANY WAY. 
*         USE IT AT YOUR OWN RISK.  IF IT DESTROYS YOUR CRITICAL DATA BASE, 
*         THAT IS YOUR PROBLEM.  THE AUTHOR ASSUMES NO RESPONSIBILITY FOR 
*         ITS USE OR MISUSE AND WILL NOT FIX IT AT A LATER TIME TO MEET 
*         YOUR NEEDS.  THE DOCUMENTATION IS SKETCHY AND OFTEN MISLEADING. 
*         YOU HAVE BEEN WARNED.  TURN THE PAGE IF YOU DARE. 
          EJECT 

Anyway, Funadv was written by Neal White III, using the source of adventure
as a starting point.  While there are many locations in FUNADV that will
seem familiar from Adventure, don't bet on it.  There are new paths in
new directions, and things sometimes work the same.  Sometimes not.
For example, in Funadv, it is possible to carry enough water to the 
plant without having to go back.

All files in the distribution are Ascii, except for FUNADV.EXE. which is
in Hexify format.  The dehex program source, in Macro, is included.
  
The data file FUNADV.DAT should be placed in the same directory as 
FUNADV.EXE.  No logicals, set def, etc, are required.

The local maintainer should create a binary database after setting
the program up.  This allows for a somewhat faster startup.  This can
be accomplished by entering "SAVE" to the "Are you a Wizard" question
at the beginning of the game.  This requires write access to the 
directory where the game is stored.

  Charles Smith
  7 March 1992 
