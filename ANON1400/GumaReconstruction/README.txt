Files received by email from Alex Guma, 2023-08-01.

If I understand correctly, this is a slightly reconstructed version of
the game, preserved and then edited (in 2023) by Alex Guma.
He writes (2023-07-31):

> To create the data files, I went through some different approaches,
> using Kermit to copy the data in plain text format, but as I think I
> mentioned in an earlier email, the programs expect them in a BASIC file
> format. [...] since we do not have a data editor, I will
> create simple programs we can paste over and run to re-create the files
> with new data each time we make a change.

And also (2023-08-01):

> I have updated the movement matrix to remove the exit from the room 65,
> the Train.  I wonder if this was one of the changes between version 2
> and 3, because my data listings clearly show an exit for that room. 
> After I did this, the train now appears to work.
>
> You may also have noticed that in the burgerland station room you get
> the train and the card reader listed twice.  This also looks like a
> change between the two versions in that certain objects are put into the
> room via the logic rather than by the data file.  I assume this was done
> because of how these two need to appear in both stations.  I have
> modified the object file to hide those two and get rid of the
> duplication.  I believe there are a few other cases, but I have not
> addressed those yet.  If you spot something like that, let me know.
>
> By the way, if you want to be able to hop to any room you want, there is
> a cheat that I enabled by setting the Z7 value to 1 at the start of the
> program. You can now use the ALLEZ command.  If you type that in, you
> will can enter a room number (without an obvious prompt).  You can then
> jump directly to that room.
