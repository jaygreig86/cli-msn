#include <stdio.h>
#include "../libs/curl.h"
#include "../libs/types.h"
#include "../libs/easy.h"
#include "headers/ticket.h"
#include <sys/stat.h>
#include <string.h>
#define printf s_msg
extern int s_msg();
extern char certificate[128];
int nexus();
int login_nexus_passport();
int nexus_respond();
int login_parse();
int nexus_redirect();
FILE *headerfile;
CURL *curl;
char recvbuff[2048];
extern int pid;
char loginserv[255];
char errorptr[2048];

char linebuff[2048];
int errorcode;

/* Connect to the nexus server
   Return Values:
   0 - Success
   1 - Software error
*/

int nexus(char *chls, char *username, char *password)
{
	int ctr, tmp, attempt;
	snprintf(tempfile, sizeof(tempfile), "/tmp/cli_msn.%d.tmp", pid);
	printf("Connecting to nexus.passport.com over SSL\n");
        if (! (headerfile = fopen(tempfile, "w+")) ){
                printf("Unable to open temporary file\n");
                return 1;
        }
        if (chmod(tempfile, 0000600) ){
                printf("Security Failure: Temporary file %s is readable by others\n", tempfile);
        }
        if (curl_global_init(CURL_GLOBAL_SSL)){
               	printf("ERROR: libcurl global_init failed\n");
                return 1;
        }
        if (! ( curl = curl_easy_init() ) ){
                printf("ERROR: libcurl easy_init failed\n");
                return 1;
        }

	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorptr);
        if ( curl_easy_setopt(curl, CURLOPT_URL, "HTTPS://nexus.passport.com/rdr/pprdr.asp") ){
                printf("ERROR: libcurl easy_setopt_url failed\n");
                printf("%s", errorptr);
                return 1;
        }
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorptr);
        curl_easy_setopt(curl, CURLOPT_WRITEHEADER, headerfile);
        curl_easy_setopt(curl, CURLOPT_CAINFO, certificate);
        if ((errorcode = curl_easy_perform(curl))){
                printf("ERROR: libcurl easy perform failed\n");
                printf("%s", errorptr);
		if (errorcode == 60 ){
			printf("Set the certificate file path using the -c flag, example:\n \"./msn -c /etc/curl-ca-bundle.crt\"\n");
		}
                printf("Error Code: %d\n", errorcode);
                return 1;
        }

	rewind(headerfile);

	while(!feof(headerfile)){
		fgets(linebuff, sizeof(linebuff), headerfile);
		if ( strstr(linebuff, "PassportURLs")) {
			ctr = 0;
			memset(loginserv, '\0', sizeof(loginserv));
			strcpy(loginserv, "HTTPS://");
			do {
				if ( linebuff[ctr] == 'D' && linebuff[ctr + 1] == 'A' && linebuff[ctr + 2] == 'L' ){
					ctr += 8;
					tmp = 8;
					do {
			                loginserv[tmp] = linebuff[ctr];
			                ctr++;
			                tmp++;
                    			} while ( linebuff[ctr] != ',' );
			        }
				ctr++;
			} while ( linebuff[ctr] != '\0' && linebuff[ctr] != '\n' );
			break;
		}
	}
	printf("Located Login Server: %s\n", loginserv);

        curl_easy_cleanup(curl);
        fclose(headerfile);

	for(attempt=0;attempt<4;attempt++){
		if ( login_nexus_passport(loginserv, username, password, chls) ) return 1;
		switch(nexus_respond()){
			case 0:
			        if ( remove(tempfile) ){
        			        printf("Security Failure: Unable to remove temporary file %s\n", tempfile);
        			}
				return 0;
				break;
			case 1:
                                if ( remove(tempfile) ){
                                        printf("Security Failure: Unable to remove temporary file %s\n", tempfile);
                                }
                                return 1;

				break;
			case 2:
				nexus_redirect();
				break;
		}		
	}

        if ( remove(tempfile) ){
                printf("Security Failure: Unable to remove temporary file %s\n", tempfile);
        }

	return 0;
}

/* Login to the nexus passport server and request auth
   Return Values:
   0 - Success
   1 - File error
*/

int login_nexus_passport(char *loginserv, char *username, char *password, char *chls)
{
	char sendbuff[512];

	snprintf(sendbuff, sizeof(sendbuff), "GET %s HTTP/1.1\r\nAuthorization: Passport1.4 OrgVerb=GET,OrgURL=http%%3A%%2F%%2Fmessenger%%2Emsn%%2Ecom,sign-in=%s,pwd=%s,%s\r\nHost: loginnet.passport.com\r\n\r\n", loginserv, username, password, chls);
 
        if (! (headerfile = fopen(tempfile, "w+")) ){
                printf("Unable to open temporary file\n");
                return 1;
        }
        printf("Logging in to %s\n", loginserv);

        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, loginserv);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, sendbuff);
        curl_easy_setopt(curl, CURLOPT_CAINFO, certificate);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, headerfile);
        curl_easy_setopt(curl, CURLOPT_WRITEHEADER, headerfile);

        if (curl_easy_perform(curl)){
                printf("ERROR: libcurl easy perform failed\n");
                return 1;
        }

        curl_easy_cleanup(curl);
	fclose(headerfile);
	return 0;
}

/* Read the response from the nexus server
   Return Values:
   0 - Success
   1 - Incorrect login info
   2 - Redirection to a different login server
   3 - Bad Request
*/

int nexus_respond()
{
        if (! (headerfile = fopen(tempfile, "r+")) ){
                printf("Unable to open temporary file\n");
                return 1;
        }

	while(!feof(headerfile)){
		memset(linebuff, '\0', sizeof(linebuff));
		fgets(linebuff, sizeof(linebuff), headerfile);
		if ( strstr(linebuff, "HTTP/1.1 200 OK")) {
                        printf("Passport Login Successful\n");
                                login_parse();
	                        return 0;
                }
                if ( strstr(linebuff, "HTTP/1.1 401 Unauthorized")){
                        printf("Passport Login Failed\n");
                        printf("Incorrect Login/Password\n");
                        fclose(headerfile);
/*                        if ( remove(tempfile) ){
                                printf("Security Failure: Unable to remove temporary file %s\n", tempfile);
                        }*/
                        curl_global_cleanup();
                        return 1;
                }
                if ( strstr(linebuff, "HTTP/1.1 302 Found")){
                        printf("Passport Redirection\n");
                        return 2;
                }
	}
	fclose(headerfile);
	printf("Bad Request\n");
	return 3;
}
/* Retrieve the ticket
   Return Values:
   0 - Success
   1 - File error
*/

int login_parse()
{
	int ctr, tmp;
        if (! (headerfile = fopen(tempfile, "r+")) ){
                printf("Unable to open temporary file\n");
                return 1;
        }
        while(!feof(headerfile)){
                fgets(linebuff, sizeof(linebuff), headerfile);
                if ( strstr(linebuff, "Authentication-Info:") ){
                        ctr = 0;
                        do {
				if ( linebuff[ctr] == 'f' && linebuff[ctr + 1] == 'r' && linebuff[ctr + 6] == 'P' && linebuff[ctr + 8] == '\'' ){
                                        ctr += 9;
                                        tmp = 0;
                                        do {
                                                ticket[tmp] = linebuff[ctr];
                                                ctr++;
                                                tmp++;
                                        } while ( linebuff[ctr] != '\'' && linebuff[ctr] != '\0' && linebuff[ctr] != '\n' );

                                        fclose(headerfile);
                                        return 0;
                                }
                                ctr++;
                                tmp++;
                        } while ( linebuff[ctr] != '\0' && linebuff[ctr] != '\n' );
                }
        }

        fclose(headerfile);
        return 0;
}

int nexus_redirect()
{
	int ctr, tmp;
        if (! (headerfile = fopen(tempfile, "r+")) ){
                printf("Unable to open temporary file\n");
                return 1;
        }
        memset(loginserv, '\0', sizeof(loginserv));
        while(!feof(headerfile)){
                fgets(recvbuff, sizeof(recvbuff), headerfile);
                if ( strstr(recvbuff, "Location: ") ){
                        ctr = 10;
                        tmp = 0;
                        do {
                                loginserv[tmp] = recvbuff[ctr];
                                tmp++;
                                ctr++;
                        } while ( recvbuff[ctr] != '\n' && recvbuff[ctr] != '\0' );
                        loginserv[strlen(loginserv) - 1] = '\0';
                }
        }

        if ( remove(tempfile)){
                printf("Security Failure: Unable to remove temporary file %s\n", tempfile);
        }
	return 0;
}

