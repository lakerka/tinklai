#include	"client.h"

int main (void)
{
	SOCKET ClientSockDesc;	// Kliento pagrindinio soketo-klausytojo deskriptorius.
	char UserInput [2000];	// Masyvas vartotojo komandoms nuskaityti.
	int SendResult;			// Siuntimo funkcijos resultatui saugoti.
	char *Packets [10];		// Buferiu masyvas duomenims gauti.
	int Quant = 0;			// Skaitliukas gautiems paketams skaiciuoti.
	int iCounter, jCounter;	// Skaitliukai.
	unsigned int ParseResult;// Komandu analizes rezultatui saugoti.
	fd_set ReadMask;		// Reikalinga selectui.
	struct timeval TimeVal;	// reikalinga selectui.
	int Flag;				// Veliavele.


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
	if ( INVALID_SOCKET == (ClientSockDesc = initializeClient() ) )
	{
		printf ("Client error: client initialization failed.\n");
		goto EXIT;
	}

	// Amzinas ciklas, kurio pagalba vartotojas yra rezime 'gyvas'.
	while ( 1 )
	{
        break;
		// Isvalome standartinio I/O buferi.
		fflush ( stdin );

		// Isvalome UserInput
		memset (UserInput, 0, sizeof (UserInput));

		// Uzklausiame vartotojo jo pasirinkimo.
		fgets (UserInput, sizeof (UserInput), stdin);
		UserInput [strlen (UserInput) - 1] = '\0';

		if ( 0 == strcmp (UserInput, "") ) continue;



		// Siunciame turima paketa serveriui.
		/*if ( SOCKET_ERROR == (SendResult = SendAllData (&ClientSockDesc,*/
			/*UserInput, strlen (UserInput))) )*/
		/*{*/
			/*printf ("MathClient error: data transmission to the server failed.\n");*/
			/*break;*/
		/*}*/

		// Gauname rezultata.
		/*if ( SOCKET_ERROR == ReceiveAllData (&ClientSockDesc, Packets, &Quant) )*/
		/*{*/
			/*printf ("MathClient error: data reception from server failed.\n");*/
			/*break;*/
		/*}*/


	// Uzdarome soketa ir tokiu budu pranesame serveriui apie nutraukta rysi.
    }
	closesocket (ClientSockDesc);

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

