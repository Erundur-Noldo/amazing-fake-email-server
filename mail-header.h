#ifndef MAIL_HEADER_H
#define MAIL_HEADER_H

#define HEADER_NUM 6
#define HEADER_WIDTH 12
#define MAX_NAME_SIZE 30

/*let's hope that these are large enough...*/
#define MAX_SIZE_FOR_FILENAME 12
#define MAX_SIZE_FOR_FILELINE 100

#define USERS_NUM 3
#define ID_BYTES_NEEDED (USERS_NUM >> 2) + !((USERS_NUM & 0b11) == 0) /*this is just a complicated way to say ceil(num_users/4)*/



typedef struct Mail {
	char unsigned id[ID_BYTES_NEEDED+1];
	List* lines;
} Mail;


extern const char names[USERS_NUM][20];







Mail* createMailFromFile (int filename_number);
void  saveMailToFile     (Mail* mail);
void  freeMail           (Mail* mail);
int   getRelationToMail  (Mail* mail, int user);
void  setRelationToMail  (Mail* mail, int user, int relation);







void  addLine     (List* lines, char data[], int pos);
List* getLine     (List* list,  int line_num);
void  renderMail  (Mail* mail,  int mode);
void  renderFrame (List* lines, int x, int y);
void  renderLine  (List* lines, int line_num);


#endif
