#include "server.h"

int main(void) {
    /*char paketas[] = "X";*/
    /*paketas[1] = 'O';*/
    /*printf("%s\n", paketas);*/
    /*marshalPacket(paketas);*/
    /*printf("%s\n", paketas);*/

    /*unmarshalPacket(paketas);*/
    /*printf("%s\n", paketas);*/
    /*return 0;*/
    /*printf("%lu", sizeof(char));*/
    /*printf("%lu", sizeof(int));*/
    /*char eil[] = {"15:mano"};*/
    /*UnmarshalPacket(eil);*/
    SOCKET servSockDesc;
    fd_set mainSocketSet, tempSet;
    unsigned int maxKnowSocketDesc, iCounter;
    struct timeval TimeVal; // Laiko struktura dirbti su select().

#ifdef WIN32OS
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1, 1), &wsaData);
#endif

 
    // Isvalo soketu aibes, kad jos neturetu jokiu siuksliu.
    FD_ZERO(&mainSocketSet);
    FD_ZERO(&tempSet);

    // Inicializuojame laiko struktura. 0 - reiskia, kad select() funkcija turi blokuotis laukdama, kol atsiras bent vienas aktyvus soketas.
    TimeVal.tv_sec = 0;
    TimeVal.tv_usec = 0;

    // Inicializuokime serverio funkcini moduli.
    if (INVALID_SOCKET == (servSockDesc = initializeServer())) {
        exit(EXIT_FAILURE);
    }

    // Itraukti serverio klausanti soketa i pagrindine aibe 
    // ir pazymeti klausantiji soketa kaip maksimalu 
    // programai zinoma soketo deskriptoriu.

    FD_SET(servSockDesc, &mainSocketSet); // ideda socketo deskript i aibe
    maxKnowSocketDesc = servSockDesc;
    // Pagrindinis amzinas ciklas, kuris palaiko serveri rezimu "gyvas".

    while (1){

        // Kiekvienoje iteracijoje inicializuokime pagalbine soketu aibe pagrindine.
        tempSet = mainSocketSet;
       
        if (SOCKET_ERROR == select (maxKnowSocketDesc + 1, &tempSet, NULL, NULL, &TimeVal)) {
            exit(EXIT_FAILURE);
        }

        /*Musu sukurto serverio soketas klausosi. 
         * Jeigu kas nors nori i ji rasyti tada, 
         * mums reikia priimti nauja prisijungima. 
         * Jeigu neregistruojamas serverio soketo 
         * aktyvumas tada reiskias aktyvuojasi esamo 
         * kliento soketas(klientas siuncia duomenis).*/
        
        // Nuskanuoti galimu deskriptoriu erdve iki turimo maksimalaus.
        for (iCounter = 0; iCounter <= maxKnowSocketDesc; iCounter++) {

            // Tikriname su kiekviena skaitliuko reiksme, 
            // ar jo aprasomas soketas nepriklauso pagrindinei soketu aibei.
            if (FD_ISSET (iCounter, &tempSet)) {
                // Jei tos skaitliuko reiksmes aprasomas 
                // soketas yra 'servSockDesc', t.y. nauju 
                // prisijungimu laukiantis, tai reikia apdoroti 
                // naujo kliento prisijungima.

                if (iCounter == servSockDesc) {
                    // Apdorojame nauja prisijungima.

                    if (SOCKET_ERROR == handleNewConnection (&servSockDesc, &maxKnowSocketDesc, &mainSocketSet)) {
                        printf ("Server error: acceptance of new connection was erroneous.\n");
                    }

                    printf("\nSiunciame pasveikima naujai prisijungusiam varotojui.\n");
                    char greeting[] = "Sveiki prisijunge prie musu serverio!";
                    sendData(&maxKnowSocketDesc, greeting);
                }
                else {
                    // Jei tai ne nauju prisijungimu laukiancio soketo 
                    // deskriptorius, tai reiskia, kad gauta informacija 
                    // is kazkurio prisijungusio kliento.
                    // Reikia ja priimti ir atitinkamai apdoroti.
                    //
                    // Priimame ir apdorojame pranesima.
                    /*HandleDataFromClient ((SOCKET)iCounter, &mainSocketSet);*/
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

