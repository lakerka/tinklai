#include "server.h"
/*#include "common.h"*/
int main(void) {

    /*char eil[] = {"15:mano"};*/
    /*UnmarshalPacket(eil);*/
    SOCKET ServSockDesc;
    fd_set MainSocketSet, TempSet;
    unsigned int MaxKnownSocketDesc, iCounter;
    struct timeval TimeVal; // Laiko struktura dirbti su select().

#ifdef WIN32OS
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1, 1), &wsaData);
#endif

 
    // Isvalo soketu aibes, kad jos neturetu jokiu siuksliu.
    FD_ZERO(&MainSocketSet);
    FD_ZERO(&TempSet);

    // Inicializuojame laiko struktura. 0 - reiskia, kad select() funkcija turi blokuotis laukdama, kol atsiras bent vienas aktyvus soketas.
    TimeVal.tv_sec = 0;
    TimeVal.tv_usec = 0;

    // Inicializuokime serverio funkcini moduli.
    if (INVALID_SOCKET == (ServSockDesc = InitializeServer())) {
        exit(EXIT_FAILURE);
    }

    // Itraukti serverio klausanti soketa i pagrindine aibe 
    // ir pazymeti klausantiji soketa kaip maksimalu 
    // programai zinoma soketo deskriptoriu.

    FD_SET(ServSockDesc, &MainSocketSet); // ideda socketo deskript i aibe
    MaxKnownSocketDesc = ServSockDesc;
    // Pagrindinis amzinas ciklas, kuris palaiko serveri rezimu "gyvas".

    while (1){

        // Kiekvienoje iteracijoje inicializuokime pagalbine soketu aibe pagrindine.
        TempSet = MainSocketSet;
       
        if (SOCKET_ERROR == select (MaxKnownSocketDesc + 1, &TempSet, NULL, NULL, &TimeVal)) {
            exit(EXIT_FAILURE);
        }

        /*Musu sukurto serverio soketas klausosi. 
         * Jeigu kas nors nori i ji rasyti tada, 
         * mums reikia priimti nauja prisijungima. 
         * Jeigu neregistruojamas serverio soketo 
         * aktyvumas tada reiskias aktyvuojasi esamo 
         * kliento soketas(klientas siuncia duomenis).*/
        
        // Nuskanuoti galimu deskriptoriu erdve iki turimo maksimalaus.
        for (iCounter = 0; iCounter <= MaxKnownSocketDesc; iCounter++) {

            // Tikriname su kiekviena skaitliuko reiksme, 
            // ar jo aprasomas soketas nepriklauso pagrindinei soketu aibei.
            if (FD_ISSET (iCounter, &TempSet)) {
                // Jei tos skaitliuko reiksmes aprasomas 
                // soketas yra 'ServSockDesc', t.y. nauju 
                // prisijungimu laukiantis, tai reikia apdoroti 
                // naujo kliento prisijungima.

                if (iCounter == ServSockDesc) {
                    // Apdorojame nauja prisijungima.

                    if (SOCKET_ERROR == HandleNewConnection (&ServSockDesc, &MaxKnownSocketDesc, &MainSocketSet)) {
                        printf ("Server error: acceptance of new connection was erroneous.\n");
                    }
                }
                else {
                    // Jei tai ne nauju prisijungimu laukiancio soketo 
                    // deskriptorius, tai reiskia, kad gauta informacija 
                    // is kazkurio prisijungusio kliento.
                    // Reikia ja priimti ir atitinkamai apdoroti.
                    //
                    // Priimame ir apdorojame pranesima.
                    /*HandleDataFromClient ((SOCKET)iCounter, &MainSocketSet);*/
                }
            }
        }
    }

#ifdef WIN32OS

    WSACleanup();

#endif

        return 1;

}




/*select(<SocketCount + 1>, <read>, <write>, <exception>, <block time>)*/

/*int FD_ISSET(int fd <file descriptor>, <file descriptor set> fdset* ) ar deskriptorius su numeriu fd priklauso fdset*/

