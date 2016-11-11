/* MSN Conf file processor
 * filename: /src/conf.c
 * Coded by James Greig
 * James@mor-pah.net
 * Date: 30/06/03
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "headers/defs.h"
#include "headers/conf.h"
extern char certificate[128];
extern int args[NO_OF_ARGS][1];
int get_var();
void conf()
{
	int ctr, tmp;
	int flag = 0;
	FILE *fp;
	char ch;
	char *homedir;
	char pre[255];                                  /* Temp Buffer for config lines */
	char home_directory[255];
	char _confstr[MAX_NO_LINES][MAX_LINE_LENGTH];   /* Temp buffer for valid config lines */

	homedir = getenv("HOME");
        if (homedir == NULL)
        {
                homedir = getenv("USER");
                if (homedir == NULL)
                {
                        homedir = getenv("USERNAME");
                        if (homedir == NULL)
                        {
                                cli_msg( stderr, "Could not find user's home directory!\n" );
                        }
                }
		strcpy(home_directory,"/home/");
        }
	
	strcpy(home_directory,homedir);
	strcat(home_directory,"/.msn/msn.conf");

	if ( (fp = fopen(home_directory, "r")) != NULL ){
		cli_msg("Reading msn.conf...\n");
	}
	else {
		cli_msg("No config file found at: %s\n", home_directory);
		cli_msg("Continuing with command line args and defaults\n");
		return;
		}
	ctr = 0;
	while(!feof(fp)) {
		memset(pre, '\0', sizeof(pre));
		fgets(pre, MAX_LINE_LENGTH, fp);
		if (  ( strstr(pre, "user") > 0 ) || ( strstr(pre, "pass") > 0 ) || ( strstr(pre, "cert") > 0 ) || ( strstr(pre, "log") > 0 ) || ( strstr(pre, "away") > 0 ) || ( strstr(pre, "arecon") > 0 )){
			strcpy(_confstr[ctr], pre);
		ctr++;
		}
	}
        /* Username was not specified by an arg switch, so fetch from conf */
        if ( username[0] == 0 || certificate[0] == 0) {
                for(tmp=0;tmp<MAX_NO_LINES;tmp++){
                        if ( _confstr[tmp][0] == 'a' && _confstr[tmp][1] == 'r' && _confstr[tmp][2] == 'e' && _confstr[tmp][3] == 'c' && _confstr[tmp][4] == 'o' && _confstr[tmp][5] == 'n'){
                                char variable[MAX_LINE_LENGTH];
                                memset(variable, '\0', sizeof(variable));
                                if ( get_var(_confstr[tmp], strlen(_confstr[tmp]), variable, 1) ){
                                        cli_msg("Malformed msn.conf at line %d, terminated incorrectly\n", tmp);
                                        _confstr[tmp][strlen(_confstr[tmp]) - 1] = '\0';
                                        cli_msg("\"%s\"\n", _confstr[tmp]);
                                        destroy_window();
                                }
                                if (! strcmp(variable,"yes") ){
                                        cli_msg("Auto Reconnect: Enabled\n");
                                        args[4][0] = 1;
                                } else args[4][0] = 0;
                        }

			if ( _confstr[tmp][0] == 'l' && _confstr[tmp][1] == 'o' && _confstr[tmp][2] == 'g' ){
				char variable[MAX_LINE_LENGTH];
				memset(variable, '\0', sizeof(variable));
				if ( get_var(_confstr[tmp], strlen(_confstr[tmp]), variable, 1) ){
					cli_msg("Malformed msn.conf at line %d, terminated incorrectly\n", tmp);
                                        _confstr[tmp][strlen(_confstr[tmp]) - 1] = '\0';
                                        cli_msg("\"%s\"\n", _confstr[tmp]);
					destroy_window();
				}
				if (! strcmp(variable,"yes") ){
					cli_msg("Logging: Enabled\n");
					args[2][0] = 1;
				} else args[2][0] = 0;
			}
                        if ( _confstr[tmp][0] == 'a' && _confstr[tmp][1] == 'w' && _confstr[tmp][2] == 'a' && _confstr[tmp][3] == 'y' ){
                                char variable[MAX_LINE_LENGTH];
				memset(variable, '\0', sizeof(variable));
                                if ( get_var(_confstr[tmp], strlen(_confstr[tmp]), variable, 1) ){
                                        cli_msg("Malformed msn.conf at line %d, terminated incorrectly\n", tmp);
                                        _confstr[tmp][strlen(_confstr[tmp]) - 1] = '\0';
                                        cli_msg("\"%s\"\n", _confstr[tmp]);
                                        destroy_window();
                                }
                                if ( atoi(variable) ){
                                        cli_msg("AutoAway: Enabled\n");
                                        args[3][0] = atoi(variable) * 60;
                                } else args[3][0] = 0;
                        }

                        if ( _confstr[tmp][0] == 'c' && _confstr[tmp][1] == 'e' && _confstr[tmp][2] == 'r' && _confstr[tmp][3] == 't'){
				memset(certificate, '\0', sizeof(certificate));
                                if ( get_var(_confstr[tmp], strlen(_confstr[tmp]), certificate, 1) ){
                                        cli_msg("Malformed msn.conf at line %d, terminated incorrectly\n", tmp);
                                        _confstr[tmp][strlen(_confstr[tmp]) - 1] = '\0';
                                        cli_msg("\"%s\"\n", _confstr[tmp]);
                                        destroy_window();
                                }
                                cli_msg("Read Certificate locality from msn.conf\n");
                        }

                        if ( _confstr[tmp][0] == 'u' && _confstr[tmp][1] == 's' && \
                                _confstr[tmp][2] == 'e' && _confstr[tmp][3] == 'r' && _confstr[tmp][4] == ' ' && username[0] == 0){
				memset(username, '\0', sizeof(username));
                                if ( get_var(_confstr[tmp], strlen(_confstr[tmp]), username, 1) ){
                                        cli_msg("Malformed msn.conf at line %d, terminated incorrectly\n", tmp);
                                        _confstr[tmp][strlen(_confstr[tmp]) - 1] = '\0';
                                        cli_msg("\"%s\"\n", _confstr[tmp]);
                                        destroy_window();
                                }
				cli_msg("Read username %s from msn.conf\n", username);
				flag = 1;
                        }
			if ( _confstr[tmp][0] == 'p' && _confstr[tmp][1] == 'a' && \
				_confstr[tmp][2] == 's' && _confstr[tmp][3] == 's' && flag){
				memset(password, '\0', sizeof(password));
                                if ( get_var(_confstr[tmp], strlen(_confstr[tmp]), password, 1) ){
                                        cli_msg("Malformed msn.conf at line %d, terminated incorrectly\n", tmp);
                                        _confstr[tmp][strlen(_confstr[tmp]) - 1] = '\0';
                                        cli_msg("\"%s\"\n", _confstr[tmp]);
                                        destroy_window();
                                }
                                cli_msg("Read Password from msn.conf\n");
			} 
                }
        }

	fclose(fp);
	return;
}

/* This is a modified version of get_element as it grabs the config element with the ''s */
int get_var(char *str, int strl, char *rstr, int element)
{
        int ctr = 0;
	int set = 0;
        int space_count = 0;

        if (! strl) return;

        memset(rstr, '\0', sizeof(rstr));

        do {
                if ( str[ctr] == ' ' ){
                        space_count++;
                        ctr++;
                }
                if ( space_count == element ){
			if ( strlen(rstr) == 0 && str[ctr] != 39 && !set) return 1;
			else set = 1;
			if ( str[ctr] != 39 ) rstr[strlen(rstr)] = str[ctr];
		}
                ctr++;
        } while(str[ctr] != '\0' && str[ctr] != '\n' && str[ctr] != '\r' && ctr <= strl);

        return 0;
}

