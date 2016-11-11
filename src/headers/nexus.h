FILE *headerfile;
char sendbuff[1024];
char recvbuff[2048];
CURL *curl;
int ctr, tmp;
char loginserv[255];
char timmay[2048];
extern void *cli_msg(char *, char *, char *, char *, char *);
extern void *s_msg(char *, char *, char *);
extern void *destroy_window();
extern int pid;

