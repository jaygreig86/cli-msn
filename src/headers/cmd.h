char msgbody[MAX_MESSAGE_LEN + MAX_ACCOUNT_LEN + 30];

extern void *cli_msg();
extern void ctrl_c();
extern void destroy_window();
extern void elapsed_time(void);
extern WINDOW *_cmd, *_status, *_cmdph, *console, *_contacts;
extern void help();

char history[50][255];
char temp[255];
int cmdhist = 1;
int cmdreq = 0;
extern int lc;
void input_case(char *);

extern void contacts_out();
extern int offset;
extern char contactn[MAX_LIST_SIZE][MAX_ACCOUNT_LEN];

