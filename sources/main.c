#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

#include "../headers/dependencies.h"
#include "../headers/socket.h"

int port;
char *bd;

// conserve le contenu du fichier
FILE *file;

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Oups ! Deux arguments sont attendus\n");
        printf("arg 1: le port d'écoute\narg 2: le chemin du fichier contenant les informations sur les trains\n");
        exit(1);
    } else if(argc < 3)
    {
        char arg1[strlen(argv[1])];
        strcpy(arg1, argv[1]);
        port = is_correct_port(arg1);

        if (port == -1)
        {
            printf("Oups ! le port entré est incorrect\n");
            exit(1);
        } else {
            printf("Oups ! 1 argument est attendu\n");
            printf("arg 2: le chemin du fichier contenant les informations sur les trains\n");
            exit(1);
        }
    }

    file = fopen(argv[2], "r");
    if (file == NULL)
    {
        printf("Oups ! fichier inexistant\n");
        exit(1);
    }
    
    port = atoi(argv[1]);
    bd = calloc(strlen(argv[2]), sizeof(char));
    strcpy(bd, argv[2]);

    struct sockaddr_in my_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;

    /*interception de la mort d'un fils */
    struct sigaction act;
    act.sa_handler = handler_child;
    act.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &act, NULL);

    /*interception de l'interruption du programme ctrl+c*/
    signal(SIGINT, handler_interruption);

    /* Création de la socket et écoute */
    int server_desc = ecouter(my_addr);

    /* Acceptation d'une connexion */
    accepter(client_addr, server_desc);

    // fermer le socket
    close(server_desc);

    free(bd);

    return 0;
}