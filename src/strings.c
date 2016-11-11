#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/defs.h"

/* This is a function to obtain an element of a string, an element is seperated by a space
 Example:
	String = "The cat sat on the mat"
	get_element(string, strlen(string), return_string, 2)
	Return String = "sat"
 */
void get_element(char *str, int strl, char *rstr, int element)
{
        int ctr = 0;
        int space_count = 0;

        if (! strl) return;

	memset(rstr, '\0', sizeof(rstr));

        do {
                if ( str[ctr] == ' ' ){
                        space_count++;
                        ctr++;
                }
                if ( space_count == element )rstr[strlen(rstr)] = str[ctr];
                ctr++;
        } while(str[ctr] != '\0' && str[ctr] != '\n' && str[ctr] != '\r' && ctr <= strl);

        return;
}

/* This function will cut down a string to the specified length */
void cut_string(char *str, int strl, char *rstr)
{
        int ctr = 0;
        if (! strl) return;
        do{
                if ( str[ctr] != '\n' )  rstr[ctr] = str[ctr];
                ctr++;
        }while( ctr < strl - 1 && str[ctr] != '\0' && str[ctr] != '\n' && ctr < MAX_ACCOUNT_LEN);
        rstr[ctr] = '\n';
        rstr[ctr+1] = '\0';

        return;
}

/* removes the F= etc from the start of a string */
void rem_element(char *str, int strl, char *rstr)
{
	int ctr = 2;
	memset(rstr, '\0', sizeof(rstr));
	do {
		rstr[strlen(rstr)] = str[ctr];
		ctr++;
	} while(str[ctr] != '\0' && str[ctr] != '\n' && str[ctr] != '\r' && ctr <= strl);
	
	return;
}

/* This function will get a users contact address from a string i.e. blah@blah.com */
void get_contactstr(char *str, int strl, char *rstr, int element)
{
        int ctr = 0;
        int space_count = 0;

	char prep[MAX_ACCOUNT_LEN + MAX_GUID_LEN];

        memset(rstr, '\0', sizeof(rstr));
	memset(prep, '\0', sizeof(prep));

        do {
                if ( str[ctr] == ' ' ){
                        space_count++;
                        ctr++;
                }
                if ( space_count == element )prep[strlen(prep)] = str[ctr];
                ctr++;
        } while(str[ctr] != '\0' && str[ctr] != '\n' && str[ctr] != '\r' && ctr <= strl);

	ctr = 2;
	
        do {
                rstr[strlen(rstr)] = prep[ctr];
                ctr++;
        } while(prep[ctr] != '\0' && prep[ctr] != '\n' && prep[ctr] != '\r' && ctr <= strl);

        return;
}
/* Return the number of elements */
int count_element(char *str, int strl)
{
        int ctr = 0;
        int space_count = 0;

        if (! strl) return;

        do {
                if ( str[ctr] == ' ' && ctr < strl - 2){
                        space_count++;
                        ctr++;
                }
                ctr++;
        } while(str[ctr] != '\0' && str[ctr] != '\n' && str[ctr] != '\r' && ctr <= strl);

        return space_count+1;
}
parse_onscreen(char *str, int strl, char *rstr)
{
	/* to do */
}	
