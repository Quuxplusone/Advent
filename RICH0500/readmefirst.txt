AdventP - Adventure in Pascal

Read this First

1. Unzip the file into a directory.  
2. Double click the adventp.exe file

Quirks

Save files do not accept '.', so the save files are untyped files (ie. have no extention on the back).  They work, though (as far as I know).

After quitting, there is an extra input, so as to let you read the final message

Files

Main directory - the program, data file, and this readmefirst file.

SRC - the program source.  The *.pas file is the actual source; the other files are used by the Lazarus IDE.

ORIGINALS - the original listing I used (a copier at work turned it into a PDF file), and the data file as sent to me by George Richmond 



Changes 

For the most part, I tried to keep as much of the original code intact as possible.  Some things, however, weren't possible.  The version of Pascal I used (FreePascal with the Lazarus IDE) had some differences from the Pascal of the 70's.  GOTOs, used in a few places to get out of processing once it was known that the input wasn't going to make sense, cannot jump out of routines.  To program around this I sometimes set a variable to signal that a goto was occuring in a called routine, and to continue the abort actions in this one.  I'm not sure I got it all correct, though, as testing hasn't finished. 

Another minor, but annoying change is the lack of a parameterized new() built-in function.  With a variant Record you could tell the new function which variant to allocate, and you would get only enough storage for that variant.  Because that is missing, every location object has storage for 10 lines of description, whether it needs it or not.

Another thing that is in the current versions of Pascal are real strings.  In CDC Pascal, a string was a static array of characters.  If you wanted to process it as if it wasn't the entire array, it was up to the program.  And that is what AdventP does.  It puts strings into arrays and fills the end of the string with CHR(0) characters, which is not a printable character in the CDC 6 bit character code, nor in ASCII.  I could have replaced all that processing with real dynamic strings, but as stated, I wanted this to be as close to the original as possible.

The data file didn't quite match my version of the code.  For one thing, there was a set of hints provided, which was not in the code I had.  I dropped them.  Another strange change was the random attackers were called Orcs in my version of the code, but the data file had it changed to Dwarf.  I kept the Orcs.

There were some changes to the way end of lines were processed.  The EOS (End of Segment) built-in function is no longer there, so that is out.    

The biggest change was the way the Save and Restore worked.  A small assembler program would write out memory as an executable file.  When the file was run, the game would pick up exactly where it left off, with the state of the game in the same memory locations.  This was also used in the game right after reading in the data file and building the internal structures to store it.  So, theoretically, the data file wasn't needed for running the program, all the data was already read in.

As that was a solution specific to the NOS operating system, I had to do something different.  I wrote my own Save and Restore routines.  I discovered some strange things about the internals:

The program has two distinct phases, the initialization stage, and the game stage.  During initialization, input comes from the data file.  During game play, input comes from the termial.  During initialization, data is kept as a linked list.  At the end of initialization the linked list is converted into a tree, and the room names are removed from the objects (as the player never refers to the rooms when giving commands to the computer).  For the save/restore function, I kept the names, as I needed to save the state of the rooms.

The original program, as stated, ran on a CDC NOS system.  The Control Data Corperation Cyber machines (I think the one we used was a 170) had 60 bit words and 6 bit characters.  One aspect of this was everything was upper case.  I changed that as I typed in the program.  As FreePascal doesn't care about case in the program, I wasn't entirely consistent with my capitalization of names.

CDC Pascal (actually, any early Pascal) had no initialization statements, and limitted memory.  In the program you can see where I initialized a number of arrays with strings of object names.  I used these in debug code (not all of which has been removed).  Theoretically, several sections of the data file could be removed and placed into the program using these initializations.   
 		
		
History of this recreation 

I played the game on the University of Minnesota's CDC mainframe running the NOS operating system in the late 1970's.  Fascinated with the original FORTRAN version of Adventure (the 350 point Crowther and Woods version) the Pascal version was much more readable, and at least a bit more understandable.  (I didn't realize some of the internals until I had to debug this version.)

When I worked at CDC in the mid 1980's I printed a copy of the source on the lazer printer. It was delivered in a sealed plastic bag, which ended up in a file in my house and stayed there for over 25 years.  Then I was cleaning up, opened the package, and found the printout.  Unfortunately, I did not keep a copy of the data file.  

The program comments start:

   Written by
      George H. Richmond
      Storage Technology Corperation

   With the support of
      Mike Preston
      University of Colorado at Boulder
      English Department  
		
With some help from Mike Preston, who I could find, I was finally able to get in contact with George Richmond, who I had trouble finding.  He was nice enough to send a copy of the data file.  There were a few things that didn't match the source, but I was able to modify it to work.  

I ran the listing though a copier that converted it into PDF format, in the hopes of using an Optical Character Reader to convert it back to text, but the OCR only got about 1 in 5 characters correct.  I typed it in by hand.		
		