#include <netinet/in.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#define MAX 100

void communication(int sock_desc);

void preparation(){
    int p = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in s;
    struct hostent *h;

    s.sin_family = AF_INET;
    s.sin_port = htons(5000);
    s.sin_addr.s_addr = inet_addr("127.0.0.1");

    int c = connect(p,(struct sockaddr*) &s, sizeof(s));
    if(c == -1){
        perror("erreur connection au serveur \n");
    }else{
        printf("connexion établie ...");
        communication(p);
    }
    
    close(p);
}

void communication(int sock_desc) {
    char buff[MAX];

    //"1;Valence;Grenoble;7:15"
    char msg[MAX];
    int result_len;
    int exit = 0;

    while (exit != 1)
    {
        // vide le contenu du buffer
        bzero(buff, MAX);


        printf("\n=> ");
        //écrit au serveur
        fgets(msg, 100, stdin);

        // envoi une requête au serveur
        int ecrit = write(sock_desc, msg, strlen(msg));
        printf("\noctets écrits: %d\n", ecrit);

        //si le message est exit, la communication s'arrête et le socket se ferme
         if (strncmp("exit", msg, 4) == 0) {
             printf("Deconnexion...\n");
             exit = 1;
         } else {
            // attend la taille du resultat
            read(sock_desc, &result_len, sizeof(int));
            printf("\noctets à recevoir: %d\n", result_len);

            // attend le resultat
            read(sock_desc, buff, result_len);
            printf("\nserveur: %s\n", buff);
         }
    }
    

}



int main(){
    preparation();
    return 0;
}

