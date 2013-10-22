#ifndef		unix				// Jeigu tai ne unix tipo OS:

#define		WIN32OS				// 1. Apibreziame varda 'WIN32OS'.
#include	<windows.h>			// 2. Itraukiame pagrindini winapi '.h' faila.
#include	<winsock.h>			// 3. Itraukiame darbui su soketu api skirta '.h' faila.
#include	<time.h>			// 4. Itraukiame darn\bui su laiku skirta '.h' faila.

#else							// Priesingu atveju, jei tai unix tipo OS:

#include	<sys/types.h>		// Itraukiame keleta '.h' failu, skirtu
#include	<sys/socket.h>		// tipu apibrezimui, darbui su soketais,
#include	<netinet/in.h>		// internetiniu strukturu, konstantu bei
#include	<arpa/inet.h>		// funkciju apibrezimams palaikyti.
#include	<sys/time.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<netdb.h>

#define		SOCKET int			// Apibreziame sinonima 'int' tipui, kad butu
								// unix ir WIN sistemoms bendras kodas dirbant
								// su soketu deskriptoriais.
#define		closesocket	close	// Apibreziame sinonima close () funkcijai
								// closesocket (), kuris yra analogas Windows
								// tipo sistemose. Tai del kodo suvienodinimo.
#define		INVALID_SOCKET -1	// Apibreziame del kodo suvienodinimo, klaidos
#define		SOCKET_ERROR -1		// situacijai nusakyti konstantas, kurios WINSOCK'o
								// yra jau apibreztos.
#endif


// Nurodome pre-kompiliatoriui, kokius '.h' failus reikia itraukti,
// nepriklausomai nuo operacines sistemos, kurioje kodas kompiliuojamas.
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

// Funkciju, skirtu siunciamu duomenu paketu
// sudejimui ir isskaidymui, antrastes.
void MarshalPacket ( char* Packet );
int UnmarshalPacket ( char* Packet );

// Funkciju, skirtu duomenu paketams issiusti
// ir gauti, antrastes.
int SendPacket ( SOCKET* s, const char* Packet, int MaxBufferSize);
int ReceivePacket ( SOCKET* s, char* Packet);

