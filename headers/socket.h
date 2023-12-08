#define PORT 5002

int ecouter(struct sockaddr_in p);
int accepter(struct sockaddr_in client_addr, int serv_descr);
void fils();
void communication(int sock_desc);
void operation(char *req);