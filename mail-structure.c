#include "list-header.h"
#include "mail-header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const char names[USERS_NUM][20] = {
	"STONE",
	"HOMEGIVER",
	"STARGAZER",
};




char* getNextAvailableName(int increment) {
	/*the file contains the next name that can be used to name a mail. We read it, then increment the value stated there, and then output the read value*/
    FILE *read_file = fopen("./next-available-name", "r"); /*first we read the file*/
    int available_name;
    fscanf(read_file, "%d", &available_name);
    fclose(read_file);

    if(increment) {
	    FILE *write_file = fopen("./next-available-name", "w"); /*then we clear it and write into it*/
	    fprintf(write_file, "%d", available_name+1);
	    fclose(write_file);
	}

    char* output = (char*) malloc (MAX_SIZE_FOR_FILENAME+1);
    sprintf(output, "%012d", available_name);
    return output;
}


Mail* createMailFromFile(int filename_number) {
	Mail* mail = (Mail*) malloc (sizeof(Mail));


	char filename_complete[8+MAX_SIZE_FOR_FILENAME+1];
	sprintf(filename_complete, "./mails/%012d", filename_number);

	FILE *mail_file;
	mail_file = fopen(filename_complete, "r");


    List* lines = createList(sizeof(List*));

    char mail_line[MAX_SIZE_FOR_FILELINE];
    fgets(mail_line, MAX_SIZE_FOR_FILELINE, mail_file);
    for(int i=0; i<ID_BYTES_NEEDED; i++) {
    	mail->id[i] = (unsigned char) mail_line[i];
    }


	while(fgets(mail_line, MAX_SIZE_FOR_FILELINE, mail_file)) {
		int length = strlen(mail_line);
		if(mail_line[length-1] == '\n') {
			mail_line[length-1] = '\0';
		}
		addLine(lines, mail_line, -1);
	}

	fclose(mail_file);



	mail->lines = lines;




	return mail;
}


void saveMailToFile(Mail* mail) {
	char* filename_number = getNextAvailableName(1);
	

	char filename_complete[8+MAX_SIZE_FOR_FILENAME+1];
	sprintf(filename_complete, "./mails/%s", filename_number);

	FILE* mail_file = fopen(filename_complete, "w");
	for(int i=0; i<ID_BYTES_NEEDED; i++) { /*doing it this way so it also puts '\0' into the file :þ*/
		fputc(mail->id[i], mail_file);
	}
	fputc('\n', mail_file);

	int lines = listSize(mail->lines);
	for(int i=0; i<lines; i++) {
		fputs(readList(getLine(mail->lines, i), "str"), mail_file);
		fputc('\n', mail_file);
	}	
	fclose(mail_file);
}





void freeMail(Mail* mail) {
	if(mail->lines == NULL)
		return;

	for (int i = 0; i < listSize(mail->lines); ++i) {
        List* sublist = *(List**)getListItem(mail->lines, i)->data;
        freeList(sublist);
    }
    freeList(mail->lines);
}



int getRelationToMail(Mail* mail, int user) {
	/*
	00 ~ no relation
	01 ~ CC
	10 ~ receiver
	11 ~ sender
	*/

	int byte_num = user/4;
	int sub_byte = user%4;
	int selected_bytes = 0b00000011 << sub_byte*2;

	return (mail->id[byte_num] & selected_bytes) >> sub_byte*2;
}

void setRelationToMail (Mail* mail, int user, int relation) {
	/*
	00 ~ no relation
	01 ~ CC
	10 ~ receiver
	11 ~ sender
	*/

	int byte_num = user/4;
	int sub_byte = user%4;
	int selected_bytes = ~(0b00000011 << sub_byte*2);
	int replace_by = relation << sub_byte*2;

	int output = (selected_bytes & mail->id[byte_num]) + replace_by;
	mail->id[byte_num] = output;
}