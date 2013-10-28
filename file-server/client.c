#include	"client.h"

// Kliento inicializacija.
SOCKET initializeClient ( void )
{
	SOCKET clientSockDesc;						// Kliento soketo deskriptorius.
	char serverHostName [256] = {0};			// Hosto, kuriame paleistas serveris, vardas.
	unsigned short int serverPortNumber;		// Porto numeris, kuriuo serveris laukia klientu.
	struct sockaddr_in serverAddress;			// Serverio adreso struktura.
	struct hostent *ptrServerHostEntry = NULL;	// Serverio hosto informacine struktura.
	char temp [10] = {0};
#ifdef WIN32OS
	const char yes = '1';
#else
	const int yes = 1;
#endif
	char* packet;							// Bufferis pasveikinimo duomenims laikyti.
	int dummyPackQ = 0;							// Gautu paketu skaiciaus kintamasis.


	// Uzklausiame vartotojo hosto, kuriame paleistas serveris, vardo.
	while ( 0 == strcmp (serverHostName, "") ) {
		printf ("Server host name  : ");
		fgets (serverHostName, sizeof (serverHostName), stdin);
	}
	serverHostName [strlen (serverHostName) - 1] = '\0';


	// Uzklausiame vartotojo porto, kuriuo serveris laukia prisijungimu, numerio.
	while ( 0 == strcmp (temp, "") ) {
		printf ("Server port number: ");
		fgets (temp, sizeof (temp), stdin);
	}
	temp [strlen (temp) - 1] = '\0';
	serverPortNumber = (unsigned short int)atoi (temp);
		

	// Pagal turima hosto varda susizinome hosto informacija.
	if ( NULL == (ptrServerHostEntry = gethostbyname (serverHostName)) ) {
		return INVALID_SOCKET;
    }


	// Inicializuojame soketo adreso struktura.
	serverAddress.sin_family	= AF_INET;
	serverAddress.sin_port		= htons ( serverPortNumber );
	serverAddress.sin_addr		= *(struct in_addr *)ptrServerHostEntry->h_addr;
	memset (&(serverAddress.sin_zero), 0, 8);


	// Sukuriame pati soketo deskriptoriu.
	if ( INVALID_SOCKET == (clientSockDesc = socket (AF_INET, SOCK_STREAM, 0)) ) {
		return INVALID_SOCKET;
    }


	// Uzdedame opcija soketo adreso ir porto pakartotinam panaudojimui.
	if ( SOCKET_ERROR == setsockopt (clientSockDesc,
		SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) ) {

		return INVALID_SOCKET;
    }


	// Bandome prisijungti prie serverio.
	if ( SOCKET_ERROR == connect (clientSockDesc,
		(struct sockaddr *)&serverAddress, sizeof (struct sockaddr)) ) {

		closesocket (clientSockDesc);
		return INVALID_SOCKET;
	}
	

	// Isvedame informacija apie sekminga prisijungima.
	printf ("Client: connected successfully to host \'%s\' - (%s).\n",
		serverHostName, inet_ntoa (*(struct in_addr *)ptrServerHostEntry->h_addr) );

     /*Gauname serverio pasveikinima.*/
    if ( SOCKET_ERROR == receiveData(&clientSockDesc, &packet) ) {
        
        closesocket (clientSockDesc);
        return INVALID_SOCKET;
    }
	// Atspauzdiname serverio pasveikinima:
	printf ("Server message received: %s\n", packet);
    free (packet);

	// Graziname sukurto soketo deskriptoriu.
	return clientSockDesc;
}

// 0 - klaida arba neatpazinta komanda
// 1 - komanda quit
// 2 - komanda isfile[tarpas][failo vardas su keliu]
int parseCommand(char* userInput) {
    int commandCompareResult = strcmp(userInput, "quit");
    if ( commandCompareResult == 0 ) {
        return 1;
    }
    // jeigu tai ne komanda quit
    char command[] = {"isfile"};
    int commandLen = strlen(command);
    int userInputLen = strlen(userInput);
    if (userInputLen >= commandLen) {
        // jeigu eiluciu pradzios sutampa reiskias
        // radome komanda kuria naudotojas nori iskviesti
        int userInputMatchCommand = 1;
        int j;
        for (j = 0; j < commandLen; j++) {
            if ( userInput[j] != command[j] ) {
                userInputMatchCommand = 0;
            } 
        }
        if ( userInputMatchCommand == 1 ) {
            
            // po komandos turi sekti vienas tarpas
            // ir failo kelias su failo vardu
            // tad po komandos turi buti bent dar 2 simboliai

            if ( userInputLen >= commandLen + 2 
                  && userInput[commandLen] == ' '
                  && userInput[commandLen + 1] != ' ' ) {
                    // nuimame comandos pavadinima ir paliekame tik
                    // parametrus
                    /*int i;*/
                    /*for (i = commandLen + 1; i < userInputLen */
                            /*&& userInput[i] != ' '; i++) {*/
                        /*userInput[i - (commandLen + 1)] = userInput[i];*/
                    /*}*/
                    // jeigu paskutinis simbolis nebuvo tarpas reikia
                    // pastumeti simboliu skaitliuka, kad po visu
                    // simboliu padetume eil. pab. simb.
                    /*if (userInput[i] != ' ') {*/
                        /*i++;*/
                    /*}*/
                    /*userInput[i] = '\0';*/
                    return 2;
            }else {
                printf("Client error: isfile styntax:isfile filepath/filename.extension\n");
            }
        }
    }
    // nebuvo atpazinta ne viena komanda
    return 0;
}
