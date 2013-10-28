#include	"client.h"

int main (void)
{
	SOCKET clientSockDesc;   // Kliento pagrindinio soketo-klausytojo deskriptorius.
	char userInput [2000];   // Masyvas vartotojo komandoms nuskaityti.
	char *serverOutput; // rodykle serverio atsakui saugoti.
	int sendResult;          // Siuntimo funkcijos resultatui saugoti.
	char *packets [10];      // Buferiu masyvas duomenims gauti.
	int quant = 0;           // Skaitliukas gautiems paketams skaiciuoti.
	int iCounter, jCounter;  // Skaitliukai.
	fd_set readMask;         // Reikalinga selectui.
	struct timeval TimeVal;  // reikalinga selectui.
	int flag;                // Veliavele.


//------------------------------------------------------------
// Jei kompiliuojama Windows tipo sistemoje, tai reikia
// pradedant darba inicializuoti soketu biblioteka.
#ifdef	WIN32OS
	WSADATA	wsaData;
	if ( -1 == WSAStartup (MAKEWORD (1, 1), &wsaData) )
	{
		printf ("Client error: WSAStartup() failed.\n");
		exit ( EXIT_FAILURE );
	}
#endif
//------------------------------------------------------------


	// Bandome sukurti ir suristi su reikiamu adresu ir portu serverio
	// pagrindini deskriptoriu, t.y. inicializuojame serveri.
	if ( INVALID_SOCKET == (clientSockDesc = initializeClient() ) )
	{
		printf ("Client error: client initialization failed.\n");
		goto EXIT;
	}

	// Amzinas ciklas, kurio pagalba vartotojas yra rezime 'gyvas'.
	while ( 1 )
	{
  
        char *command;

		// Isvalome standartinio I/O buferi.
        fflush ( stdin );
        fflush ( stdout );
        
		// Isvalome userInput
		memset (userInput, 0, sizeof (userInput));

		// Uzklausiame vartotojo jo pasirinkimo.
		fgets (userInput, sizeof (userInput), stdin);

		userInput [strlen (userInput) - 1] = '\0';

		if ( 0 == strcmp (userInput, "") ) continue;

        printf("user input:%s\n", userInput);

        int interpretation = parseCommand(userInput);
        // neatpazinta komanda
        if ( interpretation == 0 ) {
            printf("Client-init error: Unknown command.\n");
            continue;
        }

         /*Siunciame turima paketa serveriui.*/
        if ( SOCKET_ERROR 
                == (sendResult = sendData(&clientSockDesc,
            userInput)) ) {
            printf ("Client-init error: data transmission to the server failed.\n");
            break;
        }

        // Gauname rezultata.
        if ( SOCKET_ERROR 
                == receiveData(&clientSockDesc, &serverOutput) ) {
            printf ("Client-init error: data reception from server failed.\n");
            break;
        }
        
        /*printf("%d\n", (serverOutput == NULL));*/
        printf("Client received server response: %s\n", serverOutput);

        /*printf("ERROR OCCURED\n");*/
    }

	// Uzdarome soketa ir tokiu budu
    // pranesame serveriui apie nutraukta rysi.
	closesocket (clientSockDesc);
EXIT:
    
//------------------------------------------------------------
// Jei kompiliuojama Windows tipo sistemoje, tai reikia
// baigus darba atlaisvinti soketu biblioteka.
#ifdef	WIN32OS
	if ( -1 == WSACleanup () )
	{
		printf ("Client error: WSACleanup() failed.\n");
		exit ( EXIT_FAILURE );
	}
#endif
//------------------------------------------------------------

	return 1;
}

