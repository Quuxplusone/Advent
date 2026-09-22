{********>>>>>>>><<<<<<<<********

>>	MODULE:	NOWAY
		13-OCT-80

		This module contains the procedure NOWAY.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE NOWAY
	.IDENT /V0/    }

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

{ ***************** TRAVEL TABLE PROCEDURES ******************** }

{  Non-applicable motion.  Various messages depending upon word given.  }
PROCEDURE NOWAY;
VAR I:INTEGER;
BEGIN
    NEWLOC:=LOCATION;
    IF (K>=43)AND(K<=50) THEN I:=9 ELSE CASE K OF
	29,30: I:=9;
	7,36,37: I:=10;
	11,19: I:=11;
	62,65: I:=42;
	17: I:=80;
	ELSE I:=12
    END;
    RSPEAK(I)
END;

