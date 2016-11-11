#include <stdio.h>
#include <string.h>
#include "headers/defs.h"
extern char contactn[MAX_LIST_SIZE][MAX_ACCOUNT_LEN];
extern char contactp[MAX_LIST_SIZE][STATUS_LEN];
extern char contactf[MAX_LIST_SIZE][MAX_NICK_LEN];
extern char contactc[MAX_LIST_SIZE][MAX_GUID_LEN];
extern int blocked[MAX_LIST_SIZE];
extern char clisto[MAX_LIST_SIZE][MAX_ACCOUNT_LEN]; /*online*/
extern char clista[MAX_LIST_SIZE][MAX_ACCOUNT_LEN]; /*away*/
void parse_nick();
void parse_address();

/* Add a contact to the contact list */
void add_contact(char *address, char *friendly, char *guid, int op)
{
        int ctr;
	char friendly_parsed[MAX_NICK_LEN];
	char email_parsed[MAX_ACCOUNT_LEN];

        for(ctr=0;ctr<=MAX_LIST_SIZE;ctr++){
		if (! strcmp(contactn[ctr], address) ) return;
	}
        for(ctr=0;ctr<=MAX_LIST_SIZE;ctr++){
                if (! strlen(contactn[ctr]) ){
			memset(email_parsed, '\0', sizeof(email_parsed));
			parse_address(address, strlen(address), email_parsed);

			strcpy(contactn[ctr], email_parsed);

			memset(friendly_parsed, '\0', sizeof(friendly_parsed));
			parse_nick(friendly, strlen(friendly), friendly_parsed);

                        strcpy(contactf[ctr], friendly_parsed);

                        strcpy(contactc[ctr], guid);
                        blocked[ctr] = op;
                        return;
                }
        }
        return;
}

/* This will return 1 if the contact was already online i.e NLN BSY */
int adjust_contact(char *address, char *friendly, char *status)
{
        int ctr;
	char friendly_parsed[MAX_NICK_LEN];

        for(ctr=0;ctr<=MAX_LIST_SIZE;ctr++){
                if (! strcmp(contactn[ctr], address) ){
                        if ( strlen(friendly) > 0 ){
				memset(friendly_parsed, '\0', sizeof(friendly_parsed));
				parse_nick(friendly, strlen(friendly), friendly_parsed);
				strcpy(contactf[ctr], friendly_parsed);
			}
			if (! strcmp(status, "FLN") ){
				memset(contactf[ctr], '\0', sizeof(contactf[ctr]));
				memset(contactp[ctr], '\0', sizeof(contactp[ctr]));
				return 0;
			}
                        if ( strlen(contactp[ctr]) > 0 ) {
                                strcpy(contactp[ctr], status);
                                return 1;
                        } else strcpy(contactp[ctr], status);
                        return 0;
                }
        }
        return 0;
}

void del_contact(char *address)
{
        int ctr;

        for(ctr=0;ctr<=MAX_LIST_SIZE;ctr++){
                if (! strcmp(contactn[ctr], address) ){
			memset(contactn[ctr], '\0', sizeof(contactn[ctr]));
			memset(contactc[ctr], '\0', sizeof(contactc[ctr]));
                        memset(contactf[ctr], '\0', sizeof(contactf[ctr]));
                        memset(contactp[ctr], '\0', sizeof(contactp[ctr]));
                        return;
                }
        }
        return;
}

/* Clear the contact lists up */
void clear_contact()
{
	int ctr;
	cli_msg("Clearing Lists up\n");
        for(ctr=0;ctr < MAX_LIST_SIZE;ctr++){
                memset(contactn[ctr], '\0', sizeof(contactn[ctr]));
                memset(contactp[ctr], '\0', sizeof(contactp[ctr]));
                memset(contactf[ctr], '\0', sizeof(contactf[ctr]));
                memset(contactc[ctr], '\0', sizeof(contactc[ctr]));
                memset(blocked, 0, sizeof(blocked));
                memset(clisto[ctr], '\0', MAX_ACCOUNT_LEN);
                memset(clista[ctr], '\0', MAX_ACCOUNT_LEN);
        }
	contacts_out(NULL, NULL);
	return;
}

char *get_name(char *guid)
{
        int ctr;
	char *name;
        for(ctr=0;ctr<=MAX_LIST_SIZE;ctr++){
                if (! strcmp(contactc[ctr], guid) ){
			return contactn[ctr];
                }
        }
        return;
}

void parse_address(char *address, int alen, char *raddress)
{
        int ctr = 0;
        do {
                if ( (address[ctr] >= 48 && address[ctr] <= 57) || (address[ctr] >= 63 && address[ctr] <= 90) || (address[ctr] >= 97 && address[ctr] <= 122) || address[ctr] == 45 || address[ctr] == 46 || address[ctr] == 95) raddress[strlen(raddress)] = address[ctr];
                ctr++;
        }while (address[ctr] != '\0' && ctr < alen);
        return;
}

void parse_nick(char *nick, int nicklen, char *rnick)
{
	int ctr = 0;
	do {
		if ( (nick[ctr] >= 32 && nick[ctr] <= 126) ) rnick[strlen(rnick)] = nick[ctr];
//		else cli_msg("UNKNOWN CHAR \\%c\\%d\\\n", nick[ctr], nick[ctr]);
		ctr++;
	}while (nick[ctr] != '\0' && ctr < nicklen);
	return;
}

/* Parse friendly names that are received 
   on connect and when changes are made 
   These are parsed in URL format, i.e
   %20 = ' ' ad %25 = '%'. */
int parse_friendly(char *friendly, int flen, int start, char *rfriendly)
{
	char ch;
	int ctr = start;
	memset(rfriendly, '\0', sizeof(rfriendly));
	do {
		ch = friendly[ctr];
		if ( ch == '%' && friendly[ctr + 1] == '2' && friendly[ctr + 2] == '0' ){
                	ch = ' ';
               		rfriendly[strlen(rfriendly)] = ch;
	                ctr += 2;
		}
                if ( ch == '%' && friendly[ctr + 1] == '2' && friendly[ctr + 2] == '5' ){
                	ch = '%';
               		rfriendly[strlen(rfriendly)] = ch;
                        ctr += 2;
		}
		if ( ch != '\n' && ch != '%' && ch != ' ' && ch != '\r')rfriendly[strlen(rfriendly)] = ch;
                ctr++;
	} while( friendly[ctr] != ' ' && friendly[ctr] != '\n' && ctr < flen);
	return ctr;
}
