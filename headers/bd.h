struct trouver_trains_params
{
    char *ville_depart;
    char *ville_arrivee;
    char *depart_interval_0;
    char *depart_interval_1;
};

struct trouver_trains_par_trajet
{
    char *ville_depart;
    char *ville_arrivee;
};

void trouver_train(char *ville_depart, char *ville_arrivee, char *heure_depart);
void trouver_trains(struct trouver_trains_params params);
void trouver_train_par_trajet(struct trouver_trains_par_trajet params);
void reallocation(char *line);