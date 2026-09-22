{********>>>>>>>><<<<<<<<********

>>	MODULE:	PLOVER
		13-OCT-80

		This module contains the procedure PLOVER.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE PLOVER
	.IDENT /V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

{ ***************** TRAVEL TABEL PROCEDURES **************** }

{  Travel 301.  Plover-alcove passage.  Can carry only  emerald.  Note: Travel 
table must include "useless" entries going through passage, which can never be 
used for actual motion, but can be spotted by "go back".  }
PROCEDURE PLOVER;
BEGIN
    NEWLOC:=99+100-LOCATION;
    IF (HOLDING<>0)AND NOT ((HOLDING=1)AND TOTING(EMERALD)) THEN BEGIN
	NEWLOC:=LOCATION;
	RSPEAK(117)
    END
END;

