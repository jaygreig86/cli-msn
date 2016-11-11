/* CLI-MSN Global Variables and Constants
 * filename: /src/headers/global.h
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 22/11/05
 */
char recvbuff[1024];				/* General Purpose Receive buffer */
char convoa[MAX_TCP_CON][255];			/* Address of user(s) in an open conversation */
char username[MAX_ACCOUNT_LEN];			/* Storage Buffer for username */
char password[255];				/* Storage Buffer for relative password */
char convot[MAX_TCP_CON][16];			/* Public or private */
char query[128];				/* The buffer to store the address for the query */
char nick[384];					/* NickName Buffer */
char lag[2][8];					/* Storage of status lag o' meter output */
int cid;					/* command ID */
int tid;					/* Transaction ID */
int ii;						/* User number relative to connection number -1 */
extern int i;					/* Connection number (socket number in socket array */

/*unused*/

char convo_type[MAX_TCP_CON][16];                /* Conversation type in open panel (public/private/file) */
char convo_users[MAX_TCP_CON][255];              /* Users currently in conversation in open panel */
char convo_socket[MAX_TCP_CON][1];               /* Socket number to match with panel, i.e. socket 3 = panel 2 */
