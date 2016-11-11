#define ERROR_LIST 5
#define COM_LIST 18
extern void destroy_window();
extern char lag[2][8];
extern char nick[384];
extern int png, _time, tid;
extern int clientport;
extern char *MD5Data(char *, unsigned int, char *);
extern char username[130];
extern void sendData();
extern void s_msg();
extern char clientip[16];
extern char who[255];
extern char query[128];
extern char msg[1024];
extern int tcp_socket();
extern void cli_msg();
extern int readData();
int form_ctr = 0;
int gimp = 0;
int spuff;
int op_value;
int ctr2;
int online = 0;
void ping_time();
extern WINDOW *_status, *console, *_contacts;
char commands[COM_LIST][4] = {
{"QNG"},
{"PRP"},
{"LST"},
{"CHL"},
{"MSG"},
{"FLN"},
{"ILN"},
{"NLN"},
{"RNG"},
{"XFR"},
{"ADC"},
{"REM"},
{"BLP"},
{"Cli"},
{"GTC"},
{"NOT"},
{"UBX"},
{"UUX"},
};
char errorlist[ERROR_LIST][4] = {
{"201"},
{"209"},
{"403"},
{"800"},
{"913"}
};
char errordesc[ERROR_LIST][255] = {
{"Error: Unable to add invalid email address\n"},
{"Error: Invalid nickname\n"},
{"Error: Unable to remove non-existent contact\n"},
{"Error: Changing too rapidly\n"},
{"Error: Not allowed when hiding\n"}
};
char buff[2048];

