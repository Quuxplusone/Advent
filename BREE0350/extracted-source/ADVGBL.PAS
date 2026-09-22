{********>>>>>>>><<<<<<<<********

>>	MODULE:	ADVGBL
		28-OCT-80

		This module (and only this module) contains all the global
	data declarations.

********>>>>>>>><<<<<<<<********}

{ *********   GLOBAL DATA DECLARATIONS  ***************}

{	Data base limits, should be changed if text files are changed     }
CONST	LOCSIZ=140;	{Maximum location number}
	MAXTRS=64;	{Maximum object number}
{	Object number mnemonics	}
	LAMP=2;
	BOTTLE=20;
	WATER=21;
	OIL=22;
	KEYS=1;
	GRATE=3;
	CAGE=4;
	ROD=5;
	ROD2=6;
	STEPS=7;
	BIRD=8;
	DOOR=9;
	PILLOW=10;
	SNAKE=11;
	FISSURE=12;
	TABLET=13;
	CLAM=14;
	OYSTER=15;
	MAGAZINE=16;
	DWARF=17;
	KNIFE=18;
	FOOD=19;
	PLANT=24;
	PLANT2=25;
	AXE=28;
	MIRROR=23;
	DRAGON=31;
	CHASM=32;
	TROLL=33;
	TROLL2=34;
	BEAR=35;
	MESSAGE=36;
	VENDING=38;
	BATTERY=39;
{	Treasure number mnemonics	}
	NUGGET=50;
	COINS=54;
	CHEST=55;
	EGGS=56;
	TRIDENT=57;
	VASE=58;
	EMERALD=59;
	PYRAMID=60;
	PEARL=61;
	RUG=62;
	SPICES=63;
	CHAIN=64;
{	Motion verb mnemonics	}
	ENTRANCE=64;
	DEPRESSION=63;
	NULL=21;

	BACK=8;
	LOOK=57;
	CAVE=67;
{	Action verb mnemonics	}
	SAY=3;
	LOCK=6;
	THROW=17;
	FIND=19;
	INVENTORY=20;

{	Type declarations:

	Word types are used for vocabulary input and manipulation.  }
TYPE	WORD= ARRAY[1..5] OF CHAR;

{	Usigned integer types.  }
	UNSIGNED=0..65535;

{	Line types are used for text manipulation.  }
	LINE=ARRAY[1..72] OF CHAR;

{	Location arrays are data associated with locations  }
	LOCARRAY=ARRAY[1..LOCSIZ] OF INTEGER;

{	Object arrays are data associated with objects  }
	OBJARRAY=ARRAY[1..MAXTRS] OF INTEGER;

{	Linked object records are used to keep a linked list of objects at
	a particular location, including carried objects (which are at
	location number -1).  }
	OBJLINK=^NXTOBJ;
	NXTOBJ=RECORD
			NXT:OBJLINK;
			OBJ:INTEGER
		END;

{	Linked verb records hold the verbs applicable to a particular motion
	entry in the travel array.  }
	VERBLINK=^NXTVRB;
	NXTVRB=RECORD
			NXTVERB:VERBLINK;
			VERBVAL:INTEGER
		END;

{	Linked key records contain one set of motion information for a location.
	The second entry points to the next possible motion (another keylink),
	the first entry points to the linked list of verbs that cause this 
	motion, and the third entry contains the new location number and 
	conditional information on the motion.   }
	KEYLINK=^NXTKEY;
	NXTKEY=RECORD
			NXTVERB:VERBLINK;
			NXTLINK:KEYLINK;
			NEWLOC:ARRAY[1..2] OF INTEGER
		END;

{	Alpha table records are the nodes in the vocabulary tree.  Each record

	has an array of record numbers indexed by the ASCII value of the next
	letter in sequence in a word, followed by the numeric "definition" of
	the letter sequence so far (0 if undefined).  }
	ATAB=RECORD
			NXT:ARRAY[65..90] OF INTEGER;
			VAL:ARRAY[1..2] OF INTEGER
		END;

{	Line records are text file records.  Each contains a number followed
	by a line of text.  }
	LREC=RECORD
			LOC:INTEGER;
			TXT:LINE
		END;

{	Variable declarations:  

	Location number the player is currently at,   }
VAR	LOCATION,
{	The number of objects being carried,    }
	HOLDING:INTEGER;
{	The vocabulary table file,    }
	KATAB:FILE OF ATAB;
{	Pointers into the vocabulary table file, each corresponding to the first
	letter in a word,  }
	KTAB:ARRAY[65..90] OF INTEGER;
{	Array of lists of objects at locations,   }
	ATLOC:ARRAY[-1..LOCSIZ] OF OBJLINK;
{	Properties of objects, their immovability info, and the current
	location of each object,   }
	PROP,FIXED,PLACE:OBJARRAY;
{	"Liquid" assets, hints eligibility and other location conditions,   }
	CONDI:LOCARRAY;
{	Random number generator seed,  }
	SEED:UNSIGNED;
{	Random number generator start flag,   }
	STRTRAN:BOOLEAN;
{	Maximum number of hints,   }
	HNTMAX,
{	Tally of objects "unseen" by player,   }
	TALLY,
{	Tally of objects which can't be found (because player blew it),  }
	TALLY2,
{	Object value,  }
	OBJ,
{	Location of pirate's chest at dead end in maze,  }
	CHLOC,
{	Dead end in other maze, location of chest "clue",  }
	CHLOC2,
{	Dwarf activity level flag,   }
	DFLAG,
{	Alternate starting location for a dwarf (so as not to start with 
	one dwarf on top of player,   }
	DALTLC,
{	Number of turns taken by player so far,   }
	TURNS,
{	Number of times player has typed "west" instead of "w",   }

	IWEST,
{	Location of dwarves knife after being thrown,  }
	KNFLOC,
{	Number of times we've said "not allowed to give more details",   }
	DETAIL,
{	Number of times between long descriptions of a location,   }
	ABBNUM,
{	The maximum number of times the player may die,   }
	MAXDIE,
{	The number of times a player has died,   }
	NUMDIE,
{	Number of dwarves killed, }
	DKILL,
{	Progress in saying "fee fie foe foo",   }
	FOOBAR,
{	Bonus score gained in repository,   }
	BONUS,
{	Caveclosing timers,   }
	CLOCK1,CLOCK2,
{	Maximum possible score,   }
	MAXSCOR,
{	Maximum number of turns the lamp may be on before batteries die,   }
	LIMIT: INTEGER;
{	Text file for all messages,   }
	ADVTXT:FILE OF LREC;
{	Array of travel table starting entries for all locations,  }
	KEY:ARRAY[1..LOCSIZ] OF KEYLINK;
{	Array of record numbers for object messages,   }
	PTEXT,
{	Initial object locations,  }
	PLACE1,FIXED1,
{	Default verb respnses,  }
	ACTSPK:OBJARRAY;
{	Array of record numbers for short and long location descriptions,   }
	STEXT,LTEXT,
{	Number of times we've described a location,  }
	ABB:LOCARRAY;
{	Array of record numbers for random messages,  }
	RTEXT:ARRAY[1..300] OF INTEGER;
{	Array of record numbers for magic messages,   }
	MTEXT:ARRAY[1..100] OF INTEGER;
{	Record number for start of player class messages,   }
	CTEXT:INTEGER;
{	Hints "at location" counters,  }
	HINTLC:ARRAY[1..10] OF INTEGER;
{	Array of flags used to keep track of hints already given,   }
	HINTED:ARRAY[1..10] OF BOOLEAN;
{	Hints information: message numbers, points deducted, etc.,   }
	HINTS:ARRAY[1..10,1..4] OF INTEGER;
{	Dwarf old and current locations,   }
	ODLOC,DLOC:ARRAY[1..6] OF INTEGER;
{	"Dwarf has seen player" flags,   }
	DSEEN:ARRAY[1..6] OF BOOLEAN;
{	True if it was dark before this turn,  }
	WZDARK,
{	True if lamp running out warning given,   }
	LMWARN,

{	True if cave is closing,   }
	CLOSING,
{	True if player has discovered he can't get out of cave during closing,  }
	PANIC,
{	True if cave has closed,   }
	CLOSED,
{	True if player gave up,   }
	GAVEUP,
{	True if scoring in response to "score" request,  }
	SCORING:BOOLEAN;
{	Used for verb values,   }
	VERB,
{	New location after motion verb response,   }
	NEWLOC,
{	Old location two turns prior,   }
	OLDLC2,
{	Player score,   }
	SCORE,
{	Wordtype part of vocabulary definition,   
		0, motion verbs
		1, objects
		2, action verbs
		3, miscellaneous words    }
	WORDTYPE,
{	Number of turns allowed in a demonstration game,  }
	SHORT,
{	Raw vocabulary definitions,  }
	KK,
{	Old location, last turn,  }
	OLDLOC,
{	Index variable,  }
	I,
{	Definition within wordtype of vocabulary word,  }
	K: INTEGER;
{	Words input by player,  }
	WRD1,WRD1X,WRD2,WRD2X: WORD;
{	Program flow control flag used when dragon is killed,   }
	DRAGFLG,
{	True if demonstration game during prime time,   }
	DEMO,
{	Program flow control flag, true if game is done,   }
	DONE,
{	Program flow control flag, true if player has moved to new location,   }
	MOVED,
{	Player input parsing flag, true if we have definitions for each of the
	two allowed input words,  }
	GOTBOTH,
{	Player input parsing flag, true if we don't throw away the first word, }
	NODUMP:BOOLEAN;
{	Wizardry data file.  }
	ADVWIZ:FILE OF INTEGER;
{	Cave hours.   }
	WKDAY,WKEND,HOLID:ARRAY[1..2] OF INTEGER;
{	Holiday dates.  }
	HBEGIN,HEND:UNSIGNED;
{	Holiday name.   }
	HNAME:ARRAY[1..20] OF CHAR;

{	True if restarting a saved game.  }
	SETUP:BOOLEAN;
{	True if using VT100 database.  }
	VT100:BOOLEAN;

