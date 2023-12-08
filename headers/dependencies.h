#define MAX 100

void handler_child(int sig);
void handler_interruption(int sig);
void split_request(char *req, char **tab);
void replace(char *s, char critere, char replace_caractere, char *new_s);
long int fileSize();
int is_correct_port(char *port);