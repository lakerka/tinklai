#include "server.h"


char fileDescriptions[1000][100];
int fileCount = 0;

SOCKET initializeServer (void) {

    //  Serverio soketo deskriptorius.
    SOCKET serverSockDesc;                     
    
    //  Hosto, kuriame serveris paleistas, vardas.
    char cServerHostName [256] = {0};          
    
    //  Serverio adreso struktura.
    struct sockaddr_in serverAddress;          

    //  Serverio hosto informacine struktura. (hostent)
    struct hostent *ptrServerHostEntry = NULL; 
    
#ifdef WIN32OS
        const char yes = '1';
#else
        const int yes = 1;
#endif

    // Susizinome hosto, kuriame paleistas serveris, varda.
    // grazina hostent struktura, kurioje yra ip adresas ir kt info
    if (INVALID_SOCKET == gethostname (cServerHostName, sizeof (cServerHostName))) {
        return INVALID_SOCKET;
    }
    // Pagal turima hosto varda susizinome hosto informacija.
    if (NULL == (ptrServerHostEntry = gethostbyname (cServerHostName))) {
        return INVALID_SOCKET;
    }

    // Inicializuojame soketo adreso struktura.
    serverAddress.sin_family = AF_INET; //AF_INET = 2, ipv4 

    // priskiriamas portas
    serverAddress.sin_port = htons(SERVER_PORT); 

    // priskiriamas IP adresas
    serverAddress.sin_addr = *(struct in_addr*)ptrServerHostEntry->h_addr; 
    
    memset(&(serverAddress.sin_zero), 0, 8); 

    // Sukuriame pati soketo deskriptoriu.
    // socket(address family, type, protocol to be used)
    if (INVALID_SOCKET == (serverSockDesc = socket (AF_INET, SOCK_STREAM, 0))) {
        return INVALID_SOCKET;
    }

    // Uzdedame opcija soketo adreso ir porto pakartotinam panaudojimui.
    if (SOCKET_ERROR == setsockopt(serverSockDesc , SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
        return INVALID_SOCKET;


    // Surisame sukurta deskriptoriu su turimu adresu.
    // grazina 0 jeigu pavyko, -1 jeigu įvyko klaida
    if (SOCKET_ERROR == bind (serverSockDesc, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
        closesocket(serverSockDesc);
        return INVALID_SOCKET;
    }

    // Liepiame sukurtam soketui laukti prisijungimu per porta,
    // su kuriuo jis suristas.
    // listen sekmes atveju grazina 0
    // klaidos atveju -1
    if (SOCKET_ERROR == listen(serverSockDesc, MAX_QUEUE_LENGTH)){
        closesocket(serverSockDesc);
        return INVALID_SOCKET;
    }

    // Isvedame informacija apie sekminga serverio inicializacija.
    printf ("File server: started successfully on host \'%s\' - (%s).\n", cServerHostName, inet_ntoa(*(struct in_addr*)ptrServerHostEntry->h_addr));

    // Graziname sukurto soketo deskriptoriu.
    return serverSockDesc;
}


// Nauju prisijungimu apdorojimas.
int handleNewConnection (SOCKET *servSockDesc, unsigned int *maxDesc, fd_set *mainSocketSet) {
	
    // Struktura bandancio prisijungti adresui.
	struct sockaddr_in remoteAddress;
     
	// Soketo deskriptorius bendravimui su nauju klientu.
	SOCKET newConnectionDesc;		

    // Adreso strukturos dydziui saugoti.
	int addressSize;					

    // Struktura informacijai apie nauja klienta.
	struct hostent *hostEntry;			

    /*pasveikinimo buferis*/
    char greeting[] = "You entered our server, greetings!";

#ifdef WIN32OS
	const char yes = '1';
#else
	const int yes = 1;
#endif

	//Inicializuojame adreso strukturos dydzio kintamaji.
	addressSize = sizeof (struct sockaddr_in);


	// Bandome priimti nauja prisijungima ir sukurti deskriptoriu.
	// bendravimui su juo.
    // accept sukuria nauja sujungta socketa , ir grazina naujo failo
    // deskriptoriu susieta su tuo soketu
	if (SOCKET_ERROR == (newConnectionDesc =
		accept ((*servSockDesc), (struct sockaddr *)&remoteAddress, &addressSize))) {
		return SOCKET_ERROR;
    }


	// Uzdedame opcija soketo adreso ir porto pakartotinam panaudojimui.
	if (SOCKET_ERROR == setsockopt (newConnectionDesc,
		SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes))) {
		return SOCKET_ERROR;
    }


	// Paskelbiam apie prisijungima serveryje.
	printf ("Server: new connection from \'%s\' accepted successfully.\n",
		inet_ntoa (remoteAddress.sin_addr));

	// Gauname informacija apie prisijungusi klienta.
    if (NULL == (hostEntry = gethostbyaddr ((void*)&(remoteAddress.sin_addr),
        sizeof (remoteAddress.sin_addr), AF_INET))) {

		closesocket (newConnectionDesc);
		printf ("Server: new connection from \'%s\' was immediately closed because of gethostbyaddr() failure.\n",
                inet_ntoa (remoteAddress.sin_addr));
		return SOCKET_ERROR;
	}


    //siunciame pasveikinima
    if (SOCKET_ERROR == sendData(&newConnectionDesc, greeting) ){
        closesocket (newConnectionDesc);
        printf ("Server: new connection from \'%s\' was immediately closed because of sendData() failure.\n", 
                inet_ntoa (remoteAddress.sin_addr));
        return SOCKET_ERROR;
    }

    FD_SET(newConnectionDesc, mainSocketSet);

    if ((*maxDesc) < newConnectionDesc) {
        (*maxDesc) = newConnectionDesc;
    }

	return 1;
}

int isQuit(char* command);
int isIsFile(char* userInput, char* paramsBuffer);


// Kliento siunciamu duomenu apdorojimas.
void handleDataFromClient (SOCKET clientSockDesc, fd_set *mainSocketSet){

	char *clientDataBuffer;	        // Buferis kliento pasiustai informacijai saugoti.
	int bytesReceived;				// Duomenu gavimo funkcijos grazinamam rez. saugoti.
	int packetsQuantity = 0;		// Duomenu gavimo operacija gautu paketu skaicius.
	int iCounter;					// Skaitliukas.
	char command [2000] = {0};		// Komandai saugoti.


	// Bandome gauti i isskirta buferi kliento pasiusta informacija.
	bytesReceived = receiveData(&clientSockDesc, &clientDataBuffer);
    
	// Atliekame veiksmus pagal duomenu gavimo funkcijos pranesta rezultata.
	// jei gautas pranesimas, kad vartotojas nutrauke rysi su serveriu.
	if ( bytesReceived == 0 )
	{
		// Isvedame pranesima apie prarasta rysi.
		printf ("Server: client at socket %d has quit the connection.\n", clientSockDesc);

		// Naikiname soketa.
		closesocket (clientSockDesc);

		// Pasaliname deskriptoriu is pagrindines aibes.
		FD_CLR (clientSockDesc, mainSocketSet);
	}

	// Priesingu atveju, jei ivyko klaida skaitant duomenis.
	else if ( bytesReceived == SOCKET_ERROR 
            || clientDataBuffer == NULL)
	{
		// Isvedame pranesima apie klaida.
		printf ("Server error: data reception from client was erroneous at socket %d.\n", clientSockDesc);

		// Naikiname soketa.
        closesocket (clientSockDesc);

		// Pasaliname is deskriptoriu aibes.
		FD_CLR (clientSockDesc, mainSocketSet);
	}

	// Priesingu atveju, jei viskas gerai, tai apdorojame
	// kliento uzklausas.
	else if ( bytesReceived >=  1 ) {
            
            // komanda quit
            if ( isQuit(clientDataBuffer) == 1 ) {
                closesocket(clientSockDesc);
            }
            else {

                int clientDataBufferSize = strlen(clientDataBuffer) + 1;
                char *params = (char*) malloc(clientDataBufferSize);

                // susizinome ar sutampa su isfile
                // komanda
                int outcome = isIsFile(clientDataBuffer, params); 

                // jeigu sutampa su isFile komandos
                // formatu
                if ( outcome == 1 ) {

                    //patikriname ar failas yra serveryje
                    int fileExist = isFileDescription(params);
                    
                    // serverio atsakymo masyvas
                    char exist[2];
                    
                    if ( fileExist == 1) {

                        exist[0] = '1';
                    }else {

                        exist[0] = '0';
                    }
                    exist[1] = '\0';
                    
                    // issiunciame atsakyma klientui
                    int responceSendingSucces = sendData(&clientSockDesc,  exist);
                    if ( responceSendingSucces == SOCKET_ERROR ) {
                        printf("Server error. Failed to responde to client with socket descriptor %d.\n", clientSockDesc);
                    }

                }else if ( outcome == -1 ) {
                    
                    printf("Client with socket descriptor %d error. Invalid isfile command format.\n", clientSockDesc);
                }
                free(params);
            }
			
            // Atlaisviname atminti.
            free (clientDataBuffer); 
	}
}



int isQuit(char* userInput) {

    // jeigu eilutes sutampa
    // graziname veliavele
    // kuri pranesa jog komanda buvo atpazinta
    if ( strcmp (userInput, "quit") == 0) {
        return 1;
    }
    
    return 0;
}

int isIsFile(char* userInput, char* paramsBuffer) {

    char command[] = {"isfile"};          // komanda kuria norime patikrinti
    int commandLen = strlen(command);     // komandos ilgis
    int userInputLen = strlen(userInput); // varotojo ivestu duomenu iligs
    
    // atsiustos komandos ilgs turi buti
    // tikrai ne trumpenis nei isfile eilutes ilgis
    if (userInputLen >= commandLen) {
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

                    /*nuimame comandos pavadinima*/
                    /*ir paliekame tik parametrus*/
                    int i;
                    for (i = commandLen + 1; i < userInputLen 
                            && userInput[i] != ' '; i++) {
                        paramsBuffer[i - (commandLen + 1)] = userInput[i];
                    }

                     /*jeigu paskutinis simbolis nebuvo tarpas reikia*/
                     /*pastumeti simboliu skaitliuka, kad po visu*/
                     /*simboliu padetume eil. pab. simb.*/
                    paramsBuffer[i] = '\0';

                    return 1;

            }else {

                // neteisingas komandos formatas
                // pranesame apie klaida
                return -1;
            }
        }
    }
    return 0;
}

int addFileDescription(char* fileDescription) {
    
    if ( fileDescription == NULL ) {
        return 0;
    }
    
    if ( strlen(fileDescription) + 1 > 100 ) {
        printf("Server error: file description too large\n");
        return 0;
    }
    
    int i;
    for (i = 0; i < strlen(fileDescription); i++) {
        fileDescriptions[fileCount][i] = fileDescription[i]; 
    }
    fileDescriptions[fileCount][i] = '\0';
    
    fileCount++;

    return 1;
}

int isFileDescription(char* fileDescription) {

    if ( fileDescription == NULL ) {
        return 0;
    }
    
    if ( strlen(fileDescription) + 1 > 100 ) {
        printf("Server error: file description too large\n");
        return 0;
    }
    
    int i, j;
    int existingDescLen;
    int descToCheckLen = strlen(fileDescription);

    for (j = 0; j < fileCount; j++) {

        existingDescLen = strlen(fileDescriptions[j]);
        // jeigu ilgiai nesutampa tai tuo
        // paciu nesutaps ir eilutes
        if ( existingDescLen != descToCheckLen ) {
            continue;
        }

        int descriptionsMatch = 1;
        for (i = 0; i < existingDescLen; i++) {
           // jeigu faila apibudiancios eilutes
           // nesutampa reiskias
           // tokio failo serveryje nera  
           if ( fileDescriptions[j][i] != fileDescription[i] ) {

               descriptionsMatch = 0;
           }
        }
        if ( descriptionsMatch == 1 ) {
            return 1;
        }
    }
    return 0;
}

    /*struct sockaddr_in{*/
      /*short sin_family        ; Address family  in byte 1 ; must be AF_INET. */
      /*unsigned short sin_port ; Internet Protocol (IP) port. 16 bit value found in byte 2 and 3 */
      /*IN_ADDR sin_addr        ; IP address in network byte order. 32 bit adress store in bytes 4-7*/
      /*char sin_zero[8]        ; Padding to make structure the same size as SOCKADDR. */
      /*The IP address part of this structure is of type in_addr.*/
    /*};*/

    /*struct hostent {*/
       /*char    *h_name;            [> official name of host                <]*/
       /*char    **h_aliases;        [> alias list                           <]*/
       /*int     h_addrtype;         [> host address type                    <]*/
       /*int     h_length;           [> length of address                    <]*/
       /*char    **h_addr_list;      [> list of addresses from name server   <]*/
     /*#define h_addr h_addr_list[0] [> address, for backward compatiblity   <]*/
    /*};*/

    /* bind(soketo deskriptorius                   , */
    /* rodykle i sockaddr_in arba sockaddr ,         */
    /* adreso ilgis baitais)                         */

    /*
     * setsockopt(soketo deskriptorius                ,
     * lygis kuriam priskiriamas parametras           ,
     * socketo parametras                             ,
     * nuoroda i buferi kur saugoma parametro reiksme ,
     * buferio i kuri rodo nuoroda dydis) 
     */

