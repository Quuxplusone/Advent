{********>>>>>>>><<<<<<<<********

>>	MODULE:	RDWIZ
		21-OCT-80

		This module contains the "wizardry" routine RDWIZ and the 
	function BIT.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE	RDWIZ
	.IDENT /V0/   }

{ This procedure reads one integer sequentially from ADVWIZ.  }
PROCEDURE RDWIZ(VAR X:INTEGER);
BEGIN
    X:=ADVWIZ^;
    GET(ADVWIZ)
END;

{  *************** DATA STRUCTURE ROUTINES  ******************  }

{  This function returns the value 2**I .  }
FUNCTION BIT(I: INTEGER): INTEGER;
VAR J,L:INTEGER;
BEGIN
    J:=1;
    IF I<>0 THEN FOR L:=1 TO I DO J:=J*2;
    BIT:=J
END;

