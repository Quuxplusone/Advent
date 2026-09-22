{********>>>>>>>><<<<<<<<********

>>	MODULE:	SAYIT
		13-OCT-80

		This module contains the procedure SAYIT.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	SAYIT
	.IDENT	/V0/    }

{*********************   VERB SAY  *****************************}

PROCEDURE ECHO;EXTERNAL;

FUNCTION MAGIC:BOOLEAN;EXTERNAL;

PROCEDURE PARSE;EXTERNAL;

{  "Say" -- Echo WRD2-WRD2X, unless input in response to "Say what?", in which
case we echo the entire input: "drop keys" .  Magic words override..  }
PROCEDURE SAYIT;
BEGIN 
    { Check for magic words...  }
    IF MAGIC THEN PARSE
    { Otherwise, say it....  }
    ELSE ECHO
END;
