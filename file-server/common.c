#include "common.h"

    //paketo formatas: 
    //pirmas baitas nurodo kokio dydzio 
    //naudinga informacija baitais(iki 255)
    //like baitai: naudinga informacija


// eilute nukopijuojanti funkcija
char* copyString(const char *string) {
    char* stringCopy;
    stringCopy = (char*) malloc(sizeof(string));
    strcpy(stringCopy, string);
    return stringCopy;
}

// duomenis issiuncianti nurodytam soketui funkcija
int sendData ( SOCKET* socket, const char* data) {
    char* dataCopy = copyString(data);  
    int bytesSent = 0;
    if ( marshalPacket(dataCopy) == SOCKET_ERROR ) {
        return SOCKET_ERROR;
    }
    bytesSent = sendPacket(socket, dataCopy);
    return bytesSent;
}

// duomenis nurodytu soketu gaunanti funkcija
int receiveData ( SOCKET* socket, char** data) {
    if (receivePacket(socket, data) == SOCKET_ERROR) {
        return SOCKET_ERROR;
    }
    return unmarshalPacket((*data));
}

// baita i sveika teigiama skaiciu pavercianti funkcija
int byteToInteger(char byte) {
    int twoByPower = 1;
    int result = 0;
    int i;
    for (i = 0; i < 8; i++) {
        if ((byte & 1) == 1) {
            result += twoByPower;
        }
        byte >>= 1;    
        twoByPower *= 2;
    }
    return result;
}

// sveika teigiama skaiciu i baita pavercianti funkcija
// grazina 1 jeigu sekmingai paversta, 
// grazina 0 jeigu ivyko klaida 
int integerToByte(int integer, char *byte) {
    if (integer < 0 || integer > 255) {
        return 0;
    }
    int twoByPower = 1;
    char result = 0;
    int i;
    for ( i = 0; i < 8; i++) {
        if ((integer & 1) == 1) {
            result += twoByPower;
        }
        integer >>= 1;    
        twoByPower *= 2;
    }
    (*byte) = result;
    return 1;
}

// Paketo supakavimas
int marshalPacket (char *packet) {
    int dataSize = strlen(packet);  // Siunciamu duomenu dydis - simboliu kiekis.
    char marshaledData [500] = {0}; // Buferis, skirtas saugoti duomenu paketui.
    char dataSizeByte;              // baitas apibudinantis naudingo info kieki
    int iCounter;                   // Skaitliukas.
    
    int conversionSucces = integerToByte(dataSize, &dataSizeByte);

    // pavertimas galejo nepavykti jeigu naudingos
    // informacijos kiekis didesnis negu
    // 255 baitai
    if ( conversionSucces == 0) {
        return SOCKET_ERROR;
    }
    // naudingu duomenu dydi zymi pirmas paketo baitas
    marshaledData[0] = dataSizeByte;

    //nukopijuojame naudingus duomenis
    for ( iCounter = 0; iCounter < dataSize; iCounter++ ) {
        marshaledData [iCounter + 1] = packet[iCounter];
    }

    //pridedamas duomenu pabaigos simbolis
    marshaledData [iCounter + 1] = '\0';

    //nukompijuojame suformuota paketa
    strcpy (packet, marshaledData);

    return 1;
}

// Paketo ispakavimas.
int unmarshalPacket (char *packet) {
    char unmarshaledData [500] = {0}; // Buferis, skirtas saugoti realius duomenis isskirtus is paketo.
    int iCounter = 0;                 // Skaitliukas.
    int usefullDataSize = 0;          // naudingo informacijos kiekis baitais
    int dataSize = 0;                 // Persiustu duomenu dydis, kuris bus grazintas.
    
    // nuskaitom duomenu dydi apibudinancia eilute
    dataSize = byteToInteger(packet[0]);

    // jeigu paketo dydis nesutampa su nurodytu
    // reiskias gavome formato neatitinkanti paketa
    // galime ji ismesti
    if (strlen(packet) - 1 != dataSize) {
        return SOCKET_ERROR;
    }

    // kopijuojame naudingus duomenis
    for (iCounter = 1; iCounter < strlen(packet); iCounter++) {
        unmarshaledData[iCounter - 1] = packet[iCounter];
    }
    // pridedame eilutes pabaigos simboli
    unmarshaledData[iCounter - 1] = '\0';

    strcpy (packet, unmarshaledData);
    
    return dataSize;
}

//paketo siuntimas
int sendPacket ( SOCKET* socket, const char* packet) {
    int sentBytes = 0;              // Jau issiustu baitu skaicius.

    int bytesLeft = strlen(packet); // Kiek baitu dar liko issiusti.
    int currSentBytesCount;         // Per viena karta issiunciamu baitu sk.

    /*printf("Common.c: %s\n", packet);*/
    while( bytesLeft > 0 ) {

        /*int send (<The socket descriptor to use to send the data> ,
                     <A pointer to the buffer to send> ,
                     <The length of the buffer to send> ,
                     <flags>)*/
        currSentBytesCount = send ((*socket), packet + sentBytes, bytesLeft, 0);

        /*printf("Common.c: %d\n", bytesLeft);*/
        if ( currSentBytesCount == SOCKET_ERROR ) {
            return SOCKET_ERROR;
        }
        sentBytes += currSentBytesCount;
        bytesLeft -= currSentBytesCount;
    } 

    return sentBytes;
}

// informacija gaunama po viena paketa
// klientas vienu metu gali siusti viena paketa
int receivePacket ( SOCKET* socket, char** packet) {
     
    int bytesReceived = 0;        // Jau gautu baitu skaicius.
    int packetSize = 0;           // paketo dydis

    int currRecvByteCount;        // Per viena karta gautu baitu sk.
    char dataBuffer [2000] = {0}; // Duomenu buferis
    
    // testi, padaryti normalu paketo gavima
    currRecvByteCount = recv ((*socket), dataBuffer, sizeof (dataBuffer), 0);

    bytesReceived += currRecvByteCount;
    
    //nieko negavome, reiskias vartotojas nutrauke rysi
    if (currRecvByteCount == 0) {
        return 0;
    
    // jeigu ivyko klaida pranesame apie tai
    }else if (currRecvByteCount == SOCKET_ERROR) {
        return SOCKET_ERROR;
        
    // gali buti kad liko nepriimtu duomenu, turime juos priimti
    }else {

        //pirmame baite yra nurodytas
        //naudingos informacijos dydis
        //ji ir pasiimame
        packetSize = byteToInteger(dataBuffer[0]); 

        while( 1 ) { 
			// Jei turime maziau baitu nei nurodyta paketo dydyje.
            if ( bytesReceived - 1 < packetSize ){
                // Perskaitome dar.
                // Kreipiames i recv(), noredami 
                // gauti kazkokia tai dali informacijos.
                currRecvByteCount = recv ((*socket), dataBuffer + bytesReceived, sizeof (dataBuffer), 0);

                // Modifikuojame jau perskaitytu baitu skaitliuka.
                bytesReceived += currRecvByteCount;

                // Jei siuo metu gautu baitu sk == 0, tai reiskia, kad vartotojas nutrauke rysi su serveriu.
                if ( 0 == currRecvByteCount ) {
                    return 0;
                // Priesingu atveju, pranesame, kad skaitant ivyko klaida.
                }else if ( currRecvByteCount == SOCKET_ERROR){
                    return SOCKET_ERROR;
                }else { 
                    continue;
                }
            }
            //gauta tiek baitu kiek tiketasi galime,
            //iskirti naudinga informacija ir baigti nuskaityma
            // jeigu meginama siusti daugiau negu viena paketa like baitai ignoruojami
            else if (bytesReceived - 1 >= packetSize) {
                
                dataBuffer[packetSize + 1] = '\0';
                // nukopijuojame gauta paketa
                (*packet) = (char*) malloc( packetSize + 1);
                memset ((*packet), 0, packetSize + 2);
                strcpy ((*packet), dataBuffer);
				break;
            }

        }

    }
    
    return 1;

}
/*send() is used to transmit a message to another transport end-point. send() may be used only when the socket is in a connected state. socketDescriptor is a socket created with socket(). If the message is too long to pass atomically through the underlying protocol (non-TCP protocol), then the error TM_EMSGSIZE is returned and the message is not transmitted. A return value of TM_SOCKET_ERROR indicates locally detected errors only. A positive return value does not implicitly mean the message was delivered, but rather that it was sent.*/
