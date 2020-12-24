//#define DOS
/*** Vocab Trainer ***/
/* based on vocab 1.2 */

/* done v106: implement passing filename of vocfile for DOS */
/* done v106: filename longer 16 warning */
/* todo all words must be asked once */
/* todo if wrong, then 3 times the wrong one again */
/* todo if often wrong, then ask more often */
/* todo:  hitting enter at y/n at end of so many errors do you want to continue should take me to the menu */
/* when saving file ask "file exists, do you want to overwrite?"  */ 
/* beim verlassen sagen "file not saved, are you sure you want to quit?" */
/* test all vocabulary  similar to learn vocab - linear check as of all words */

/* this is a port of my favorite program that I used to memorize words when I was a student in high-school */
/* A complete rewrite is necessary to clean up the structure and variables.  there were errors in the original version.  but the aim was only to port it line by line  from BASIC-language to C-language  */
/*  program output optimized for 80x24 screens */
/*  Oliver Kaltstein   Sept 2014, Osaka, Japan   */

/* make sleep time (call it the BrainConstant)  configurable in commercial version.  recommeded:  5 sec or longer  */
// TODO  needs to accept vocfile as parameter adn go straigt to learning, 
// also in BASIC version autoloader for existig auto.v64 file

#include <stdio.h>
#ifdef DOS
   #include <conio.h>   /* ms-dos way to clear the screen */
   #include <dos.h>     /* ms-dos way to sleep */
#else
   #include <unistd.h>  /* unix way to sleep */
   //#include <wchar.h> /* wide characters for unicode output */
#endif
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>   	//for randomizer
#include <sys/types.h>
// own filename to help showing the verison:
// https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#include <libgen.h>
#define PRINTFILE() { char buf[] = __FILE__; printf("ver:  %s\n", basename(buf)); }

/* prototypes.  if you put the functions here then you don't need prototypes */
int  load (void);
void enter (void);
void edit (void);
void delete (void);
void quiz(void);
void save(void);
void catalog(void);

/* global variables */
char ER[]="     German/English = G";
char DR[]="     English/German = E";
char DT[]="               German                              English        ";
char ET[]="               English                             German         ";
char LD[]=" German     ";
char LE[]=" English    ";
char DD[81][211]/*={"Auto","Wasser","Haus","\x2f\x2a"}*/;			/* German Vocab List */
char ED[81][211]/*={"car","water","house","\x2f\x2a"}*/;			/* English Vocab List */
char D[81];				
char E[81];				
char W[81];				
char S[81];				/* Vokabelabfragerichtung D/E oder anders rum                     */
char c;					/* used for detecting and throwing away the \n in an input-method */
char A[81];		   		/* main menu choice 						  */
int LF;					/* Laufende Nummer used as Index for deleting and changing        */
int IA;		   			/* Interaktive Auswahl                                            */
int ID=0;	   			/* index showing how many vocabularies are in memory */
int X;		   			/* loop counter */
int X1;		   			/* loop counter */
int XX;		   			/* index mapping while purging ? */
int XY;		   			/* index mapping while purging ? */
int XD;					/* counter for writing into a file */



// http://stackoverflow.com/questions/14099473/how-to-scanf-only-integer-and-repeat-reading-if-the-user-enter-non-numeric-chara
int clean_stdin()                       /* used for input of int-variables */
{
    while (getchar()!='\n');
    return 1;
}



#ifdef DOS
// todo:  implement passing filename of vocfile for DOS
int main(argv,argc)
#else
int main(int argc,char** argv)
#endif
{ 
	srand(time(NULL));    // randomizer seed allowed to run only one time.  Oliver
	/* local variables for main */
	//printf("Debug: %s ",sED[3]);
	

	/*** TITLE PAGE ***/
#ifdef DOS
	clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
/*** args ***/
        if (argc > 2) {
            printf("usage: voc [file]\n");
            exit(1);
        }
        if (argc == 2) {
            printf("trying to load... %s",argv[1]);
// this block is supposed to be in one central function, needs clean up !
            {
  	        char BM[256];				/* filename */
                char const* const fileName = argv[1];
	        FILE *file;             /* filepointer is necessary for I/O.  It must be declared on top in TurboC++ */

                file = fopen(fileName, "r");
                if (!file) {
                    fprintf(stderr, "ERROR: could not find the vocabulary-file: %s\n", argv[1]);
                    sleep(3);
                    return -1;
	        }
	        ID = 0;
Lnext:
	        ID++;
	        if (ID > 200 ) 
	        { 
	    	    fclose(file);
		    printf("too many lines*\n");
	    	    return -3; 
	        }

	        fgets(DD[ID], sizeof(DD[ID]), file);             // INPUT#1,DD[ID]
	        strtok(DD[ID], "\n");
	        fgets(ED[ID], sizeof(ED[ID]), file);             // INPUT#1,ED[ID]
	        strtok(ED[ID], "\n");
                if (strcmp(DD[ID], "\x2f\x2a") == 0) goto Lfinish; // slash asterisk combination not allowed in C 
	        goto Lnext;
Lfinish:
	        strcpy(DD[ID],"");
	        strcpy(ED[ID],"");
	        ID--;  						// ID% = ID% - 1 
       	        fclose(file);      				// CLOSE 1 
	    }
            printf("... done");
            sleep(1);
        }






	printf("\n\n\n\n");
	printf("                         V O C A B U L A R Y\n\n");
	printf("                                     T r a i n e r\n\n\n");
	printf("                                    ported from C64 to UNIX(tm)\n\n\n\n\n\n\n");
	printf("                                                      March 2014 by:\n\n");
	printf("                                                      Oliver Kaltstein\n");
	printf("                                                      NAIST, dormitory 7-311\n");
	printf("                                                      Takayama-cho, Ikoma-shi\n");
	printf("                                                      Nara-ken, Japan\n");
	PRINTFILE()
	sleep(3);

	/*** Main Menu ***/
	/*L100*/
	do
	{
/*		printf("DEBUG: ID=%i\n",ID);
		for (X=0;X<20;X++)
		{
			printf("DEBUG: DD[%d]=%s$\n",X,DD[X]);
			printf("DEBUG: ED[%d]=%s$\n",X,ED[X]);
		}
*/


#ifdef DOS
        	clrscr();
#else
     		printf("\e[1;1H\e[2J");
#endif
 		printf("\n\n");
 		printf("                         M A I N    M E N U \n\n\n");
 		printf("                         1. Load File\n\n");
 		printf("                         2. Enter Vocabulary\n\n");
 		printf("                         3. Edit Vocabulary\n\n");
 		printf("                         4. Delete Vocabulary\n\n");
 		printf("                         5. Learn Vocabulary\n\n");
 		printf("                         6. Save File\n\n");
 		printf("                         7. Directory\n\n");
 		printf("                         8. Exit\n\n");
 		printf("                select ? ");

//		scanf(" %c",&A);        /* nur ein Buchstabe wird erwartet.  need space in front of %.  this method worked.  But one \n was kept in the buffer confusing the next fgets.  we need to accept more characters */
		fgets (A, 80, stdin);    /* this method also works.  it is the prefered method, because it prevents buffer overflow.  a number 1 or 2 is causing trouble, sizeof mus be big, then long input and \n  will be swallowed and ignored */

    		/* Remove trailing newline, if there. */
		if ((strlen(A)>0) && (A[strlen (A) - 1] == '\n'))
		A[strlen (A) - 1] = '\0';

		if ((strcmp(A,"1") == 0)||(strcmp(A,"\xef\xbc\x91") == 0)||(strcmp(A,"\xe4\xb8\x80") == 0)) load();
		if ((strcmp(A,"2") == 0)||(strcmp(A,"\xef\xbc\x92") == 0)||(strcmp(A,"\xe4\xba\x8c") == 0)) enter();
                IA = 0;
		if ((strcmp(A,"3") == 0)||(strcmp(A,"\xef\xbc\x93") == 0)||(strcmp(A,"\xe4\xb8\x89") == 0)) edit();
		if ((strcmp(A,"4") == 0)||(strcmp(A,"\xef\xbc\x94") == 0)||(strcmp(A,"\xe5\x9b\x9b") == 0)) delete();
		if ((strcmp(A,"5") == 0)||(strcmp(A,"\xef\xbc\x95") == 0)||(strcmp(A,"\xe4\xba\x94") == 0)) quiz();
		if ((strcmp(A,"6") == 0)||(strcmp(A,"\xef\xbc\x96") == 0)||(strcmp(A,"\xe5\x85\xad") == 0)) save();
		if ((strcmp(A,"7") == 0)||(strcmp(A,"\xef\xbc\x97") == 0)||(strcmp(A,"\xe4\xb8\x83") == 0)) catalog();
                //printf("DEBUG %c\n",A);
	} while ((strcmp(A,"8")!=0) && (strcmp(A,"\xef\xbc\x98")!=0) && (strcmp(A,"\xe5\x85\xab")!=0) && (strcmp(A,"e")!=0) && (strcmp(A,"q")!=0));
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	return(0);
}

int load(void)
{
	char BM[256];				/* filename */
        char const* const fileName = BM;
	FILE *file;             /* filepointer is necessary for I/O.  It must be declared on top in TurboC++ */
Lload:
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	printf("\n\n                    L O A D   V O C A B U L A R Y\n\n");
	printf("\n\n    filename ? ");
        //scanf("%s",BM);
	fgets (BM, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(BM)>0) && (BM[strlen (BM) - 1] == '\n'))
		BM[strlen (BM) - 1] = '\0';
	//fgets(BM, 255, stdin);
	//BM[255] = 0;
	//printf("DEBUG: bm=%s\n",BM);
	if(strlen(BM) == 0) return -2;   //bug: this line is not working with scanf() !
	if(strlen(BM) > 16) {
             fprintf(stderr, "ERROR: filename is longer than 16 characters: %s\n", BM);
             sleep(3);
             goto Lload;
	}

        file = fopen(fileName, "r");
        if (!file) {
             fprintf(stderr, "ERROR: could not find the vocabulary-file: %s\n", BM);
             sleep(3);
             return -1;
	}
	ID = 0;
Lloadnext:
	ID++;
	if (ID > 200 ) 
	{ 
		fclose(file);
		return -3; 
	}

	fgets(DD[ID], sizeof(DD[ID]), file);       
	strtok(DD[ID], "\n");
//printf("DEBUG: DD[%d]=%s$\n",ID,DD[ID]);
	fgets(ED[ID], sizeof(ED[ID]), file);      
	strtok(ED[ID], "\n");
//printf("DEBUG: ED[%d]=%s$\n",ID,ED[ID]);
        /* note that fgets don't strip the terminating \n, that's the reason strcmp is failing */
        /* this is not acceptable for my program, I have to find a better way to read a file   */
        if (strcmp(DD[ID], "\x2f\x2a") == 0) goto Lloadfinish; // slash asterisk combination not allowed in C 
	goto Lloadnext;
Lloadfinish:
	strcpy(DD[ID],"");
	strcpy(ED[ID],"");
	ID--;  				
	fclose(file);      	
	return 0;
}

void enter(void)
{
Lenter:
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	if (ID<200) goto Lnotfull;
	for (X=0;X<10;X++) printf("  program-array full: maximum index reached\n");
	return;
Lnotfull:
	printf("\n\n                    E N T E R   V O C A B U L A R Y\n\n");
	printf("\n\n%s\n\n%s ? ",DT,LD);
	fgets (D, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(D)>0) && (D[strlen (D) - 1] == '\n'))
		D[strlen (D) - 1] = '\0';

	if(strlen(D) > 60) goto Lenter;
	if(strlen(D) == 0) goto Lenter;
Lenterredo:
// system("clear");
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	printf("\n\n                    E N T E R   V O C A B U L A R Y\n\n");
	printf("\n\n%s\n\n",DT);
	printf("%s : %s",LD,D);
	printf("\n%s ? ",LE);
//      scanf("%s",E);
	fgets (E, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(E)>0) && (E[strlen (E) - 1] == '\n'))
		E[strlen (E) - 1] = '\0';
	if(strlen(E) > 60) goto Lenterredo;
	if(strlen(E) == 0) goto Lenterredo;
// system("clear");
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	printf("\n\n                    E N T E R   V O C A B U L A R Y\n\n");
	printf("\n\n%s\n\n",DT);
	printf("%s : %s",LD,D);
	printf("\n");
	printf("%s : %s\n\n\n\n",LE,E);
	ID++;
	strcpy(DD[ID],D);
	strcpy(ED[ID],E);
	strcpy(W,"");
	strcpy(E,"");
	strcpy(D,"");
	printf("    continue y/n ? ");
Lredocont:
//        scanf("%s",W);
	fgets (W, 80, stdin);
	printf("\n");
	/* Remove trailing newline, if there. */
	if ((strlen(W)>0) && (W[strlen (W) - 1] == '\n'))
		W[strlen (W) - 1] = '\0';
	//strtok(W,"\n");
        if (strcmp(W,"") == 0) goto Lredocont; 
        if ((strcmp(W,"j") == 0)||(strcmp(W,"J") == 0)||(strcmp(W,"y") == 0)||(strcmp(W,"Y") == 0)) goto Lenter; 
        if (strcmp(W,"n") == 0) return;
	goto Lredocont;
}

void edit(void)
{
Ledit:
//TODO:  the original edit in BASIC is wrong.  It allows change beyond ID .  to make a perfect port I kept the error :-)
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	if (IA>=ID) IA=0;
	X=0;
	printf("\n\n                    E D I T   V O C A B U L A R Y\n\n");
	printf("%s\n\n",DT);
	for(X1=0;X1<10;X1++) 
	{
		IA++;
//		printf("%3d %20s %20s\n",IA,DD[IA],ED[IA]);
		printf("%3d %-35s %-s\n",IA,DD[IA],ED[IA]);    
//		wprintf("%3d %-35s %-s\n",IA,DD[IA],ED[IA]);    
	}
	printf("\n\n");
        if (strcmp(W,"a") == 0) goto Lchangenumber; 
	strcpy(W," ");
	printf("select (a-alter, e-end, n-next page) ? ");
//        scanf("%s",W);
	fgets (W, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(W)>0) && (W[strlen (W) - 1] == '\n'))
		W[strlen (W) - 1] = '\0';
        if (strcmp(W,"n") == 0) goto Ledit; 
        if (strcmp(W,"e") == 0) return; 
        if (strcmp(W,"a") != 0) 
	{
		IA=0;
		goto Ledit;       /* very dirty style */
	}
	if (IA>9) 
	{
		IA=IA-10;
		goto Ledit;  /* dirty */
	}

/* the original program doesn't check if the changed word  */
/* is within the range of existing words.  Need to check ID% variable */

	IA=0;
	goto Ledit;
Lchangenumber:
	printf("change which number ? ");
	//  scanf("%s",LF);     /* might work, but I am trying something new with clean_stdin */
	if (((scanf("%d%c", &LF, &c)!=2 || c!='\n') && clean_stdin()) || LF<1 || LF>ID || LF>200) goto Ledit;  /*error-input ??*/
/* LF>ID means that the vocab doesn't exist */
//        printf("DEBUG LF=%d\n",LF);   /*input okay */
	strcpy(D,"");
	strcpy(E,"");
	printf("%s ? ",LD);
        //scanf("%s",D);
	fgets (D, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(D)>0) && (D[strlen (D) - 1] == '\n'))
		D[strlen (D) - 1] = '\0';
	if(strlen(D) > 60) goto Ledit;
	if(strlen(D) == 0) strcpy(D,DD[LF]);
	printf("%s ? ",LE);
        //scanf("%s",E);
	fgets (E, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(E)>0) && (E[strlen (E) - 1] == '\n'))
		E[strlen (E) - 1] = '\0';
	if(strlen(E) > 60) goto Ledit;
	if(strlen(E) == 0) strcpy(E,ED[LF]);
	strcpy(DD[LF],D);
	strcpy(ED[LF],E);
	strcpy(W," ");
	LF=0;
	strcpy(D," ");
	strcpy(E," ");
	if (IA>9) IA=IA-10;
	goto Ledit;
	if (LF>ID) goto Ledit;
	goto Ledit;
}


//TODO:  the original delete in BASIC is wrong.  so I will rewrite it.  compare to vocab8b.c and to VOCAB.BAS
//TODO:  ID can become -1 if you delete the first line of an empty list.  Same problem in the BASIC version.
//       this will lead to:  ?bad subscript error in 8050
//       or:  segmentation fault in the C version of the program.
void delete(void)
{
Ldelete:
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	if(IA>=ID) IA=0;
	X = 0;
	printf("\n\n                    D E L E T E   V O C A B U L A R Y\n\n");
	printf("%s\n\n",DT);
	for(X1=0;X1<10;X1++) 
	{
		IA++;
	//	printf("%3d %20s %20s\n",IA,DD[IA],ED[IA]);
		printf("%3d %-35s %-s\n",IA,DD[IA],ED[IA]);
	}
	printf("\n\n");
        if (strcmp(W,"l") == 0) goto Lreallydelete; 
	strcpy(W," ");
	printf("select (l-delete, e-end, n-next page) ? ");
        scanf("%s",W);
        if (strcmp(W,"n") == 0) goto Ldelete; 
        if (strcmp(W,"e") == 0) return; 
        if (strcmp(W,"l") != 0) 
	{
		IA=0;
		goto Ldelete;       /* very dirty style */
	}
	if (IA>9) 
	{
		IA=IA-10;
		goto Ldelete;  /* dirty */
	}
	goto Ldelete;
Lreallydelete:
//	printf("delete current number ? ");
	printf("delete which number ? ");
	//  scanf("%s",LF);     /* might work, but I am trying something new with clean_stdin */
	if (((scanf("%d%c", &LF, &c)!=2 || c!='\n') && clean_stdin()) || LF<1 ||  LF>200 ) goto Lskip;
        printf("DEBUG LF=%d\n",LF);

	// never get here because I am checking above...  
	strcpy(DD[LF],"");
	strcpy(ED[LF],"");
	strcpy(W," ");
Lskip:
	if (IA>9) 
	{
		IA=IA-10;
        printf("DEBUG IA=%d\n",IA);
	}

        /* cleaning up from deleted position until end of array */

	printf("DEBUG: entering 4 loop");
	for (XX=LF;XX<=ID;XX++)
        { 
		XY=XX+1;        // this is causing the shift to copy back all data
        printf("DEBUG LF=%d\n",LF);
        printf("DEBUG ID=%d\n",ID);
        printf("DEBUG XX=%d\n",XX);
        printf("DEBUG XY=%d\n",XY);
		strcpy(DD[XX],DD[XY]);
		strcpy(ED[XX],ED[XY]);
		strcpy(DD[XY],"");
		strcpy(ED[XY],"");
        }
	ID--;     		// this is to shorten the list
	goto Ldelete;      /* dirty */


}


void quiz(void)
{
	int iAN;
	int iDR;
	int iAA;
	int iB;		/* Strecken - randomizer multiplier */
	int iV; 	/* Verschiebung - randomizer Offset */
	int iFF;
	int iZ1;	/* Zaehler Schleife Vokabelabfrage */
	int iZ2;
	int iHI;
	int iAS;	/* Anzahl Summe Vokabelabfrage */
	int iRO;	/* randomizer Output */
	int iX;		/* randomizer result */
	int iSF;	/* error percentage */
	char P[81];	/* pauken */

Lquiz:
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	if (ID>2) goto Lenoughdata;
	printf("\n\n\n\n\n\n\n\n\n\n         not enough data available,\n\n\n         enter or load data first.\n");
	sleep(3);
	return;
Lenoughdata:
	printf("\n\n                    L E A R N   V O C A B U L A R Y\n\n\n\n\n");
	iAN=0;
	strcpy(W," ");
	iAA=0;
	iDR=0;
	iFF=0;
	iZ2=0;
	iDR = ID / 3;
	iAA = iDR * 2;
	iHI = iDR + iAA;
	iAN = iDR + (ID - iHI);
	iAS=0;
	printf("\n     how many queries would \n\n     you like ? ");
	if (((scanf("%d%c", &iAS, &c)!=2 || c!='\n') && clean_stdin()) || iAS<0 || iAS>32767)   /*error-input ??*/  {}



Lquerynumbers:
	if (iAS>200) 
	{
		printf("\n    (no more than 200 queries)      ? ");
		if (((scanf("%d%c", &iAS, &c)!=2 || c!='\n') && clean_stdin()) || iAS<0 || iAS>200) 
		{
			iAS=0;
			goto Lquerynumbers;  
		}
	}


//        printf("DEBUG iAS=%d\n",iAS);   /*input okay */

	if (iAS<1) return;

// AS%=AS   		/* despite this I will contine to call it iAS */
	printf("\n\n\n%s\n%s ? ",ER,DR);
	fgets (S, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(S)>0) && (S[strlen (S) - 1] == '\n'))
		S[strlen (S) - 1] = '\0';
        if ((strcmp(S,"d") == 0) ||
        (strcmp(S,"D") == 0) ||
        (strcmp(S,"g") == 0) ||
        (strcmp(S,"G") == 0) ||
        (strcmp(S,"f") == 0) ||
        (strcmp(S,"F") == 0) ||
        (strcmp(S,"e") == 0) ||
        (strcmp(S,"E") == 0) ||
        (strcmp(S,"h") == 0) ||
        (strcmp(S,"H") == 0) ||
        (strcmp(S,"j") == 0) ||
        (strcmp(S,"J") == 0)) goto Lspaghetti; 
	goto Lquiz;
Lspaghetti:
for (iZ1=1;iZ1<=iAS;iZ1++)
{
//printf("DEBUG 6 doing loop iZ1=%d iAS=%d ",iZ1,iAS );

strcpy(P,"");
iZ2++;
if (iZ2==3)    /* vocab that was entered last will not be asked so much */
{
	iV=1;
	iB=iAA;
	iZ2=0;
}
else
{	
	iV=iAA+1;
	iB=iAN;
}
iRO = rand()%100;
iX = iB*iRO/100+iV;

Lrequery:
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
printf("\n\n                    L E A R N   V O C A B U L A R Y\n\n\n");
        if ((strcmp(S,"d") == 0)||(strcmp(S,"D") == 0)||(strcmp(S,"g") == 0)||(strcmp(S,"G") == 0)) goto Lbackwards; 
	printf("%s\n\n\n",ET);
	printf("%s : %s\n\n\n",LE,ED[iX]);
	printf("%s ? ",LD);

	fgets (P, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(P)>0) && (P[strlen (P) - 1] == '\n'))
		P[strlen (P) - 1] = '\0';

	if(strlen(P) == 0) goto Lrequery;

        if (strcmp(P,DD[iX]) == 0) goto Lqueryend;

	iFF++;
	printf("\n correction  : %s\n",DD[iX]);
	sleep(7);   /* this is used when learning from English to German */
	goto Lqueryend;
Lbackwards:
	printf("%s\n\n\n%s : %s\n\n\n",DT,LD,DD[iX]);
	printf("%s ? ",LE);

	fgets (P, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(P)>0) && (P[strlen (P) - 1] == '\n'))
		P[strlen (P) - 1] = '\0';
	if(strlen(P) == 0) goto Lrequery;
        if (strcmp(P,ED[iX]) == 0) goto Lqueryend;
	iFF++;
	printf("\n correction  : %s\n",ED[iX]);
	sleep(7);  /* this is used when learning German to English */
Lqueryend: {}    
}

if (iFF==0) goto Lperfect;
iSF=100*iFF / iAS;
printf ("\n\n\n\n");
printf("               unfortunately %4d  percent were wrong\n\n                       continue anyway ? Y/N ? ",iSF);
	fgets (W, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(W)>0) && (W[strlen (W) - 1] == '\n'))
		W[strlen (W) - 1] = '\0';
        goto Lonemore;
Lperfect:
printf("\n\n\n\n\n   P E R F E C T  !!    All of the answers were correct  !!!!!\n\n\n                           continue Y/N ? ");
	fgets (W, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(W)>0) && (W[strlen (W) - 1] == '\n'))
		W[strlen (W) - 1] = '\0';
Lonemore:
        if ((strcmp(W,"j") == 0)||(strcmp(W,"J") == 0)||(strcmp(W,"y") == 0)||(strcmp(W,"Y") == 0)) goto Lquiz; 
	return;
}



void save(void)
{
	char MO[256];				/* filename */
        char const* const fileName = MO;
	FILE *file;             /* filepointer is necessary for I/O.  It must be declared on top in TurboC++ */
Lsave:
#ifdef DOS
        clrscr();
#else
        printf("\e[1;1H\e[2J");
#endif
	printf("\n\n                   S A V E   V O C A B U L A R Y\n\n");
	printf("\n\n    filename  ? ");
        //scanf("%s",MO);
	fgets (MO, 80, stdin);
	/* Remove trailing newline, if there. */
	if ((strlen(MO)>0) && (MO[strlen (MO) - 1] == '\n'))
		MO[strlen (MO) - 1] = '\0';
	if(strlen(MO) == 0) return ; 
	if(strlen(MO) > 16) {
             fprintf(stderr, "ERROR: filename is longer than 16 characters: %s\n", MO);
             sleep(3);
             goto Lsave;
	}

        file = fopen(fileName, "w");
        if (!file) {
             fprintf(stderr, "ERROR: could not write the vocabulary-file: %s\n", MO);
             sleep(3);
             return ;
        }
	XD=0;
Lsavenext:
	XD++;
	if (XD>ID) goto Lsaveeof;
	fputs(DD[XD], file);
	fputs("\n", file);
	fputs(ED[XD], file);
	fputs("\n", file);
	goto Lsavenext;
Lsaveeof:
	fputs("/*", file);
	fputs("\n", file);
	fputs("/*", file);
	fputs("\n", file);
	fclose(file);
	return;
}

void catalog(void)
{
   /* DIRECTORY */

   DIR *dp;
   struct dirent *ep;     
   char A[81];		   		

#ifdef DOS
   clrscr();
#else
   printf("\e[1;1H\e[2J");
#endif
   dp = opendir ("./");
   if (dp != NULL)
   {
      while ((ep = readdir (dp)))
         puts (ep->d_name);
     (void) closedir (dp);
   }
   else
     perror ("ERROR: could not read the directory.  ");
 
   printf("Press ENTER to continue ? ");
   //scanf("%c",&A);    /* before getch() consume the newline because scanf is dirty */ 
   //getchar();
   fgets (A, 80, stdin);
   /* Remove trailing newline, if there. */
   if ((strlen(A)>0) && (A[strlen (A) - 1] == '\n'))
  	A[strlen (A) - 1] = '\0';
   printf("\n");
}

// what have I learned about input ?
// -input only as a string
// -input plenty of characters
// -don't use formated input for (unformated) human data
// -maybe if ((strlen...  is doing something good - i haven't tested it, but it's a proven method.
