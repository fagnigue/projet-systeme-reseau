#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../headers/dependencies.h"

extern int child_group;
extern int socket_desc;
extern char *bd;
extern FILE *file;

/* Interception de la mort d'un enfant*/
void handler_child(int sig)
{
    wait(NULL);
    printf("intercept\n");
}

/* Interception de l'interruption du programme principal
*  Tuer les enfants du processus
*  Fermer le socket d'écoute
*/
void handler_interruption(int sig) {
    printf("\nInterruption !\n");
    

    // fermeture du socket d'écoute
    close(socket_desc);

    // fermeture du fichier contenant les informations sur les trains
    if (fclose(file) == -1)
    {
        perror("fclose");
    }
    
    if (child_group > 0)
    {
       killpg(child_group, SIGKILL);
    } else
    {
        exit(1);
        printf("\nProcessus arrêté !\n");
    }
    
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

/* Remplace le caractère ':' par le caractère '.'
*  Dans le but de transformer l'heure en un float
*  Exemple: '6:15' -> '6.15'
*  Et effectuer le critère de renvoi du prochain train
*  Dans le cas où celui demandé est inexistant
*/
void replace(char *s, char critere, char replace_caractere, char *new_s) {
    int taille = strlen(s);

    for (int i = 0; i < taille; i++)
    {
        new_s[i] = s[i] == critere ? replace_caractere : s[i];
    }
}

/*  Récupère la taille du fichier 
*   contenant les informations sur
*   les trains
*/
long int fileSize() 
{ 
    FILE* fp = fopen(bd, "r"); 
  
    fseek(fp, 0L, SEEK_END); 
  
    // calcul de la taille du fichier
    long int res = ftell(fp); 
  
    fclose(fp); 
  
    return res; 
}

/*
* Verifie si le port entré est correct
* cette fonction vérifie chaque caractère
* de la chaine s'il est bien un entier
*/
int is_correct_port(char *port) {
    char *numbers = "0123456789";

    int taille = strlen(port);
    int termine = 0;
    int is_ok = 1;
    int i = 0;

    while (i<taille && !termine)
    {
        char caractere[2];
        caractere[0] = port[i];
        
        if (strstr(numbers, caractere) == NULL)
        {
            termine = 1;
            is_ok = 0;
        }
        
        i++;
    }

    return (is_ok == 1) ? atoi(port) : -1;
}