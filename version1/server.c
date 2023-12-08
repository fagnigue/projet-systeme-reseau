#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <netdb.h>
#include <float.h>
#define PORT 5000
#define MAX 100


struct trouver_trains_params
{
    char *ville_depart;
    char *ville_arrivee;
    char *depart_interval_0;
    char *depart_interval_1;
    char *result;
};

struct trouver_trains_par_trajet
{
    char *ville_depart;
    char *ville_arrivee;
    char *result;
};


int ecouter(struct sockaddr_in p);
int accepter(struct sockaddr_in client_addr, int serv_descr);
void fils();
void handler_child(int sig);
void handler_interruption(int sig);
void communication(int sock_desc);
void operation(char *req, char* result);
void split_request(char *req, char **tab);
void trouver_train(char *ville_depart, char *ville_arrivee, char *heure_depart, char *result);
void trouver_trains(struct trouver_trains_params params);
void replace(char *s, char critere, char replace_caractere, char *new_s);
void trouver_train_par_trajet(struct trouver_trains_par_trajet params);

int socket_desc;
int child_group;

int main(int argc, char const *argv[])
{

    struct sockaddr_in my_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;

    /* comportement du père à la mort d'un fils */
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

    return 0;
}

int ecouter(struct sockaddr_in p)
{
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    p.sin_family = AF_INET;
    p.sin_addr.s_addr = htonl(INADDR_ANY);
    p.sin_port = htons(PORT);

    int bind_ = bind(socket_desc, (struct sockaddr *)&p, sizeof(struct sockaddr_in));

    if (bind_ != 0)
    {
        perror("erreur lors du bind au niveau serveur"),
            exit(0);
    }

    int listen_ = listen(socket_desc, 10);

    if (listen_ == -1){
        perror("erreur lors du listen au niveau serveur");
        exit(0);
    } else {
        printf("Serveur en marche ...\n");
    }

    return socket_desc;
}

int accepter(struct sockaddr_in client_addr, int serv_descr)
{
    socklen_t len = sizeof(client_addr);

    while (1)
    {
        int socket_desc = accept(serv_descr, (struct sockaddr *)&client_addr, &len);

        switch (fork())
        {
        case -1:
            perror("erreur lors de la création du fils");
            exit(1);
        case 0:
            fils(socket_desc);
        default:
            break;
        }
    }

    close(serv_descr);
}

void fils(int server_desc)
{
    child_group = getpgrp();
    printf("Connexion client n°%d\n", server_desc);
    communication(server_desc);
    printf("Deconnexion client n°%d\n", server_desc);
    close(server_desc);
    exit(0);
}

void handler_child(int sig)
{
    wait(NULL);
    printf("intercept\n");
}

void handler_interruption(int sig) {
    printf("\nInterruption %d!",child_group);
    killpg(child_group, SIGKILL);
    close(socket_desc);
    exit(1);
}

void communication(int sock_desc)
{
    char buff[MAX];
    char result_len[10];
    char *result;

    int exit = 0;

    while (exit != 1)
    {
        // vide le contenu du buffer
        bzero(buff, MAX);

        // lit le message envoyé par le client
        int lu = read(sock_desc, buff, MAX);
        if (lu == -1)
        {
            perror("\nechec de lecture");
        }
        else
        {
            printf("=> requête client n°%d: %s\n", sock_desc, buff);
        }

        //si le message est exit, la communication s'arrête et le socket se ferme
        if (strncmp("exit", buff, 4) == 0) {
            exit = 1;
        } else { // sinon écrit au client

            result = calloc(500, sizeof(char));

            // operation sur la requete envoyé
            operation(buff, result);

            int tmp = strlen(result);

            //envoi de la taille de la reponse
            write(sock_desc, &tmp, sizeof(int));

            //envoi la réponse
            int ecrit = write(sock_desc, result, strlen(result));
            if (ecrit == -1)
            {
                perror("echec d'écriture");
            }

            free(result);
        }
    }
}

void operation(char *req, char* result)
{
    char **tab;

    tab = calloc(10, 50);

    split_request(req, tab);

    //Récupérer le numéro de la requête
    char num[3];
    sprintf(num, "%s", tab[0]);
    int num_req = atoi(num);

    switch (num_req)
    {
    case 1:
    {
        /* ville_depart;ville_arrivee;heure_depart */
        char *ville_depart = tab[1];
        char *ville_arrivee = tab[2];
        char *heure_depart = tab[3];

        trouver_train(ville_depart, ville_arrivee, heure_depart, result);
        break;
    }
    case 2:
    {
        /* ville_depart;ville_arrivee;depart_intervalle(depart_1;depart_2)*/

        struct trouver_trains_params params = {
            tab[1], // ville depart
            tab[2], // ville arrivee
            tab[3], // heure depart 0
            tab[4], // heure depart 1
            result
        };
        
        trouver_trains(params);
        strcpy(result, params.result);
        
        break;
    }
    case 3:
    {
        /* ville_depart;ville_arrivee
         * Affiche la liste de tous les trains puis
         * 1. Selectionne le train le meilleur prix
         * 2. Ou le train au temps optimum
         */
        struct trouver_trains_par_trajet params_ = {
            tab[1], // ville depart
            tab[2], // ville arrivee
            result
        };

        trouver_train_par_trajet(params_);
        strcpy(result, params_.result);
        break;
    }
    default:
        break;
    }

    free(tab);
}

/* Décomposition de la requete
*  Selon le délimiteur ;
*/
void split_request(char *req, char **tab)
{
    char *token;
    char copied_command[100];

    strcpy(copied_command, req);

    token = strtok(copied_command, ";");

    int i = 0;
    while (token != NULL)
    {
        tab[i] = calloc(strlen(token), sizeof(char));
        tab[i] = token;
        token = strtok(NULL, ";");
        i++;
    }

}

/* Fonction qui retourne un train satisfaisant le critere
 * ou premier train possible à partir de l'horaire de depart demandé
 */
void trouver_train(char *ville_depart, char *ville_arrivee, char *heure_depart, char *result)
{
    FILE *file = fopen("bd", "r");

    char line[100];
    char result_[100];

    //critère de recherche d'un train: trajet(ville_depart;ville_arrive)
    char trajet[100];
    sprintf(trajet, "%s;%s", ville_depart, ville_arrivee);

    // critere de recherche d'un train: trajet;heure_depart
    char trajet_hd[200];
    sprintf(trajet_hd, "%s;%s", trajet, heure_depart);

    //conversion de l'heure de départ en décimale
    char heure[7];
    replace(heure_depart, ':', '.', heure);
    float h_depart = atof(heure);

    float max = FLT_MAX;

    while (fgets(line, sizeof(line), file)) {
        if(!(strstr(line, trajet) == NULL)) {
            /*
            * Si la ligne ne correspond pas spécifiquement au trajet (heure départ compris)
            * mais les villes de départ et d'arrivée y sont
            */
            char **tab = calloc(8, 50);
            split_request(line, tab);

            //heure du train de la ligne
            char heure_t[7];
            replace(tab[3], ':', '.', heure_t);
            float heure_train = atof(heure_t);

            float diff_heure = heure_train - h_depart;

            if (diff_heure >= 0)
            {
                if (diff_heure < max)
                {
                    max = diff_heure;
                    strcpy(result_, line);
                }
            }
            free(tab);
        }
        
    }

    fclose(file);
    strcpy(result, result_);
}

void replace(char *s, char critere, char replace_caractere, char *new_s) {
    int taille = strlen(s);

    for (int i = 0; i < taille; i++)
    {
        new_s[i] = s[i] == critere ? replace_caractere : s[i];
    }
}

void trouver_trains(struct trouver_trains_params params) {
    FILE *file = fopen("bd", "r");

    char *lines;
    lines = calloc(1000, sizeof(char));

    //critère de recherche d'un train: trajet(ville_depart;ville_arrive)
    char trajet[100];
    sprintf(trajet, "%s;%s", params.ville_depart, params.ville_arrivee);

    //conversion de l'heure de départ en décimale
    char hd_0[7];
    replace(params.depart_interval_0, ':', '.', hd_0);
    float h_depart_0 = atof(hd_0);

    char hd_1[7];
    replace(params.depart_interval_1, ':', '.', hd_1);
    float h_depart_1 = atof(hd_1);

    // lecture de la base de donnée ligne par ligne
    char line[100];
    while (fgets(line, sizeof(line), file)) {

        if(!(strstr(line, trajet) == NULL)) {
            /*
            * Si la ligne contient le critère du trajet
            */
            char **tab = calloc(8, 50);
            split_request(line, tab);


            //heure du train de la ligne
            char hd_t[7];
            replace(tab[3], ':', '.', hd_t);
            float heure_train = atof(hd_t);

            if (h_depart_0 <= heure_train && heure_train <= h_depart_1)
            {
                
                strcat(lines, line);
                strcat(lines, "#");
            }

            free(tab);
            
        }
        
    }

    strcpy(params.result, lines);
    fclose(file);
    free(lines);
}

void trouver_train_par_trajet(struct trouver_trains_par_trajet params) {
    FILE *file = fopen("bd", "r");

    char *lines;
    lines = calloc(1000, sizeof(char));

    //critère de recherche d'un train: trajet(ville_depart;ville_arrive)
    char trajet[strlen(params.ville_depart)+strlen(params.ville_arrivee)+1];

    sprintf(trajet, "%s;%s", params.ville_depart, params.ville_arrivee);

    //lecture de la base de donnée ligne par ligne
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        if(!(strstr(line, trajet) == NULL)) {
            /*
            * Si la ligne contient le critère du trajet
            */
            strcat(lines, line);
            strcat(lines, "#");
        }
        
    }

    strcpy(params.result, lines);
    fclose(file);
    free(lines);
}