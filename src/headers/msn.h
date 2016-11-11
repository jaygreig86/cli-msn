char certificate[128];
int result, user;
pid_t pid;
extern WINDOW *_cmd;
extern WINDOW *_status;
extern void elapsed_time();
extern void ping_time();
extern void record_time();
extern char *dispatch();
extern int notification();
extern WINDOW *console;
extern void _window();
extern void cli_msg();
extern void destroy_window();
extern void conf();
extern void resizeHandler();
extern int switch_b();
extern int notifperm_func();
extern int input();
extern int input_case();
extern char msgbody[];

extern char msgqn[MAX_TCP_CON][MAX_ACCOUNT_LEN];
extern char msgqm[MAX_TCP_CON][MAX_MESSAGE_LEN];
extern int args[NO_OF_ARGS][1];
extern int tcp_sock[];
int clearcmd = 0;
void cut_string();

