/*gcc -o temp main-code.c mail-structure.c interface.c list.c -lncurses*/

#include "list-header.h"
#include "mail-header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>







int main() {
	int user = 2;
    char user_input;

    printf("What would you like to do??");
    printf("w ~ write new mail   r ~ read mail");
    scanf("%c", &user_input);


    Mail* mail;
    if(user_input == 'w') {
    	mail = createMailFromFile(0); /*the base mail*/
    	renderMail(mail, 1);
	    saveMailToFile(mail);
    } else if(user_input == 'r') {
    	int num_of_mail;
    	printf("Please enter the number of the mail you'd like to see: ");
	    scanf("%d", &num_of_mail);
	    mail = createMailFromFile(num_of_mail);
	    renderMail(mail, 0);
    }


    freeMail(mail);




	return 0;
}



