#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <float.h>

#include "../headers/bd.h"
#include "../headers/dependencies.h"

extern char *bd;
extern FILE *file;
extern char *result;

/* Fonction qui retourne un train satisfaisant le critère
 * ou premier train possible à partir de l'horaire de depart demandé
 */
void trouver_train(char *ville_depart, char *ville_arrivee, char *heure_depart)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

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
    
    while ((read = getline(&line, &len, file)) != -1) {
        printf("line %s\n", line);
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
                    // augmentation de l'espace mémoire
                    reallocation(line);

                    strcpy(result, line);
                }
            }
            free(tab);
        }
        
    }
    free(line);
}

void trouver_trains(struct trouver_trains_params params) {

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
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

    while ((read = getline(&line, &len, file)) != -1) {
        printf("line %s\n", line);
        if(!(strstr(line, trajet) == NULL)) {
            /*
            * Si la ligne contient le critère du trajet
            */
            char **tab = calloc(8, 50);
            split_request(line, tab);

            // heure du train de la ligne
            char hd_t[7];
            replace(tab[3], ':', '.', hd_t);
            float heure_train = atof(hd_t);

            if (h_depart_0 <= heure_train && heure_train <= h_depart_1)
            {
                // augmentation de l'espace mémoire
                reallocation(line);
                
                printf("line %s\n", line);
                strcat(result, line);
                strcat(result, "#");
            }

            free(tab);
            
        }
        
    }
    free(line);
}

void trouver_train_par_trajet(struct trouver_trains_par_trajet params) {
    
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    //critère de recherche d'un train: trajet(ville_depart;ville_arrive)
    char trajet[strlen(params.ville_depart)+strlen(params.ville_arrivee)+1];

    sprintf(trajet, "%s;%s", params.ville_depart, params.ville_arrivee);

    while ((read = getline(&line, &len, file)) != -1) {
        printf("line %s\n", line);
        if(!(strstr(line, trajet) == NULL)) {
            /*
            * Si la ligne contient le critère du trajet
            */
           
            // augmentation de l'espace mémoire
            reallocation(line);

            strcat(result, line);
            strcat(result, "#");
        }
    }

    free(line);
}

void reallocation(char *line) {
    // taille de l'espace à allouer
    size_t taille_line = strlen(line);

    // recuperer la taille du contenu de result
    size_t taille_contenu = strlen(result);

    // réalloue l'espace mémoire (agrandit l'espace mémoire)
    result = (char *) realloc(result, (taille_contenu + taille_line)*sizeof(char));
}