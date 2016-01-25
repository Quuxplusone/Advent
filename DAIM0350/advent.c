
/*	program ADVENT.C					*\
\*	WARNING: "advent.c" allocates GLOBAL storage space by	*\
\*		including "advdef.h".				*\
\*		All other modules use "advdec.h"		*/


#include	<stdio.h>
#include	<string.h>
#include	"advent.h"	/* #define preprocessor equates	*/
#include	"advword.h"	/* definition of "word" array	*/
#include	"advcave.h"	/* definition of "cave" array	*/

#include	"advtext.h" 	/* definition of "text" arrays	*/

#include	"advdef.h"

#define 	setmem(a,b,c)	memset(a,c,b)

main(argc, argv)
int	argc;
char	**argv;
{
	int	rflag;		/* user restore request option	*/

	rflag = 0;
	dbugflg = 0;
	while (--argc > 0) {
		++argv;
		if (**argv !=  '-')
			break;
		switch(tolower(argv[0][1])) {
		case 'r':
			++rflag;
			continue;
		case 'd':
			++dbugflg;
			continue;
		default:
			printf("unknown flag: %c\n", argv[0][1]);
			continue;
		}				/* switch	*/
	}					/* while	*/
	if (dbugflg < 2)
		dbugflg = 0;	/* must request three times	*/
	opentxt();
	initplay();
	if (rflag)
		restore();
	else if (yes(65, 1, 0))
		limit = 1000;
	else
		limit = 330;
	saveflg = 0;
	srand(511);				/* seed random	*/
	while(!saveflg)
		turn();
	if (saveflg)
		saveadv();
	fclose(fd1);
	fclose(fd2);
	fclose(fd3);
	fclose(fd4);
	exit(0);				/* exit = ok	*/
}						/* main		*/

/* ************************************************************	*/

/*
	Initialize integer arrays with sscanf
*/
scanint(pi, str)
int	*pi;
char	*str;
{

	while (*str) {
		if  ((sscanf(str, "%d,", pi++)) < 1)
			bug(41);	/* failed before EOS	*/
		while (*str++ != ',')	/* advance str pointer	*/
			;
	}
	return;
}

/*
	Initialization of adventure play variables
*/
initplay()
{
	turns = 0;

	/* initialize location status array */
	setmem(cond, (sizeof(int))*MAXLOC, 0);
	scanint(&cond[1], "5,1,5,5,1,1,5,17,1,1,");
	scanint(&cond[13], "32,0,0,2,0,0,64,2,");
	scanint(&cond[21], "2,2,0,6,0,2,");
	scanint(&cond[31], "2,2,0,0,0,0,0,4,0,2,");
	scanint(&cond[42], "128,128,128,128,136,136,136,128,128,");
	scanint(&cond[51], "128,128,136,128,136,0,8,0,2,");
	scanint(&cond[79], "2,128,128,136,0,0,8,136,128,0,2,2,");
	scanint(&cond[95], "4,0,0,0,0,1,");
	scanint(&cond[113], "4,0,1,1,");
	scanint(&cond[122], "8,8,8,8,8,8,8,8,8,");

	/* initialize object locations */
	setmem(place, (sizeof(int))*MAXOBJ, 0);
	scanint(&place[1], "3,3,8,10,11,0,14,13,94,96,");
	scanint(&place[11], "19,17,101,103,0,106,0,0,3,3,");
	scanint(&place[23], "109,25,23,111,35,0,97,");
	scanint(&place[31], "119,117,117,0,130,0,126,140,0,96,");
	scanint(&place[50], "18,27,28,29,30,");
	scanint(&place[56], "92,95,97,100,101,0,119,127,130,");

	/* initialize second (fixed) locations */
	setmem(fixed, (sizeof(int))*MAXOBJ, 0);
	scanint(&fixed[3], "9,0,0,0,15,0,-1,");
	scanint(&fixed[11], "-1,27,-1,0,0,0,-1,");
	scanint(&fixed[23], "-1,-1,67,-1,110,0,-1,-1,");
	scanint(&fixed[31], "121,122,122,0,-1,-1,-1,-1,0,-1,");
	scanint(&fixed[62], "121,-1,");

	/* initialize default verb messages */
	scanint(actmsg, "0,24,29,0,33,0,33,38,38,42,14,");
	scanint(&actmsg[11], "43,110,29,110,73,75,29,13,59,59,");
	scanint(&actmsg[21], "174,109,67,13,147,155,195,146,110,13,13,");

	/* initialize various flags and other variables */
	setmem(visited, (sizeof(int))*MAXLOC, 0);
	setmem(prop, (sizeof(int))*MAXOBJ, 0);
	setmem(&prop[50], (sizeof(int))*(MAXOBJ-50), 0xff);
	wzdark = closed = closing = holding = detail = 0;
	limit = 100;
	tally = 15;
	tally2 = 0;
	newloc = 3;
	loc = oldloc = oldloc2 = 1;
	knfloc = 0;
	chloc = 114;
	chloc2 = 140;
/*	dloc[DWARFMAX-1] = chloc				*/
	scanint(dloc, "0,19,27,33,44,64,114,");
	scanint(odloc, "0,0,0,0,0,0,0,");
	dkill = 0;
	scanint(dseen, "0,0,0,0,0,0,0,");
	clock1 = 30;
	clock2 = 50;
	panic = 0;
	bonus = 0;
	numdie = 0;
	daltloc = 18;
	lmwarn = 0;
	foobar = 0;
	dflag = 0;
	gaveup = 0;
	saveflg = 0;
	return;
}

/*
	Open advent?.txt files
*/
opentxt()
{
	fd1 = fopen("advent1.txt", "r");
	if (!fd1) {
		printf("Sorry, I can't open advent1.txt...\n");
		exit();
	}
	fd2 = fopen("advent2.txt", "r");
	if (!fd2) {
		printf("Sorry, I can't open advent2.txt...\n");
		exit();
	}
	fd3 = fopen("advent3.txt", "r");
	if (!fd3) {
		printf("Sorry, I can't open advent3.txt...\n");
		exit();
	}
	fd4 = fopen("advent4.txt", "r");
	if (!fd4) {
		printf("Sorry, I can't open advent4.txt...\n");
		exit();
	}
}

/*
		save adventure game
*/
saveadv()
{
	char	*sptr;
	FILE	*savefd;
	char	username[13];
	int i;

	printf("What do you want to name the saved game? ");
	scanf("%s", username);
	if (sptr = strchr(username, '.'))
		*sptr = '\0';		/* kill extension	*/
	if (strlen(username) > 8)
		username[8] = '\0';	/* max 8 char filename	*/
	strcat(username, ".adv");
	savefd = fopen(username, "wb");
	if (savefd == NULL) {
		printf("Sorry, I can't create the file...%s\n", \
		       username);
		exit();
	}

	putw(turns,savefd);	/* mh 30-Aug-1988 */
	putw(loc,savefd);
	putw(oldloc,savefd);
	putw(oldloc2,savefd);
	putw(newloc,savefd);
	for(i=0;i<MAXLOC;i++) putw(cond[i],savefd);
	for(i=0;i<MAXOBJ;i++) putw(place[i],savefd);
	for(i=0;i<MAXOBJ;i++) putw(fixed[i],savefd);
	for(i=0;i<MAXLOC;i++) putw(visited[i],savefd);
	for(i=0;i<MAXOBJ;i++) putw(prop[i],savefd);
	putw(tally,savefd);
	putw(tally2,savefd);
	putw(limit,savefd);
	putw(lmwarn,savefd);
	putw(wzdark,savefd);
	putw(closing,savefd);
	putw(closed,savefd);
	putw(holding,savefd);
	putw(detail,savefd);
	putw(knfloc,savefd);
	putw(clock1,savefd);
	putw(clock2,savefd);
	putw(panic,savefd);
	for(i=0;i<DWARFMAX;i++) putw(dloc[i],savefd);
	putw(dflag,savefd);
	for(i=0;i<DWARFMAX;i++) putw(dseen[i],savefd);
	for(i=0;i<DWARFMAX;i++) putw(odloc[i],savefd);
	putw(daltloc,savefd);
	putw(dkill,savefd);
	putw(chloc,savefd);
	putw(chloc2,savefd);
	putw(bonus,savefd);
	putw(numdie,savefd);
	putw(object1,savefd);
	putw(gaveup,savefd);
	putw(foobar,savefd);
	putw(saveflg,savefd);
	putw(dbugflg,savefd);

	if (fclose(savefd)  ==  -1) {
		printf("Sorry, I can't close the file...%s\n", \
		       username);
		exit();
	}
	printf("OK -- \"C\" you later...\n");
}

/*
	restore saved game handler
*/
restore()
{
	char	username[13];
	FILE	*restfd;
	int	i;
	char	*sptr;

	printf("What is the name of the saved game? ");
	scanf("%s", username);
	if (sptr = strchr(username, '.'))
		*sptr = '\0';		/* kill extension	*/
	if (strlen(username) > 8)
		username[8] = '\0';	/* max 8 char filename	*/
	strcat(username, ".adv");
	restfd = fopen(username, "rb");
	if (restfd == NULL) {
		printf("Sorry, I can't open the file...%s\n", \
		       username);
		exit();
	}

	turns=getw(restfd);	/* mh 30-Aug-1988 */
	loc=getw(restfd);
	oldloc=getw(restfd);
	oldloc2=getw(restfd);
	newloc=getw(restfd);
	for(i=0;i<MAXLOC;i++) cond[i]=getw(restfd);
	for(i=0;i<MAXOBJ;i++) place[i]=getw(restfd);
	for(i=0;i<MAXOBJ;i++) fixed[i]=getw(restfd);
	for(i=0;i<MAXLOC;i++) visited[i]=getw(restfd);
	for(i=0;i<MAXOBJ;i++) prop[i]=getw(restfd);
	tally=getw(restfd);
	tally2=getw(restfd);
	limit=getw(restfd);
	lmwarn=getw(restfd);
	wzdark=getw(restfd);
	closing=getw(restfd);
	closed=getw(restfd);
	holding=getw(restfd);
	detail=getw(restfd);
	knfloc=getw(restfd);
	clock1=getw(restfd);
	clock2=getw(restfd);
	panic=getw(restfd);
	for(i=0;i<DWARFMAX;i++) dloc[i]=getw(restfd);
	dflag=getw(restfd);
	for(i=0;i<DWARFMAX;i++) dseen[i]=getw(restfd);
	for(i=0;i<DWARFMAX;i++) odloc[i]=getw(restfd);
	daltloc=getw(restfd);
	dkill=getw(restfd);
	chloc=getw(restfd);
	chloc2=getw(restfd);
	bonus=getw(restfd);
	numdie=getw(restfd);
	object1=getw(restfd);
	gaveup=getw(restfd);
	foobar=getw(restfd);
	saveflg=getw(restfd);
	dbugflg=getw(restfd);

	    if (fclose(restfd)	==  -1) {
		printf("Warning -- can't close save file...%s\n", \
		       username);
	}
}


