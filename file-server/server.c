#include "server.h"

SOCKET initializeServer (void) {

    SOCKET serverSockDesc;                     //  Serverio soketo deskriptorius.
    char cServerHostName [256] = {0};          //  Hosto, kuriame serveris paleistas, vardas.
    struct sockaddr_in serverAddress;          //  Serverio adreso struktura.
    /*struct sockaddr_in{*/
      /*short sin_family        ; Address family  in byte 1 ; must be AF_INET. */
      /*unsigned short sin_port ; Internet Protocol (IP) port. 16 bit value found in byte 2 and 3 */
      /*IN_ADDR sin_addr        ; IP address in network byte order. 32 bit adress store in bytes 4-7*/
      /*char sin_zero[8]        ; Padding to make structure the same size as SOCKADDR. */
      /*The IP address part of this structure is of type in_addr.*/
    /*};*/

    struct hostent *ptrServerHostEntry = NULL; //  Serverio hosto informacine struktura. (hostent)
    /*struct hostent {*/
       /*char    *h_name;            [> official name of host                <]*/
       /*char    **h_aliases;        [> alias list                           <]*/
       /*int     h_addrtype;         [> host address type                    <]*/
       /*int     h_length;           [> length of address                    <]*/
       /*char    **h_addr_list;      [> list of addresses from name server   <]*/
     /*#define h_addr h_addr_list[0] [> address, for backward compatiblity   <]*/
    /*};*/

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
    serverAddress.sin_port = htons(SERVER_PORT); // priskiriamas portas
    serverAddress.sin_addr = *(struct in_addr*)ptrServerHostEntry->h_addr; // priskiriamas IP adresas
    memset(&(serverAddress.sin_zero), 0, 8); 

    // Sukuriame pati soketo deskriptoriu.
    // socket(address family, type, protocol to be used)
    if (INVALID_SOCKET == (serverSockDesc = socket (AF_INET, SOCK_STREAM, 0))) {
        return INVALID_SOCKET;
    }
    /*
     * setsockopt(soketo deskriptorius                ,
     * lygis kuriam priskiriamas parametras           ,
     * socketo parametras                             ,
     * nuoroda i buferi kur saugoma parametro reiksme ,
     * buferio i kuri rodo nuoroda dydis) 
     */

    // Uzdedame opcija soketo adreso ir porto pakartotinam panaudojimui.
    if (SOCKET_ERROR == setsockopt(serverSockDesc , SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
        return INVALID_SOCKET;

    /* bind(soketo deskriptorius                   , */
    /* rodykle i sockaddr_in arba sockaddr ,         */
    /* adreso ilgis baitais)                         */

    // Surisame sukurta deskriptoriu su turimu adresu.
    if (SOCKET_ERROR == bind (serverSockDesc, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
        closesocket(serverSockDesc);
        return INVALID_SOCKET;
    }

    // Liepiame sukurtam soketui laukti prisijungimu per porta,
    // su kuriuo jis suristas.
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
	struct sockaddr_in remoteAddress;	// Struktura bandancio prisijungti adresui.
	SOCKET newConnectionDesc;			// Soketo deskriptorius bendravimui su nauju klientu.
	int addressSize;					// Adreso strukturos dydziui saugoti.
	char buffer [1000] = {0};			// Buferis pasveikinimo informacijai laikyti.
	struct hostent *HostEntry;			// Struktura informacijai apie nauja klienta.
#ifdef WIN32OS
	const char yes = '1';
#else
	const int yes = 1;
#endif


	//Inicializuojame adreso strukturos dydzio kintamaji.
	addressSize = sizeof (struct sockaddr_in);


	// Bandome priimti nauja prisijungima ir sukurti deskriptoriu.
	// bendravimui su juo.
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
	if (NULL == (HostEntry = gethostbyaddr ((void*)&(remoteAddress.sin_addr),
		sizeof (remoteAddress.sin_addr), AF_INET))) {
		closesocket (newConnectionDesc);
		printf ("Server: new connection from \'%s\' was immediately closed because of gethostbyaddr() failure.\n",
                inet_ntoa (remoteAddress.sin_addr));
		return SOCKET_ERROR;
	}

    /*Siunciame pasveikima naujai prisijungusiam varotojui.*/
    char greeting[] = "You entered our server, greetings!";

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

// Kliento siunciamu duomenu apdorojimas.
void handleDataFromClient (SOCKET clientSockDesc, fd_set *mainSocketSet){
	char *clientDataBuffer;	        // Buferis kliento pasiustai informacijai saugoti.
	int receiveResult;				// Duomenu gavimo funkcijos grazinamam rez. saugoti.
	int packetsQuantity = 0;		// Duomenu gavimo operacija gautu paketu skaicius.
	int iCounter;					// Skaitliukas.
	char command [2000] = {0};		// Komandai saugoti.


	// Bandome gauti i isskirta buferi kliento pasiusta informacija.
	receiveResult = receiveData(&clientSockDesc, &clientDataBuffer);
    printf("\nreceive result: %d\n", receiveResult);
	// Atliekame veiksmus pagal duomenu gavimo funkcijos pranesta rezultata.
	// jei gautas pranesimas, kad vartotojas nutrauke rysi su serveriu.
	if ( receiveResult == 0 )
	{
		// Isvedame pranesima apie prarasta rysi.
		printf ("Server: client at socket %d has quit the connection.\n", clientSockDesc);

		// Naikiname soketa.
		closesocket (clientSockDesc);

		// Pasaliname deskriptoriu is pagrindines aibes.
		FD_CLR (clientSockDesc, mainSocketSet);
	}

	// Priesingu atveju, jei ivyko klaida skaitant duomenis.
	else if ( receiveResult == SOCKET_ERROR )
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
	else if ( receiveResult == 1 )
	{

            if ( 0 == strcmp (command, "LEAV") ) {
                closesocket(clientSockDesc);
            }

			// Atlaisviname atminti.
			free (clientDataBuffer); 
	}
}

