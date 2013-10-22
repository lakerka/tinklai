#include "common.h"

// Paketo supakavimas
void MarshalPacket (char *Packet) {
    int DataSize = strlen(Packet);   // Siunciamu duomenu dydis - simboliu kiekis.
    char PacketSizeStr [10] = {0};   // Buferis, skirtas saugoti duomenu ilgi simboliniu formatu.
    char MarshaledData [2000] = {0}; // Buferis, skirtas saugoti duomenu paketui.
    int iCounter;                    // Skaitliukas.

    // issaugo i PacketSizeStr buferi DataSize simboliniu formatu
    sprintf(PacketSizeStr, "%d", DataSize);

    //iraso duomenu dydi simboliniu formatu i eilutes pradzia
    for (iCounter = 0; iCounter < strlen(PacketSizeStr); iCounter++) {
        MarshaledData [iCounter] = PacketSizeStr[iCounter];
    }
    //duomenu dydis nuo duomenu atskiriamas skirtuku
    MarshaledData [strlen(PacketSizeStr)] = ':';

    for ( iCounter = 0; iCounter < DataSize; iCounter++ ) {
        MarshaledData [iCounter + (strlen(PacketSizeStr) + 1)] = Packet [iCounter];
    }
    //pridedamas duomenu pabaigos simbolis
    MarshaledData [iCounter + (strlen(PacketSizeStr) + 1)] = '\0';
    strcpy (Packet, MarshaledData);
}


// Paketo ispakavimas.
int UnmarshalPacket (char *Packet) {
    char UnmarshaledData [2000] = {0}; // Buferis, skirtas saugoti realius duomenis isskirtus is paketo.
    int iCounter = 0;                  // Skaitliukas.
    char PacketSizeStr [10] = {0};     // Buferis, skirtas saugoti persiustu duomenu dydi simboliniu formatu.
    int DataSize = 0;                  // Persiustu duomenu dydis, kuris bus grazintas.
    
    // nuskaitom duomenu dydi apibudinancia eilute
    while ( Packet[iCounter] != ':' && iCounter < strlen(Packet) ) {
        PacketSizeStr[iCounter] = Packet[iCounter++];
    }
    // pridedam eilutes pabaigos simboli
    PacketSizeStr[iCounter] = '\0';
    
    //paketo dydi nusakancia eilute paverciam skaiciumi
    DataSize = atoi(PacketSizeStr);
    
    //pradedam nuskaityt duomenis
    for ( iCounter = strlen(PacketSizeStr) + 1; iCounter < strlen(Packet); iCounter++ )
        if ( Packet[iCounter] != '\r' && Packet[iCounter] != '\n' )
            UnmarshaledData [iCounter - (strlen(PacketSizeStr) + 1)] = Packet[iCounter];

    //baigus kopijuot duomenis pridedam eilutes pabaigos simboli
    UnmarshaledData [iCounter - (strlen(PacketSizeStr) + 1)] = '\0';

    strcpy (Packet, UnmarshaledData);
    
    return DataSize;
}

//paketo siuntimas
int SendPacket ( SOCKET* socket, const char* Packet, int MaxBufferSize) {
    int SentBytes = 0;             // Jau issiustu baitu skaicius.
    int BytesLeft = MaxBufferSize; // Kiek baitu dar liko issiusti.
    int nBytes;                    // Per viena karta issiunciamu baitu sk.
    
    while( SentBytes < MaxBufferSize ) {
        /*int send (<The socket descriptor to use to send the data> ,
                     <A pointer to the buffer to send> ,
                     <The length of the buffer to send> ,
                     <flags>)*/
        nBytes = send ((*socket), Packet + SentBytes, BytesLeft, 0);
        if ( nBytes == SOCKET_ERROR ) {
            return SOCKET_ERROR;
        }
        SentBytes += nBytes;
        BytesLeft -= nBytes;
    } 

    return SentBytes;
}

//paketo gavimas
int ReceivePacket ( SOCKET* socket, char* Packet) {
    int receivedBytes = 0;        // Jau gautu baitu skaicius.
    int nBytes;                   // Per viena karta gautu baitu sk.
    int iCounter = 0;
    char DataBuffer [2000] = {0}; // Duomenu buferis
    // testi, padaryti normalu paketo gavima
    while (receivedBytes < sizeof(DataBuffer)) {
        nBytes = recv ((*socket), DataBuffer, sizeof (DataBuffer), 0);
        receivedBytes += nBytes;
    }
    if ( 0 == nBytes ) {
        return 0;
    }else if ( SOCKET_ERROR == nBytes ) {
        return SOCKET_ERROR;
    }else {
        memset (Packet, 0, nBytes);
        strcpy (Packet, DataBuffer);
    }
    return 1;

}
/*send() is used to transmit a message to another transport end-point. send() may be used only when the socket is in a connected state. socketDescriptor is a socket created with socket(). If the message is too long to pass atomically through the underlying protocol (non-TCP protocol), then the error TM_EMSGSIZE is returned and the message is not transmitted. A return value of TM_SOCKET_ERROR indicates locally detected errors only. A positive return value does not implicitly mean the message was delivered, but rather that it was sent.*/
