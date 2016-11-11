extern WINDOW *_status, *console;
extern int readData();
extern int check, tid;
extern void s_msg();
extern void cli_msg();
extern char msga[255];
extern char query[128];
extern void sendData();
extern void u_msg();
extern int msg_send();
extern int tcp_socket();
extern char r_msg();
extern int cookie;
extern int msgqueue();
extern char msgqn[MAX_TCP_CON][MAX_ACCOUNT_LEN];
extern char msgqm[MAX_TCP_CON][MAX_MESSAGE_LEN];
extern char convo_type[MAX_TCP_CON][16];                /* Conversation type in open panel (public/private/file) */
extern char convo_users[MAX_TCP_CON][255];              /* Users currently in conversation in open panel */
extern char convo_socket[MAX_TCP_CON][1];               /* Socket number to match with panel, i.e. socket 3 = panel 2 */
