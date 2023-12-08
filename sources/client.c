#include <netinet/in.h>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <strings.h>
#include <string.h>
#define MAX 512
#define TRAJETS_MAX 1000
#define MOT_MAX 6


void envoyerRequete(char *tab, int connect);
void traiterReponse (char *reponseServeur);
void recevoirReponse (int connect);
void enleverDelim (char *line);
void separerEnLignes(char *reponseServeur, int tailleMessage);
void preparation();
void entreeVilles();
void calculPrix(char **t);
void affichageMenu(int connect, int c);
void affichageMenu3(int connect, int c);
void listeTrajet(char **tab);
void prixMin();
void tempsPlusCourt();
void vider();
int convertirParMinutes(char* time);

char *adresseIP;
int port;
char* trajets[TRAJETS_MAX][MOT_MAX];
int tailleTrajet = 0;


void preparation(){

    int p = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in s;
    struct hostent *h;
    char msg[MAX];
    char msgRec[MAX];


    s.sin_family = AF_INET;
    s.sin_port = htons(port);

    h = gethostbyname(adresseIP);

    bcopy((char *)h -> h_addr, (char *)&s.sin_addr.s_addr,h->h_length);

    if(p == -1){
        perror("erreur creation socket \n");
    }else{
        printf("ok creation du socket \n");
    }

    int c = connect(p,(struct sockaddr*) &s, sizeof(s));
    if(c<0){
        perror("erreur connection au serveur \n");
    }
    
    else {
        printf("ok connecte \n");
        while (1){
            affichageMenu(p,c);
        }
    }  
}

void entreeVilles(char *villeDepart, char *villeArrivee){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
    printf("Entrer la ville de départ : ");
    fgets(villeDepart, 100, stdin);
    strtok(villeDepart, "\n");

    printf("Entrer la ville d'arrivée : ");
    fgets(villeArrivee, 100, stdin);
    strtok(villeArrivee, "\n");

}


void envoyerRequete(char *tab, int connect){
    int tailleTab = strlen(tab);

    write(connect, &tailleTab, sizeof(int));

    if (write(connect, tab, tailleTab) == -1) {
        perror("erreur lors du write dans tab");
        exit(1);
    }
    else {
        printf("Requete envoyée\n");
    }
    if(strcmp(tab, "exit") == 0){
        printf("Deconnection\n");
    }
    else{
        bzero(tab, tailleTab);
    }
    

}

void recevoirReponse(int connect){
    int tailleMessage;
    int n ;
    char reponseServeur[MAX] = "";
    bzero(reponseServeur, MAX);

    read(connect, &tailleMessage, sizeof(tailleMessage));

    if(tailleMessage == 0){
        printf("Il n'a aucun train correspondant à ces critères\n");
    }
    else{
        n = read (connect, reponseServeur, tailleMessage);
    
        if (n >= 0 ){
           // printf("%s", reponseServeur);
           printf("Le serveur a repondu \n\n");
        }

        else{
            perror("erreur read");
        }

        separerEnLignes(reponseServeur, tailleMessage);
    }

}

void enleverDelim(char *line){
    char **tab = calloc(10, 12000);
    char *token;
    int i = 0;
    char *delim = ";";

    token = strtok(line, delim);
    
    bzero(tab,sizeof(tab));
    while(token != NULL) {
        tab[i]=token;
        token = strtok(NULL, delim);
        i++;
    }
    calculPrix(tab);
    printf("\n\n");

}


void calculPrix(char **t){
    char **tab = t;
    float prix = 0.0;

    if(tab[6] != NULL){
        prix = atof(tab[5]);
        if(strstr(tab[6],"REDUC") != NULL){
            prix -=  prix*0.2; 
        }
        else if(strstr(tab[6],"SUPPL") != NULL){
            prix += prix*0.1;
        }
    }else{
        prix = atof(tab[5]);
    }
    
    sprintf(tab[5], "%.2f", prix);
    listeTrajet(tab);
   printf("Le train est : Nº %s | %s | %s | %s | %s | Prix: %s euros\n", tab[0], tab[1],tab[2],tab[3],tab[4],tab[5]);

    
}

void prixMin() {
    float minPrix = atof(trajets[0][5]);
    int minIndex = 0;

    for(int k=1; k<tailleTrajet; k++){
        float prix = atof(trajets[k][5]);
        if(prix < minPrix) {
            minPrix = prix;
            minIndex = k;
        }
    }

    printf("-------------------------------------------------------------\n");
    printf("#         Le trajet le moins cher est                  :     \n");
    printf("-------------------------------------------------------------\n");
    printf("##  Reférence  : %s\n", trajets[minIndex][0]);
    printf("-------------------------------------------------------------\n");
    printf("##  Départ     : %s\n", trajets[minIndex][1]);
    printf("------------------------------------------------------------ \n");
    printf("##  Arrivée    :  %s\n", trajets[minIndex][2]);
    printf("-------------------------------------------------------------\n");
    printf("##  Heure depart : %s\n", trajets[minIndex][3]);
    printf("-------------------------------------------------------------\n");
    printf("##  Heure d'arrivée : %s\n", trajets[minIndex][4]);
    printf("-------------------------------------------------------------\n");
    printf("##  Prix       : %s euros\n", trajets[minIndex][5]);
    printf("-------------------------------------------------------------\n\n");
}

int convertirParMinutes(char* time) {
    int hours, minutes;
    sscanf(time, "%d:%d", &hours, &minutes);
    return hours * 60 + minutes;
}
/*
    Calcul et  de train le plus rapide
*/
void tempsPlusCourt() {
    int minTemps = convertirParMinutes(trajets[0][4]) - convertirParMinutes(trajets[0][3]);
    int minIndex = 0;

    for(int k=1; k<tailleTrajet; k++){
        int temps = convertirParMinutes(trajets[k][4]) - convertirParMinutes(trajets[k][3]);
        if(temps < minTemps) {
            minTemps = temps;
            minIndex = k;
        }
    }

    printf("-------------------------------------------------------------\n");
    printf("#         Le trajet le plus court est                  :     \n");
    printf("-------------------------------------------------------------\n");
    printf("##  Reférence  : %s\n", trajets[minIndex][0]);
    printf("-------------------------------------------------------------\n");
    printf("##  Départ     : %s\n", trajets[minIndex][1]);
    printf("------------------------------------------------------------ \n");
    printf("##  Arrivée    :  %s\n", trajets[minIndex][2]);
    printf("-------------------------------------------------------------\n");
    printf("##  Heure depart : %s\n", trajets[minIndex][3]);
    printf("-------------------------------------------------------------\n");
    printf("##  Heure d'arrivée : %s\n", trajets[minIndex][4]);
    printf("-------------------------------------------------------------\n");
    printf("##  Prix       : %s euros\n", trajets[minIndex][5]);
    printf("-------------------------------------------------------------\n\n");
}
/*
    Utilise les variables globales "trajets" et "tailleTrajet"
    pour mettre tous les trajets renvoyés par le serveur dans un tableau pour des traitements ultérieurs
*/
void listeTrajet(char **tab) {
    for(int j=0; j<6;j++){
        trajets[tailleTrajet][j] = malloc(strlen(tab[j]) + 1);
        strcpy(trajets[tailleTrajet][j], tab[j]);
    }
    tailleTrajet++;
}

/*
    Affichage du menu principal de l'application avec les fonctionnalités demandées:
    - Rechercher un train (Ville depart, ville d'arrivée, heure depart)
    - Rechercher les trains dans une plage d'heures de depart
    - Afficher tous les trains entre 2 villes
    * Il prend en paramètres le descripteur et la connection 

*/
void affichageMenu(int connect, int c){
    int clavier_in = 0;
    char tmp[MAX] = "";
    char heureDebut[MAX];
    char villeDepart[MAX];
    char villeArrivee[MAX];
    char heureFin[MAX];


    printf("#################################################################\n");
    printf("###                           MENU                             ##\n");
    printf("#################################################################\n");
    printf("### 1.. Rechercher un train                                    ##\n");
    printf("#################################################################\n");
    printf("### 2.. Trouver les trains pour un intervale d'heure de depart ##\n");
    printf("#################################################################\n");
    printf("### 3.. Lister les trains pour un trajet                       ##\n");
    printf("#################################################################\n");
    printf("### 0.. Quitter                                                ##\n");
    printf("#################################################################\n\n");

    do {
        printf("Veuillez entrer un entier entre 0 et 3 : ");
        if(scanf("%d", &clavier_in) != 1){
            printf("Entrée invalide. Veuillez entrer un entier entre 0 et 3.\n");
            while (getchar() != '\n');
        }

    } while (clavier_in < 0 || clavier_in > 3);

    switch(clavier_in){

        case 1:
            entreeVilles(villeDepart, villeArrivee);

            printf("Entrer l'heure de départ : ");
            scanf("%s", heureDebut);

            strcat(tmp, "1;");
            strcat(tmp, villeDepart);
            strcat(tmp, ";");
            strcat(tmp,villeArrivee);
            strcat(tmp, ";");
            strcat(tmp, heureDebut);
            strcat(tmp, ";");
            envoyerRequete(tmp, connect);
            recevoirReponse(connect);
            vider();
            break;

        case 2:
            entreeVilles(villeDepart, villeArrivee);

            printf("Entrer le début de la plage : ");
            scanf("%s", heureDebut);
            printf("Entrer la fin de la plage : ");
            scanf("%s", heureFin);

            strcat(tmp, "2;");
            strcat(tmp, villeDepart);
            strcat(tmp, ";");
            strcat(tmp, villeArrivee);
            strcat(tmp, ";");
            strcat(tmp, heureDebut);
            strcat(tmp, ";");
            strcat(tmp, heureFin);
            strcat(tmp, ";");

            envoyerRequete(tmp, connect);
            recevoirReponse(connect);
            vider();
            break;

        case 3:
            entreeVilles(villeDepart, villeArrivee);
            strcat(tmp, "3;");
            strcat(tmp, villeDepart);
            strcat(tmp, ";");
            strcat(tmp, villeArrivee);
            strcat(tmp, ";");
            envoyerRequete(tmp, connect);
            recevoirReponse(connect);
            affichageMenu3(connect,c);

            break;

        default : 
            if (clavier_in == 0){
                envoyerRequete("exit", connect);
                printf("Au revoir\n");
                close(c);
                close(connect);
                exit(0);
            }
            break;
    } 
    
}

/*
    Affichage du menu de la 3e fonctionnalité qui nous permet:
    - d'afficher le train le rapide
    - le train au meilleur prix
    * Il prend en paramètres le descripteur et la connection 
*/
void affichageMenu3(int connect, int c){

    printf("################################\n");
    printf("##    CHOIX DU TRAJET         ##\n");
    printf("################################\n");
    printf("### 1.. Le plus rapide        ##\n");
    printf("################################\n");
    printf("### 2.. Le moins cher         ##\n");
    printf("################################\n");
    printf("### 3.. Retour menu principal ##\n");
    printf("################################\n\n");
    int choix;
    do {
        printf("Veuillez entrer un entier entre 1 et 3 : ");
        if(scanf("%d", &choix) != 1){
            printf("Entrée invalide. Veuillez entrer un entier entre 1 et 3.\n");
            while (getchar() != '\n');
        }

    } while (choix < 1 || choix > 3);

    switch(choix){
        case 1:
            tempsPlusCourt();
            affichageMenu3(connect, c);
            break;
        case 2:
            prixMin();
            affichageMenu3(connect, c);
            break;
        default:
            if (choix == 3){
                affichageMenu(connect,c);
            }
        break;
    }
    
}


void separerEnLignes(char *reponseServeur, int tailleMessage){
    char **lignes = (char **) malloc(5000);
    int nbLignes = 0;
    //printf("%s \n", reponseServeur);
    char *ligne = strtok(reponseServeur,"#");

    //lignes = (char **) malloc(sizeof(char*));
    while(ligne != NULL){
       // lignes = (char **) realloc(lignes, (nbLignes+1)*sizeof(char*)); 
        lignes[nbLignes] = ligne;
        nbLignes++;
        ligne = strtok(NULL, "#");
    }
    for(int i = 0; i < nbLignes; i++){
        enleverDelim(lignes[i]);
    }
}

void vider(){
    for(int i = 0; i<TRAJETS_MAX; i++){
         for(int j = 0; j<MOT_MAX; j++){
            free(trajets[i][j]);
        }
    }
    tailleTrajet = 0;
}
int main(int argc, char *argv[]){


    if(argc < 2){
        printf("Il manque 2 arguments \n");
        printf("arg 1 : IP ou nom du serveur\n arg 2 : Nº de port\n");
        exit(0);   
    }
    else if(argc < 3){
        printf("Il manque  argument \n");
        printf("argc 2 : Nº de port\n");   
        exit(0);     
    }
    else{
        adresseIP = argv[1];
        port = atoi(argv[2]);
    }
    preparation();
    return 0;
}