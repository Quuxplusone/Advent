#!/usr/bin/perl
#
$INFILE = 'advII.txt';              # Plain text version of the database
$OUTFILE = '>advII.dat';            # Scrambled database
open INFILE or die "$INFILE: $!\n";
open OUTFILE or die "$OUTFILE: $!\n";
while (<INFILE>)
{
   print OUTFILE $_;                # Read section number
   chomp;
   if (/^( 1| 2| 5| 6|10|12)$/)     # Text sections
   {
      while (<INFILE>)
      {
         if (/^ +-1/)               # Check for the section terminator
         {
            print OUTFILE "$_";
            last;
         }
         /^(....)(.*)$/;            # Split into line ID and line text
         $number = $1;
         $line = uc $2;             # Uppercase the text
         $len = length($line);
         $app = 4 * int (($len + 3) / 4) - $len;
         $line = ' ' x $app . $line;   # Pad line to 4xN characters
         $chunk = $len + $app;      # Spit the line out from the end...
         print OUTFILE $number;     # ... in chunks of 4 characters
         print OUTFILE substr ($line, $chunk, 4) while (($chunk -= 4) >= 0);
         print OUTFILE "\n";
      }
   }
   else                    # Non-text sectinos (including vocabulary)
   {
      while (<INFILE>)
      {
         print OUTFILE $_;          # Just echo verbatim
         last if (/^ +-1/);         # Check for end of section
      }
   }
}
