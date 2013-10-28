#include	"common.h"

#ifdef	WIN32OS				// Jei tai Windows tipo OS, tai
#include	<conio.h>		// 1. Itraukiame darba su konsole palengvinanti '.h'.
#endif


// Funkcijos, skirtos klientines aplikacijos
// inicializacijai, antraste.
SOCKET initializeClient ( void );
int parseCommand(char* userInput);

